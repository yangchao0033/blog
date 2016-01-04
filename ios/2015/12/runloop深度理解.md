#RunLoop深度理解
##RunLoop的概念
一般来讲，一个线程一次只能执行一个任务，执行完成后线程就会退出。如果我们需要一个机制，让线程能随时处理事件但并不退出，通常的代码逻辑是这样的：
```swift
function loop() {
    initialize();
    do {
        var message = get_next_message();
        process_message(message);
    } while (message != quit);
}
```
这种模型通常被称作 Event Loop。 Event Loop 在很多系统和框架里都有实现，比如 Node.js 的事件处理，比如 Windows 程序的消息循环，再比如 OSX/iOS 里的 RunLoop。实现这种模型的关键点在于：如何管理事件/消息，如何让线程在没有处理消息时休眠以避免资源占用、在有消息到来时立刻被唤醒。

所以，RunLoop 实际上就是一个对象，这个对象管理了其需要处理的事件和消息，并提供了一个入口函数来执行上面 Event Loop 的逻辑。线程执行了这个函数后，就会一直处于这个函数内部 "接受消息->等待->处理" 的循环中，直到这个循环结束（比如传入 quit 的消息），函数返回。

OSX/iOS 系统中，提供了两个这样的对象：NSRunLoop 和 CFRunLoopRef。
CFRunLoopRef 是在 CoreFoundation 框架内的，它提供了纯 C 函数的 API，所有这些 API 都是线程安全的。
NSRunLoop 是基于 CFRunLoopRef 的封装，提供了面向对象的 API，但是这些 API 不是线程安全的。

CFRunLoopRef 的代码是[开源](http://opensource.apple.com/source/CF/CF-855.17/CFRunLoop.c)的，你可以在这里 [http://opensource.apple.com/tarballs/CF/CF-855.17.tar.gz](http://opensource.apple.com/tarballs/CF/CF-855.17.tar.gz) 下载到整个 CoreFoundation 的源码。为了方便跟踪和查看，你可以新建一个 Xcode 工程，把这堆源码拖进去看。
##RunLoop与线程的关系
首先，iOS 开发中能遇到两个线程对象: pthread_t 和 NSThread。过去苹果有份文档标明了 NSThread 只是 pthread_t 的封装，但那份文档已经失效了，现在它们也有可能都是直接包装自最底层的 mach thread。苹果并没有提供这两个对象相互转换的接口，但不管怎么样，可以肯定的是 pthread_t 和 NSThread 是一一对应的。比如，你可以通过 pthread_main_thread_np() 或 [NSThread mainThread] 来获取主线程；也可以通过 pthread_self() 或 [NSThread currentThread] 来获取当前线程。CFRunLoop 是基于 pthread 来管理的。

苹果不允许直接创建 RunLoop，它只提供了两个自动获取的函数：CFRunLoopGetMain() 和 CFRunLoopGetCurrent()。 这两个函数内部的逻辑大概是下面这样:
```objc
/// 全局的Dictionary，key 是 pthread_t， value 是 CFRunLoopRef
static CFMutableDictionaryRef loopsDic;
/// 访问 loopsDic 时的锁
static CFSpinLock_t loopsLock;
 
/// 获取一个 pthread 对应的 RunLoop。
CFRunLoopRef _CFRunLoopGet(pthread_t thread) {
    OSSpinLockLock(&loopsLock);
    
    if (!loopsDic) {
        // 第一次进入时，初始化全局Dic，并先为主线程创建一个 RunLoop。
        loopsDic = CFDictionaryCreateMutable();
        CFRunLoopRef mainLoop = _CFRunLoopCreate();
        CFDictionarySetValue(loopsDic, pthread_main_thread_np(), mainLoop);
    }
    
    /// 直接从 Dictionary 里获取。
    CFRunLoopRef loop = CFDictionaryGetValue(loopsDic, thread));
    
    if (!loop) {
        /// 取不到时，创建一个
        loop = _CFRunLoopCreate();
        CFDictionarySetValue(loopsDic, thread, loop);
        /// 注册一个回调，当线程销毁时，顺便也销毁其对应的 RunLoop。
        _CFSetTSD(..., thread, loop, __CFFinalizeRunLoop);
    }
    
    OSSpinLockUnLock(&loopsLock);
    return loop;
}
 
CFRunLoopRef CFRunLoopGetMain() {
    return _CFRunLoopGet(pthread_main_thread_np());
}
 
CFRunLoopRef CFRunLoopGetCurrent() {
    return _CFRunLoopGet(pthread_self());
}
```
综上：线程和RunLoop之间是一一对应的，并且关系是保存在一个全局的字典中的，并且线程刚创建时是没有RunLoop的，如果你不获取它，他一直都不会有，RunLoop的创建发生在第一次获取的时候，RunLoop的销毁发生在线程结束时。并且只能在线程内部获取runloop（主线程除外）。
# RunLoop 对外的接口
在 CoreFoundation 里面关于 RunLoop 有5个类:

CFRunLoopRef
CFRunLoopModeRef
CFRunLoopSourceRef
CFRunLoopTimerRef
CFRunLoopObserverRef

其中 CFRunLoopModeRef 类并没有对外暴露，只是通过 CFRunLoopRef 的接口进行了封装。他们的关系如下:
![image](http://blog.ibireme.com/wp-content/uploads/2015/05/RunLoop_0.png)
一个 RunLoop 包含若干个 Mode，每个 Mode 又包含若干个 Source/Timer/Observer。每次调用 RunLoop 的主函数时，只能指定其中一个 Mode，这个Mode被称作 CurrentMode。如果需要切换 Mode，只能退出 Loop，再重新指定一个 Mode 进入。这样做主要是为了分隔开不同组的 Source/Timer/Observer，让其互不影响。

**CFRunLoopSourceRef**是事件产生的地方。Source有两个版本：Source0 和 Source1。
• Source0 只包含了一个回调（函数指针），它并不能主动触发事件。使用时，你需要先调用 CFRunLoopSourceSignal(source)，将这个 Source 标记为待处理，然后手动调用 CFRunLoopWakeUp(runloop) 来唤醒 RunLoop，让其处理这个事件。
• Source1 包含了一个 mach_port 和一个回调（函数指针），被用于通过内核和其他线程相互发送消息。这种 Source 能主动唤醒 RunLoop 的线程，其原理在下面会讲到。

**CFRunLoopTimerRef**是基于时间的触发器，他和NSTimer是toll-free bridged的，可以混用。其包含一个时间长度和一个回调（函数指针）。
**CFRunLoopObserverRef**是观察者，每个Observer都包含了一个回调（函数指针），当RunLoop的状态发生变法时，观察者就能通过回调接受这个变化。可以观测的时间点有以下几个：
```objc
typedef CF_OPTIONS(CFOptionFlags, CFRunLoopActivity) {
    kCFRunLoopEntry         = (1UL << 0), // 即将进入Loop
    kCFRunLoopBeforeTimers  = (1UL << 1), // 即将处理 Timer
    kCFRunLoopBeforeSources = (1UL << 2), // 即将处理 Source
    kCFRunLoopBeforeWaiting = (1UL << 5), // 即将进入休眠
    kCFRunLoopAfterWaiting  = (1UL << 6), // 刚从休眠中唤醒
    kCFRunLoopExit          = (1UL << 7), // 即将退出Loop
};
```

##RunLoop的Mode
CFRunLoopMode和CFRunLoop的结构大致如下：
```objc
struct __CFRunLoopMode {
    CFStringRef _name;            // Mode Name, 例如 @"kCFRunLoopDefaultMode"
    CFMutableSetRef _sources0;    // Set
    CFMutableSetRef _sources1;    // Set
    CFMutableArrayRef _observers; // Array
    CFMutableArrayRef _timers;    // Array
    ...
};
struct __CFRunLoop {
    CFMutableSetRef _commonModes;     // Set
    CFMutableSetRef _commonModeItems; // Set<Source/Observer/Timer>
    CFRunLoopModeRef _currentMode;    // Current Runloop Mode
    CFMutableSetRef _modes;           // Set
    ...
};
```
这里有个概念叫：“CommonModes”一个Mode可以将自己标记为“Common”属性（通过将其ModeName添加到RunLoop得“commonModes”中）。每当RnuLoop的内容发生变化时，RunLoop的都会自动将_commonModeItems里的Source/Timer/Observer同步到具有“Common”标记的所有的Mode。
应用举例：主线程 RunLoop 默认会预制两个 Mode ：kCFRunLoopDefaultMode 和 UITrackingRunLoopMode。这两个Mode都已经被标记为“Common”属性。DefaultMode是App平时所处的状态，TrackingRunLoop是为了追踪ScrollView滑动时的状态。当你创建一个Timer并加到DefaultMode时，Timer会得到重复回调。此时滑动TableView时，RunLoop会将mode切换为TrackingRunLoopMode，这时Timer就不会被回调，也不会影响滑动的操作了。具体用例类似于在tableView中加入滚动广告栏，当你在操作tableView时会回调自动滚动栏的Timer，造成滚动栏的滑动出现卡顿。

有时你需要一个Timer，在两个Mode中都能得到回调，一种办法就是讲这个Timer加入两个Mode。还有一种就是讲Timer加入到顶层的RunLoop的“commonModeItems”中。“commonModeItems”被RunLoop自动更新到所有具有“Common”属性的Mode里去。

CFRunLoop对外暴漏的管理Mode的接口只有下面两个：
```objc
CFRunLoopAddCommonMode(CFRunLoopRef runloop, CFStringRef modeName); // 给RunLoop添加到CommonMode中
CFRunLoopRunInMode(CFStringRef modeName, ...); // 返回当前线程中指定mode的CFRunLoop对象
```
Mode暴露的管理mode item的接口有下面几个
```objc
CFRunLoopAddSource(CFRunLoopRef rl, CFRunLoopSourceRef source, CFStringRef modeName);// 添加一个CFRunLoopSource对象到一个run loop mode中（如果添加的Source是source0的话，这个方法将会调用 schedule 回调在source的上下文结构（context structure）的指定方法）。一个runloop source 可以同时被注册到多个 runloop 和 runloop modes 中。当source被发出信号，无论哪一个被注册的 runloop 都会开始检测第一个发出信号的 source 。 如过rl的mode中已经包含source时，这个方法将不会做任何事。
CFRunLoopAddObserver(CFRunLoopRef rl, CFRunLoopObserverRef observer, CFStringRef modeName); // 添加CFRunLoopObserver对象到一个run loop mode中去。 讨论：一个 runloop 观察者只能被同时注册在一个 runloop 中，尽管它可以被通过他的tunloop添加到多个runloop modes中。
如果rl已经在 mode中 包含 obsever 中，这个方法将不会做任何事。
CFRunLoopAddTimer(CFRunLoopRef rl, CFRunLoopTimerRef timer, CFStringRef mode);
CFRunLoopRemoveSource(CFRunLoopRef rl, CFRunLoopSourceRef source, CFStringRef modeName); // 添加CFRunLoopTimer 对象到一个runloop mode中
CFRunLoopRemoveObserver(CFRunLoopRef rl, CFRunLoopObserverRef observer, CFStringRef modeName);
CFRunLoopRemoveTimer(CFRunLoopRef rl, CFRunLoopTimerRef timer, CFStringRef mode);
```