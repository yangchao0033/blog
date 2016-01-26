## 使用 Run Loop 对象
一个 run loop 对象提供了一些主要接口用于向你的 run loop 中添加 input source ，timers， 和run loop observer，并且运行它。每一条线程有且只有一个run loop 与他相关联。在 Cocoa 中，这个对象是 [NSRunLoop](https://developer.apple.com/library/ios/documentation/Cocoa/Reference/Foundation/Classes/NSRunLoop_Class/index.html#//apple_ref/occ/cl/NSRunLoop) 类的一个实例。在底层的应用中，它是指向 [CFRunLoopRef](https://developer.apple.com/library/ios/documentation/CoreFoundation/Reference/CFRunLoopRef/index.html#//apple_ref/c/tdef/CFRunLoopRef) 这种不透明类型的一个指针。

## 获取 Run Loop 对象
你需要使用以下其中之一来获取当前线程的 Run Loop ：
<!--more-->

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

表 3-2 展示了一个线程入口的常用的例行程序。示例代码的关键部分展示了一个 run loop 的基础架构。本质上，你将 input sources 和 timers 添加到你的 runloop 中，然后重复的调用其中一个例行程序来启动 run loop 。每一次例行程序返回时，你需要检查一下是否满足可能会退出线程的条件。示例使用了 Core Foundation 的框架的例行程序以便检查返回结果并且可以决定如何退出 runloop。如果你是用的是 Cocoa ，你也可以使用类似的方式通过 NSRunLoop 的方法去运行 runloop ，	并且不需要检查返回值。（使用 NSRunLoop 的方法的例子可以参考 表3-14.）

表 3-2 运行 runloop

```objc
- (void)skeletionThreadMain {
    // 如果你的应用没有使用垃圾回收 请在这里添加 自动释放池（ps：这示例代码也太老了，谁还用垃圾回收啊）
    
    BOOL done = NO;
    
    // 给 runloop 添加 source 或timer，然后做一些其他的配置
    
    do {
        // 开启 runloop 并且被一个 source 被处理后要返回
        /** SInt32 32位有符号整数 */
        SInt32 result = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 10, YES);
        
        // 如果 source 已经显式的停止了 runloop ，或者根本不存在任何 source 或 timer，将会退出。
        if ((result == kCFRunLoopRunStopped) || (result == kCFRunLoopRunFinished)) {
            done = YES;
            // 在这里检查任何其他符合退出的条件并且按需设置 done 变量的值。
        }
    } while (!done);
    
    // 在这里清除代码。确保释放任何之前创建的自动释放池。
}
```
可以递归开启 runloop，换句话说，你可以使用 input source 或者 timer 的例行程序来调用 [CFRunLoopRun](https://developer.apple.com/library/ios/documentation/CoreFoundation/Reference/CFRunLoopRef/index.html#//apple_ref/c/func/CFRunLoopRun),[CFRunLoopRunInMode](https://developer.apple.com/library/ios/documentation/CoreFoundation/Reference/CFRunLoopRef/index.html#//apple_ref/c/func/CFRunLoopRunInMode)或者任何 NSRunLoop 的 runloop 启动方法。这样做你可以使用任何你想用的 mode 来运行一个 嵌套的 run loop ，包括 通过外层 run loop 使用的 mode 。

### 退出 RunLoop
有两种途径可以让 runloop 在处理事件之前退出：

* 使用超时值配置 runloop 运行。
* 直接告诉 runloop 停止（ps：。。。这条太搞了）。

使用超时值无疑是更偏爱的方法，如果你能管理它，指定一个超时值使 runloop 结束所有他的正常处理的任务， 包括在退出前向 runloop observer 发送通知。

使用 [CFRunLoopStop](https://developer.apple.com/library/ios/documentation/CoreFoundation/Reference/CFRunLoopRef/index.html#//apple_ref/c/func/CFRunLoopStop) 函数显示地停止 runloop，产生的结果和超时相似。runloop 会发送任何 runloop 提醒通知然后才退出。不同的是你可以将这项技术应用在你用无条件方式开启的 runloop 上。

尽管移除一个 runloop 的 input source 和 timer 可以造成 runloop 的退出，但这并不是一个可靠的方式来停止 runloop 。一些系统例行程序给 runloop 添加一些 input source 来处理必要的事件。你的代码可能无法看出这些 input source，你可能不能移除这些用来防止 runloop  退出的 source。

### 线程安全 和 Run Loop 对象
线程安全大多取决于你用来操作 runloop 的API。Core Foundation 函数 一般来说都是线程安全的，所以可以被任何线程调用。假如你正在执行一个修改 runloop 配置的操作，那么继续吧，对拥有 runloop 的线程来说这样做仍然是很好的作法。

Cocoa 的 `NSRunLoop` 类内部不像 Core Foundation 中的接口那样是线程安全的。如果你要使用 NSRunLoop 类去修改你的 runloop，你只能在 runloop 所在的线程中这样做。先其他线程中的 runloop 中添加 input source 或 timer 会引起你的程序崩溃或出现不可预知的异常。


### 配置 run loop source

接下来的章节将展示如何在 Cocoa 和 Core Foundation 中设置不同类型的 input source。

#### 定义一个自定义自定义 input source

创建一个自定义的 input source 你需要实现以下这些条件：

* 你想要你的 source 处理的信息
* 一段调度模块的例行程序让感兴趣的客户机了解如何连接你的 input source。
* 一段处理模块例行程序用来处理任何客户机发送的请求
* 一段取消模块的例行程序用来销毁你的 source

因为你创建了一个自定义的 input source 来处理自定义的信息，所以实际上的配置会设计的非常灵活。调度模块，处理模块和取消模块的例行程序几乎都是你的自定义 input source 的关键例行程序。剩下的大多数 input source 行为都发生在这些例行处理程序之外。比如，由你来定义一个工具用来将数据传到你的 input source并且传递你的 input source 的数据到其他线程中去。

插图 3-2 展示了一个简单的自定义 input source 的配置。在本例中，应用程序主线程维持引用了input source ， input source 的缓冲模块，还有安装 input source 的 runloop。当主线程有一个任务向切换到工作子线程中去，他会发送一个命令，命令缓冲区以及启动任务所需的任何线程的信息（因为主线程和工作子线程的 input source 都有权限去访问命令缓冲区，访问必须同步）一旦命令发送了，主线程会发送信号给 input source 来唤醒工作子线程的 runloop。一旦受到唤醒的命令， runloop 会调用 input source 的处理程序 去处理命令缓存器中缓存的命令。

图 3-2 操作一个自定义 input source

![图 3-2](https://developer.apple.com/library/ios/documentation/Cocoa/Conceptual/Multithreading/Art/custominputsource.jpg)

接下来的章节将会解释如何通过上图实现一个自定义 input source 并展示你需要实现的关键代码。

### 定义 input source
定义一个自定义 input source 需要使用 Core Foundation 的例行程序配置你的 runloop input source 并且 将它与你的 runloop 关联。尽管基础处理程序是基于 C-语言 函数的，但这不会阻止你使用 Objective-C 或者 C++ 去封装它为面向对象的代码。

插图3-2中介绍的 input source 使用一个 objective-C 对象去管理一个命令缓存器，并与 runloop 进行协调。列表3-3 展示了这个对象的定义。`RunLoopSource` 对象管理一个命令缓冲器，并且使用命令缓存器接受来自其他线程的消息。该表也展示了 `RunLoopContext` 对象的定义，该对象仅仅是一个容器，用来传递一个 `RunLoopSource` 对象和应用主线程的 runloop 引用。

表 3-3 自定义 input source 对象的定义
```objc
@interface YCRunLoopSource : NSObject
{
    CFRunLoopSourceRef runLoopSource;
    NSMutableArray *commands;
}

- (id)init;
// 添加
- (void)addToCurrentRunLoop;
// 销毁
- (void)invalidate;

// 处理方法
- (void)sourceFired;

// 用来注册需要处理的命令的客户机接口
- (void)addCommand:(NSInteger)command withData:(id)data;
- (void)fireAllCommandsOnRunLoop:(CFRunLoopSourceRef)runloop;

// 这些是CFRunLoopRef 的回调函数
/** 调度函数 */
void RunLoopSourceScheduleRoutine(void *info, CFRunLoopRef r1, CFStringRef mode);
/** 处理函数 */
void RunLoopSourcePerformRoutine (void *info);
/** 取消函数 */
void RunLoopSourceCancelRoutine (void *info, CFRunLoopRef rl, CFStringRef mode);

@end

// RunLoopContext 是一个 在注册 input source 时使用的容器对象

@interface YCRunLoopContext : NSObject
{
    CFRunLoopRef runLoop;
    YCRunLoopSource *source;
}
/** 持有 runloop 和 source */
@property (readonly) CFRunLoopRef runLoop;
@property (readonly) YCRunLoopSource *source;

- (id)initWithSource:(YCRunLoopSource*)src andLoop:(CFRunLoopRef)loop;

@end
```

尽管 Objective-C 代码管理着 input source 的自定义数据。关联一个 input source 到一个具备 基于 C-语言 的回调函数的 runloop 。其中第一个函数是当你实际将 input source 添加到 runloop 中的时刻调用。流程将展示在 表 3-4 中。因为这个 input source 仅只有一个 客户机（主线程）。它使用调度者函数通过目标线程 application 的代理发送消息在目标线程注册自己。当 application 的代理和 input source 进行通信时 ,会使用 RunLoopContext 对象中的 `info` 信息来完成这个事。

表 3-4 调度 run loop source

```objc
void RunLoopSourceScheduleRoutine(void *info, CFRunLoopRef r1, CFStringRef mode){
    YCRunLoopSource *obj = (__bridge YCRunLoopSource *)info;
    // 这里的 Appdelegate 是主线程的代理 	
    AppDelegate *del = [AppDelegate sharedAppDelegate];
    
    // 上下文对象中持有source自己
    YCRunLoopContext *theContext = [[YCRunLoopContext alloc] initWithSource:obj andLoop:r1];
    // 通过代理去注册 Source 自己 
    [del performSelectorOnMainThread:@selector(registerSource:) withObject:theContext waitUntilDone:NO];
    
}
```
其中最重要的回调例行程序是当你的 input source 被信号激活时处理自定义数据的部分。表3-5中展示了与 `RunLoopSource` 对象关联的执行者回调例行程$序,这个函数仅仅转发用来 `sourceFired` 方法工作的请求，该请求用来处理任何 `command buffer` （命令缓冲区）中存在的命令。

表3-5 input source 中的执行者
```objc
void RunLoopSourcePerformRoutine (void *info)
{
    RunLoopSource*  obj = (RunLoopSource*)info;
    [obj sourceFired];
}
```
如果你使用 `CFRunLoopSourceInvalidate` 函数将 input source 从 runloop 重移除。系统会调用你的 input source 中的取消者例行程序。你可以利用这个例行程序去通知客户机你的 input source 不再可用并且他们应该移除任何自己的相关的引用。表3-6 展示了取消者例行回调程序通过 RunLoopSource 对象进行注册。这个函数发送另一个 RunLoopContext 对象给 application 代理。但是这让代理去移除 runloop surce 的相关引用。

表3-6 销毁一个 input source

```objc
void RunLoopSourceCancelRoutine (void *info, CFRunLoopRef rl, CFStringRef mode)
{
    RunLoopSource* obj = (RunLoopSource*)info;
    AppDelegate* del = [AppDelegate sharedAppDelegate];
    RunLoopContext* theContext = [[RunLoopContext alloc] initWithSource:obj andLoop:rl];
 
    [del performSelectorOnMainThread:@selector(removeSource:)
                                withObject:theContext waitUntilDone:YES];
}
```

`
笔记：应用代理方法  registerSource: 和 removeSource 方法在下面的章节 《协调 input source 的客户机》展示
`

###为 runloop 安装 input source
表3-7 展示了 `RunLoopSource` 类的 `init` 方法 和 `addToCurrentRunLoop` 方法。`init` 方法创建了 [CFRunLoopSource](https://developer.apple.com/library/ios/documentation/CoreFoundation/Reference/CFRunLoopSourceRef/index.html#//apple_ref/c/tdef/CFRunLoopSourceRef) 不透明类型的必须关联到 runloop 的对象。它会传递 `RunLoopSource` 对象自己作为 山下文信息 以便于例行回调程序有一个指向对象的指针。input source 直到线程唤起 `addToCurrentRunLoop` 方法时才会执行安装，准确将在 RunLoopSourceScheduleRoutine 回调函数调用时。 一旦 input source 安装到 runloop 中，线程将会运行自己的 runloop 去等待 input source 发出事件。

表3-7 安装 run loop source

```objc
- (id)init {
    // 创建上下文容器，其中会连接自己的 info，retain info release info，还会关联三个例行程序。
    CFRunLoopSourceContext context = {0, (__bridge void *)(self), NULL, NULL, NULL ,NULL, NULL, &RunLoopSourceScheduleRoutine, RunLoopSourceCancelRoutine, RunLoopSourcePerformRoutine};
    /** 通过索引，上下文，和CFAllocator创建source */
    runLoopSource = CFRunLoopSourceCreate(NULL, 0, &context);
    commands = [[NSMutableArray alloc] init];
    return  self;
}

- (void)addToCurrentRunLoop{
    CFRunLoopRef runLoop = CFRunLoopGetCurrent();
    CFRunLoopAddSource(runLoop, runLoopSource, kCFRunLoopDefaultMode);
}
```

###协调 input source 的客户机

对于你的 input source 会非常有用，你需要操作它并且从其他线程向它提供消息。input source 的要点是将其添加到线程并睡眠直到有事情要做时才唤醒。事实上很有必要让其他线程了解 input surce 并且有方法可以和它交流（沟通数据）。

通知你的 input source 客户机的方法之一是发出注册请求 当你的 input source 第一次安装到你的 runloop 中时。你可以向你的 input source 注册尽可能多的客户机。或者你仅仅只是简单的用一些中央机构，然后将你的 input source 声明为感兴趣的客户端进行注册。表3-8 展示了  通过代理 和 调用唤起定义的 注册方法 当 RunLoopSource 对象的调度者函数被调用时。这个方法将会收到 RunLoopSource 提供的 RunLoopContext 对象并且将它添加到他的 source 列表中。这个表也会展示 当 input source 从 他的 runloop 中被移除时 用来注销的例行程序。
表 3-8 使用 application 的 代理 注销并且移除 input source
 
```objc
 #import "YCRunLoopSource.h"
 #import "YCRunLoopContext.h"
@interface AppDelegate : NSObject
@property (nonatomic, strong) NSMutableArray *sourcesToPing;

/** 应该是一个单例 */
+ (instancetype)sharedAppDelegate;
- (void)registerSource:(YCRunLoopContext *)context;
- (void)removeSource:(YCRunLoopContext *)context;

@end

static AppDelegate *_instance;
@implementation AppDelegate

+ (instancetype)sharedAppDelegate
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instance = [[self alloc] init];
    });
    return _instance;
}

- (void)registerSource:(YCRunLoopContext *)context
{
    [self.sourcesToPing addObject:context];
}

- (void)removeSource:(YCRunLoopContext *)context
{
    id objToRemove = nil;
    
    for (YCRunLoopContext *contextObj in self.sourcesToPing) {
        if ([contextObj isEqual:context]) {
            objToRemove = contextObj;
            break;
        }
    }
    
    if (objToRemove) {
        [self.sourcesToPing removeObject:objToRemove];
    }
}

- (NSMutableArray *)sourcesToPing {
    if (_sourcesToPing == nil) {
        _sourcesToPing = @[].mutableCopy;
    }
    return _sourcesToPing;
}
@end
```

**未完待续..**

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

**未完待续。。**


