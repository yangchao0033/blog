# 配置一个基于 port 的 input source
Cocoa 和 Core Foundation 都支持用于和线程间或者进程间通信的基于 端口的对象。接下来的章节将会向你展示如何使用一些不同类型的 port 构建 port 通信。

### 配置一个NSMachPort Object
使用 [NSMachPort](https://developer.apple.com/library/ios/documentation/Cocoa/Reference/Foundation/Classes/NSMachPort_Class/index.html#//apple_ref/occ/cl/NSMachPort) 对象创建一个本地连接。你创建一个 port 对象并把它添加到你的主线程 run loop 中去。当启动你的子线程时，你要传一些相同的对象到你的线程入口点函数中去。子线程可以使用相同的对象发送信息回到你的主线程中去。

### 实现主线程代码
表 3-12 中展示了用于启动子工作线程的主线程代码。因为 Cocoa 框架执行很多介入步骤用于配置 port 和 run loop ，`launchThread` 方法