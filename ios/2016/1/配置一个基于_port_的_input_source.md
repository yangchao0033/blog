## 使用 Run Loop 对象
一个 run loop 对象提供了一些主要接口用于向你的 run loop 中添加 input source ，timers， 和run loop observer，并且运行它。每一条线程有且只有一个run loop 与他相关联。在 Cocoa 中，这个对象是 [NSRunLoop](https://developer.apple.com/library/ios/documentation/Cocoa/Reference/Foundation/Classes/NSRunLoop_Class/index.html#//apple_ref/occ/cl/NSRunLoop) 类的一个实例。在底层的应用中，它是指向 [CFRunLoopRef](https://developer.apple.com/library/ios/documentation/CoreFoundation/Reference/CFRunLoopRef/index.html#//apple_ref/c/tdef/CFRunLoopRef) 这种不透明类型的一个指针。

## 获取 Run Loop 对象
你需要使用以下其中之一来获取当前线程的 Run Loop 。

* 在 Cocoa 中，使用 [NSRunLoop](https://developer.apple.com/library/ios/documentation/Cocoa/Reference/Foundation/Classes/NSRunLoop_Class/index.html#//apple_ref/occ/cl/NSRunLoop) 的类方法 [currentRunLoop](https://developer.apple.com/library/ios/documentation/Cocoa/Reference/Foundation/Classes/NSRunLoop_Class/index.html#//apple_ref/occ/clm/NSRunLoop/currentRunLoop) 去拿到一个 `NSRunLoop` 对象。
* 使用 [CFRunLoopGetCurrent](https://developer.apple.com/library/ios/documentation/CoreFoundation/Reference/CFRunLoopRef/index.html#//apple_ref/c/func/CFRunLoopGetCurrent) 函数。

尽管这两种方法不是 [toll-free bridged type](https://developer.apple.com/library/mac/documentation/General/Conceptual/CocoaEncyclopedia/Toll-FreeBridgin/Toll-FreeBridgin.html)(在Foundation 和 Core Foundation 中拥有等价替换接口的能力的类型)的类型,但是如果你需要可以从 `NSRunLoop` 对象里拿到 CFRunLoopRef 这种`不透明类型`(苹果封装在内部的C语言类型)。`NSRunLoop` 类定义了 `getCFRunLoop` 方法用来返回一个可以传入到 Core Foundation 代码中的 `CFRunLoopRef` 类型的C语言指针对象（结构体指针）。这两种对象都可以来自于同一个 run loop，你可以根据你的需要来选择具体使用 `NSRunLoop` 和 `CFRunLoopRef` 这两种对象的哪一种。

## 配置 Run Loop

在你运行一个子线程的 run loop 之前，你必须向其添加至少一个 input source 或者 timer。如果 run loop 没有任何需要监视的 source， 它将会在你尝试运行它的时候立即退出。请参考[配置RunLoop Sounce](https://developer.apple.com/library/ios/documentation/Cocoa/Conceptual/Multithreading/RunLoopManagement/RunLoopManagement.html#//apple_ref/doc/uid/10000057i-CH16-SW7)（本文接下来的章节将有介绍）。

除了安装 source，你还可以 run loop observer 并且使用他们检测 runloop的处于不同执行阶段。为了安装 run loop observer ，你需要创建一个 CFRunLoopObserverRef 不透明类型的指针并使用 [CFRunLoopAddObserver](https://developer.apple.com/library/ios/documentation/CoreFoundation/Reference/CFRunLoopRef/index.html#//apple_ref/c/func/CFRunLoopAddObserver) 函数将 Observer 添加到你的 run loop 中去，Run Loop Observer 必须使用 Core Foundation 框架接口创建，在 Cocoa 应用中也一样。

表 3-1 展示了在线程 runloop 中，添加 run loop Observer 的主要代码流程。本例的目的旨在告诉你如何创建一个 run loop Observer， 所以代码只是简单设置了一个run loop Observer 用来监视 run loop 的所有活动 。基本的处理代码（没有展示）仅仅是日志输出 run loop 的各项活动行为 作为 timer 的事件回调。

表3-1 创建 runloop Observer
```objc
- (void)threadMain {
    // 应用使用垃圾回收，所以不需要 自动释放池 autorelease pool
    NSRunLoop *myRunLoop = [NSRunLoop currentRunLoop];
    // 创建一个 run loop observer 并且将他添加到当前 run loop 中去
    /*!
     *  @author 杨超, 16-01-13 15:01:45
     *
     *  @brief CFRunLoopObserverContext 用来配置 CFRunLoopObserver 对象行为的结构体
     typedef struct {
        CFIndex	version;
        void *	info;
        const void *(*retain)(const void *info);
        void	(*release)(const void *info);
        CFStringRef	(*copyDescription)(const void *info);
     } CFRunLoopObserverContext;
     *
     *  @param version 结构体版本号，必须为0
     *  @param info 一个程序预定义的任意指针，可以再 run loop Observer 创建时为其关联。这个指针将被传到所有 context 多定义的所有回调中。
     *  @param retain 程序定义 info 指针的内存保留（retain）回调,可以为 NULL
     *  @param release 程序定义 info 指针的内存释放（release）回调，可以为 NULL
     *  @param copyDescription 程序定于 info 指针的 copy 描述回调，可以为 NULL
     *
     *  @since
     */
    CFRunLoopObserverContext context = {0 , (__bridge void *)(self), NULL, NULL, NULL};
    CFRunLoopObserverRef observer = CFRunLoopObserverCreate(kCFAllocatorDefault, kCFRunLoopAllActivities, YES, 0, &myRunLoopObserverCallBack, &context);
    
    if (observer) {
        CFRunLoopRef cfLoop = [myRunLoop getCFRunLoop];
        CFRunLoopAddObserver(cfLoop, observer, kCFRunLoopDefaultMode);
    }
    
    // 创建并安排好 timer
    [NSTimer scheduledTimerWithTimeInterval:0.1 target:self selector:@selector(doFireTimer) userInfo:nil repeats:YES];
    NSInteger loopCount = 10;
    do {
        // 3秒后运行 run loop 实际效果是每三秒进入一次当前 while 循环
        [myRunLoop runUntilDate:[NSDate dateWithTimeIntervalSinceNow:3]];
        loopCount --;
    } while (loopCount);
}

void myRunLoopObserverCallBack(CFRunLoopObserverRef observer, CFRunLoopActivity activity, void *info) {
    NSLog(@"observer正在回调\n%@----%tu----%@", observer, activity, info);
}

- (void)doFireTimer {
    NSLog(@"计时器回调");
}
```

当为一个长期存活的现场配置 runloop 时，至少添加一个 input source 去接收消息。尽管你可以仅仅使用一个 关联的timer 就可以进入 run loop，一旦 timer 启动，通常都会被作废掉，这将会硬气 run loop 的退出。关联一个重复执行的 timer 定时器可以保持让 runloop 在很长的一段时期内得以运行，但是需要周期性的去启动定时器 timer 来唤醒你的线程，这是投票有效的另一种形式（这句莫名其妙，不懂是干吗的）。相比之下， input source 会等待事件的发生，并保持线程处于睡眠状态直到事件确实发生了。


## 开动 run loop
在应用中，只有在子线程中才是有必要开启 run loop 的，一个 run loop 必须至少有一个用来监视的 input source 。如果一个关联的都没有，run loop 将会立即退出。

下面有一些方法开启 run loop：
* 无条件的
* 通过一套时间限制
* 在一个特别的 mode 下

无条件的进入你的 run loop 是最简单的选项，但这种也是最不可取的。无条件地运行你的 run loop 将会使你的线程进入进入永久的循环中，这使你很难控制运行循环本身。你可以添加和移除 input source 和 timer，但是只有一种方式去停止 run loop，那就是将它杀死。同时也不存在在自定义 mode 中运行 run loop 的方法。

为了替代无条件的运行 run loop ，更好的办法是使用超时值来运行 runloop。当你使用超时值时，run loop 会一直运行直到在事件来临时 或者 分配的时间结束时。当你的事件到达时，系统会分配一个 handler 去处理它，并且之后 run loop 会退出。你可以用代码重启你的 run loop 以便处理下一个事件。如果不想继续使用刚才分配时间结束的原则，也可以简单的重启 runloop 或者使用这些时间去做任何你需要做的事。

除了使用超时值，你也可以使用指定的 mode 运行 run loop。mode 和超时值不会互相排斥，并且都可以用来启动一个线程。

表 3-2 展示了一个线程入口的常用的代码模板。



### 配置一个基于 port 的 input source
Cocoa 和 Core Foundation 都支持用于和线程间或者进程间通信的基于 端口的对象。接下来的章节将会向你展示如何使用一些不同类型的 port 构建 port 通信。

### 配置一个NSMachPort Object
使用 [NSMachPort](https://developer.apple.com/library/ios/documentation/Cocoa/Reference/Foundation/Classes/NSMachPort_Class/index.html#//apple_ref/occ/cl/NSMachPort) 对象创建一个本地连接。你创建一个 port 对象并把它添加到你的主线程 run loop 中去。当启动你的子线程时，你要传一些相同的对象到你的线程入口点函数中去。子线程可以使用相同的对象发送信息回到你的主线程中去。

### 实现主线程代码
[表 3-12](#list3-12) 中展示了用于启动子工作线程的主线程代码。因为 Cocoa 框架执行很多介入步骤用于配置 port 和 run loop ，Cocoa 的 `launchThread` 方法相比于 Core Foundation 的等价功能[表 3-17](#list3-17)更加简洁明了。尽管如此，这两个框架在这一模块的功能表现基本都是相同的。其中一个存在的差异是与发送本地 port 到工作线程的方式不同，这个方法是直接发送 NSPort 对象的。

**表 3-12** <span id="list3-12">Main Thread lauch method</span>

```objc
- (void)launchThread {
    NSPort *myPort = [NSMachPort port];
    if (myPort) {
        // 这个类处理即将过来的 port 信息
        [myPort setDelegate:self];
        // 将此端口作为 input source 安装到当前 run loop 中去
        [[NSRunLoop currentRunLoop] addPort:myPort forMode:NSDefaultRunLoopMode];
        // 开启工作子线程，让工作子线程去释放 port
        [NSThread detachNewThreadSelector:@selector(LaunchThreadWithPort:) toTarget:[MyWorkerClass class] withObject:myPort];
    }
}
```

为了设置为线程间双向通信信
道，在
登记信息中，你需要让工作线程发送自己的本地 port 到主线程。接收登记信息是为了让你的主线程知道开动子线程的过程进行的非常顺利，同时也为我们为提供了一种方法去向该线程发送更多信息。

表 3-13 展示了用于主线程的[handlePortMessage:](https://developer.apple.com/library/ios/documentation/Cocoa/Reference/NSPortDelegate_Protocol/index.html#//apple_ref/occ/intfm/NSPortDelegate/handlePortMessage:)方法，这个方法会在线程到达自己的本地 port 时进行调用。当登记信息到达时，该方法将直接从 port 信息中检索子线程的 port 并保存以备后用。

**表 3-13**	<span id = "listing 3-13">处理 Mach port 信息</span>

```objc

# define kCheckinMessage 100

// 处理工作线程的响应的代理方法
- (void)handlePortMessage:(NSPortMessage *)portMessage
{
    unsigned int message = [portMessage msgid];
    // 定义远程端口
    NSPort *distantPort = nil;
    if (message == kCheckinMessage) {
        // 获取工作线程的通信 port
        distantPort = [portMessage sendPort];
        
// 引用计数+1 并 保存工作端口以备后用
        [self storeDistantPort:distantPort];
    } else {
        // 处理其他信息
    }
}

- (void)storeDistantPort:(NSPort *)port {
    // 保存远程端口
}
```

#### 实现子线程代码

对于工作子线程，你必须配置它并且是使用指定的端口进行信息沟通并返回到主线程。

表 3-14 <span id = "liting3-14"> 使用 Mach port 启动子线程 </span>

```objc

```