#iOS性能优化 --- 界面卡顿产生的原因和解决方案

##卡顿产生的原因和解决方案
![image](http://blog.ibireme.com/wp-content/uploads/2015/11/ios_frame_drop.png)
在 VSync 信号到来后，系统图形服务会通过 CADisplayLink 等机制通知 App，App 主线程开始在 CPU 中计算显示内容，比如视图的创建、布局计算、图片解码、文本绘制等。随后 CPU 会将计算好的内容提交到 GPU 去，由 GPU 进行变换、合成、渲染。随后 GPU 会把渲染结果提交到帧缓冲区去，等待下一次 VSync 信号到来时显示到屏幕上。由于垂直同步的机制，如果在一个 VSync 时间内，CPU 或者 GPU 没有完成内容提交，则那一帧就会被丢弃，等待下一次机会再显示，而这时显示屏会保留之前的内容不变。这就是界面卡顿的原因。
CPU 和 GPU 不论哪个阻碍了显示流程，都会造成掉帧现象。所以开发时，也需要分别对 CPU 和 GPU 压力进行评估和优化。

###CPU 资源消耗原因和解决方案
####对象创建
对象创建时，尽量使用轻量级对象，比如CALayer代替UIView，当你的视图并不需要处理触摸事件时。还有对象创建尽量放在后台线程进行，所有Storyboard创建对象时，其资源消耗要大得多。性能敏感的界面不提倡。
尽量推迟对象的创建时间，并将创建动作分散到多个任务中。如果对象可以复用，并且复用的代价小于创建和销毁的对象时，这类对象尽量放在一个缓存池中进行复用。比如官方的tableViewCell缓存。
####对象调整
尤其是CALayer，他的内部没有属性，当调用他的属性时，内部会使用runtime的resolveInstanceMethod方法为对象临时添加一个方法，并把对象属性值保存在一个内部Dictionary里，同时还会通知代理创建动画等，消耗大量资源。并且UIView的一些和显示相关的属性（frame/bounds/transform）等属性实际上都是CALayer属性上映射来的，消耗的资源远大于其他属性，所以在应用尽量不要过多的修改这些属性。

当视图层次调整，UIView和CALayer之间会出现很多方法与通知，所以在性能优化时，应该尽量避免视图层次调整，添加和移除视图。（考虑使用hide代替）
####对象销毁
通常当容器类持有大量对象时，其销毁对象的资源消耗非常明显，同样，如果对象可以放到后台线程中释放，会减少对主线程的资源消耗小tips：将对象用block捕获，然后扔到后台队列中去随便发送个消息以避免编译器警告，就可以保证对象在后台销毁了
```objc
NSArray *tmp = self.array;
self.array = nil;
dispatch_async(queue, ^{
    [tmp class];
});
```
####布局计算
App中最为常见的消耗CPU资源的位置，如果能在后台线程提现计算好试图布局，并对试图布局进行缓存，那么这个地方就基本不会产生太多性能问题。
不论采用什么技术惊醒视图布局，最终都会落在frame/bounds/center等属性的调整上，上面也说过，对这些属性的调整消耗资源，所以需要在后台提前计算好布局，在需要时一次性调整好这些属性，而不是多次频繁地计算和调整这些属性。

####AutoLayout
这项技术本身受到苹果官方的提倡技术，在大部分情况下有着不俗的表现，但是在复杂的界面中会产生严重的性能问题，并且随着视图的增长，对CPU的消耗呈指数型增长,具体可见[http://pilky.me/36/](http://pilky.me/36/).如果对于frame等属性的调整可以使用一些工具方法代替，或者使用ComponentKit或AsyncDisplayKit等框架

####文本计算

当界面中包含大量文本计算时（微博盆友圈），文本的宽高计算会消耗大量的资源，当只有一些简单的文本展示使用UILable的内部实现方式用 `[NSAttributedString boundingRectWithSize:options:context:] `来计算文本宽高, 用`-[NSAttributedString drawWithRect:options:context:]` 来绘制文本.尽管性能表现不错，但仍需放到后台避免阻塞主线程。

####文本渲染

屏幕上能看到的所有文本控件，包括UIWebView都是底层通过CoreText排版，绘制为Bitmap显示的，常见的控件中UILabel、UITextView文本控件，都是在主线程排版的，当显示大量文本时，CUP压力会很大，对此的解决方案一般都是自定义文本控件，用TextKit或底层的CoreText对文本异步绘制，实现非常麻烦，但优势也是显而易见的，CoreText创建好后，能获取文本的宽高信息，避免了多次计算（UILabel调整尺寸时计算一遍，UILabel绘制时内存再算一遍），CoreText对象内存占用较少，可以缓存下来多次渲染。

####图片的解码

用 UIImage 或CGImageSource创建图片时，图片不会立即解码。图片设置到UIImageView或者CALayer.contents中去，并且CALayer被提交到GPU之前CGImage中的数据才会得到解码。这一步是发生在主线程的，并且不可避免。如果要想绕开这个机制，常见的做法是在后台线程先把图片绘制到CGBitMapContext中，然后从BitMap中直接创建图片，目前网络图片库都自带这个功能。

####图像的绘制

图像的绘制通常都是以CG开头的方法吧图像绘制到画布中，然后从画布创建图片并显示的过程。常见为`[UIView drawRect:]`里面。由于CoreGraphic方法通常都是Thread Safe，所以图像绘制很容易放置到后台执行。一个简单的异步绘制过程大致如下（实际情况更加复杂，但是原理基本一致）
```objc
- (void)display {
    dispatch_async(backgroundQueue, ^{
        CGContextRef ctx = CGBitmapContextCreate(...);
        // draw in context...
        CGImageRef img = CGBitmapContextCreateImage(ctx);
        CFRelease(ctx);
        dispatch_async(mainQueue, ^{
            layer.contents = img;
        });
    });
}
```

###GPU 资源消耗原因和解决
方案
相对于 CPU 来说，GPU 能干的事情比较单一：接收提交的纹理（Texture）和顶点描述（三角形），应用变换（transform）、混合并渲染，然后输出到屏幕上。通常你所能看到的内容，主要也就是纹理（图片）和形状（三角模拟的矢量图形）两类。

####纹理的渲染

所有的 Bitmap，包括图片、文本、栅格化的内容，最终都要由内存提交到显存，绑定为 GPU Texture。不论是提交到显存的过程，还是 GPU 调整和渲染 Texture 的过程，都要消耗不少 GPU 资源。当在较短时间显示大量图片时（比如 TableView 存在非常多的图片并且快速滑动时），CPU 占用率很低，GPU 占用非常高，界面仍然会掉帧。避免这种情况的方法只能是尽量减少在短时间内大量图片的显示，*尽可能将多张图片合成为一张进行显示*。

当图片过大，超过 GPU 的最大纹理尺寸时，图片需要先由 CPU 进行预处理，这对 CPU 和 GPU 都会带来额外的资源消耗。目前来说，iPhone 4S 以上机型，纹理尺寸上限都是 4096x4096，更详细的资料可以看这里：[iosres.com](http://iosres.com/)。所以，尽量不要让图片和视图的大小超过这个值。

####视图的混合

当多个视图（CALayer）重叠在一起时，GPU会首先将她们混合起来，这样会消耗很多GPU资源，为了减轻这种情况的消耗，应该尽量减少视图数量和层次，并在不透明的视图中标明opaque属性为YES以避免Alpha通道的合成（告诉系统这个视图后面没有其他需要渲染的视图，避免多余的性能消耗），也可以用之前的方法将多个视图预先合成为一张图。

####图形的生成

CALayer 的 border、圆角、阴影、遮罩（mask），CASharpLayer 的矢量图形显示，通常会触发离屏渲染（offscreen rendering），而离屏渲染通常发生在 GPU 中。当一个列表视图中出现大量圆角的 CALayer，并且快速滑动时，可以观察到 GPU 资源已经占满，而 CPU 资源消耗很少。这时界面仍然能正常滑动，但平均帧数会降到很低。为了避免这种情况，可以尝试开启 **`CALayer.shouldRasterize`** 属性，但这会把原本离屏渲染的操作转嫁到 CPU 上去。对于只需要圆角的某些场合，也可以用**`一张已经绘制好的圆角图`**片覆盖到原本视图上面来模拟相同的视觉效果。最彻底的解决办法，就是把需要显示的图形在后台线程绘制为图片，避免使用圆角、阴影、遮罩等属性。

参考原文：[iOS 保持界面
流畅的技巧]
(http://
blog.ibireme.com/2015/11/12/smooth_user_interfaces_for_ios/)