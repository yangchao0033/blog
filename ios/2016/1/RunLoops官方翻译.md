---
layout: post
title: "RunLoop深度探究（四）"
date: 2016-01-08 18:45:07 +0800
comments: true
categories: ios
---

## Run Loops 官方翻译

Run loops 是与线程相关联的基础设施的一部分。Run loop 是用来调度工作并且协调传入事件的时间处理循环。run loop 的目的是：让你的线程在有工作的任务的事后保持忙碌，并且在空闲的事后使线程保持休眠。

<!--more-->

Run loop 的管理并不是完全自动的，你仍然需要设计你的线程代码，并利用这些代码在合适的时机开启 run loop 并且相应传入的事件。Cocoa 和 Core Foundation 框架都提供了run loop 对象，以便于帮助你配置和管理你的线程 run loop。你的应用不需要显式的创建这些 run loop 对象。每条线程，包括应用的主线程，都会配有一个相关联的 run loop 对象。只有子线程们需要显式的（手动）运行它们的 run loop。然而，该 app 框架将会自动设置并且运行作为应用程序启动过程的一部分的处于 main thread （主线程）的 run loop。

接下来，我们一起看看关于 run loop 以及它们的配置相关的更多信息。参照[NSRunLoop Class Reference](https://developer.apple.com/library/ios/documentation/Cocoa/Reference/Foundation/Classes/NSRunLoop_Class/index.html#//apple_ref/doc/uid/TP40003725) 以及 [CFRunLoop Reference](https://developer.apple.com/library/ios/documentation/CoreFoundation/Reference/CFRunLoopRef/index.html#//apple_ref/doc/uid/20001441-CH1g-112707)。

### Run Loop 剖析

run loop 是一个名副其实的循环。他会是你的线程不停地循环工作，具体包括线程的进入并且用来响应输入源所运行的事件处理程序。你的代码提供的控制状态将会被用来实现run loop具体的循环部分。换句话说，你的代码需要提供 while 或 for 循环来驱动你的 run loop。在你的 run loop 中，你使用一个 run loop 对象去“执行”接收事件的事件处理代码和调用已经安装好的 handlers。

run loop 通过两种不同类型的 source 来接收事件。Input source 传递异步事件，通常包括从其他线程或者其他应用发来的 message 。Timer source 也传递异步事件，通常发生在一个被安排好的时间或者重复的时间间隔。这两种类型的 source 都会使用 application-specific 处理例程来处理到来的事件。

插图3-1展示了 run loop 各种 source 的概念结构。input source 传递异步事件到相对应的 handler（处理程序）并且引起 `runUtilDate:` 方法（调用线程相关的 NSRunLoop 对象）退出。

_插图3-1 run loop 和它的 source 结构_
![插图3-1](https://developer.apple.com/library/ios/documentation/Cocoa/Conceptual/Multithreading/Art/runloop.jpg)

除了处理 input source，run loop 同样也会生成一些关于 run loop 的行为的通知。注册 run-loop observers（观察者）去接收这些通知并且使用他们去在线程上做额外的处理。你可以使用 Core Foundation 框架去在你的线程上安装 run-loop observer 。

下面这些段落提供了更多地关于 runloop的组成 和 它们可以操作的modes，它们还描述了在事件处理的不同时间点生成的通知。

### Run Loop Modes
一个 run loop mode 是一个包含 input sources 和 被监视的 timers 和 将要被通知的 run-loop 的observer 所组成的集合。每次你启动你的 run loop 时，你都会指定（隐式或者显式的）一种特定的“mode”来运行。在执行该运行循环的期间，只有与指定运行的 mode 相关联的 source 才会被检测和允许去发送他们的事件。（相同的，只有与指定运行的 mode 相关联的 observer 会被 run loop 的进度（progress）所通知）。关联到其他 modes 的 sources 直到 run loop 切换到对应的 mode 时才会继续让新的事件通行（只有到对应的mode下，相关的 source 才会继续其对应的功能）。

在你的代码中，可以使用名字来标记 modes，包括 Cocoa 和 Core Foundation 都定义了一些默认的mode和一些常用的mode，代码中你可以通过字符串来区别这些mode，你可以定义自定义的mode，仅仅通过指定简单的字符串来标记这些自定义的mode。尽管可以随意的对 mode 名字进行复制，但是这些 mode 的内容却不行。你必须确保你添加到得任何你创建的 mode 中的一个或多个input sources, timers, 或者 run-loop observer都是有用的。

你使用 modes 并通过你的 run loop 特定的扫描中过滤掉干扰的事件。大多数情况下，你都会将你的 run loop 运行在系统默认的 mode 中。模态面板可能会运行在“ modal ” mode 中。当处于这种 mode 下，只有和 source 关联的模态面板才可以发送事件到线程中去。对于子线程来说，你可能会使用自定义 modes 去防止优先级抵的 source 在时间要求严格的操作中发送事件。