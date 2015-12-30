#** [Effective Objective-C 2.0]**

## ***第12条*** 

### *** 理解消息转发机制 *** 

消息转发分为两大阶段。  
#### 1、 动态方法解析   
>    <pre><code>\+ (BOOL)resolve__Instance__`（或Class）`Method:(SEL)selector</code></pre>   
>    在继续往下执行之前，本类有机会新增一个处理选择子的方法。  
>    使用这种办法的`前提`：相关方法的实现代码已经写好，只等这运行的时候动态插入类里面就行。该方案通常用来实现`@dynamic`属性

#### 2、 备援接受者
> 	如果第一步返回`NO`，则会来到这里  
> 	<pre><code>\- (id)forwardingTargetForSelector:(SEL)selector</code></pre>    
> 	该方案可以用`组合(composition)`模拟出`多重继承(mutipleInheritance)`
> 	的某些特性，在这里可以交由其他对象处理，让外界感觉是自己处理的.
> 	
> 	`注意：`我们无法操作经由这一步所转发的消息。如果想在此之前修改消息内容，只能采取完整的消息转发机制。 

#### 3、 完整消息转发


>	如果转发机制已经跳过前两步，那么只能采用完整的消息转发进行处理。

>  1. 创建`NSInvocation`对象，把尚未处理的所有那条消息有关的细节封装进去，包括`selector`、`target`、以及`argument`等。  
>  2. 在触发`NSInvocation`对象时，`message-dispatch system`会将消息指派给目标对象.同时触发以下方法:
>  <pre><code>\- (void)forwardInvocation:(NSInvocation *)invocation</code></pre> 
>  该方案其实现方法与`备援接受者`等效，很少有人使用。

### 消息转发全流程
> ![消息转发全流程](http://www.th7.cn/d/file/p/2014/08/16/44221f37772ba4c6e0e003752d2daff4.jpg)
> 接收者在每一步都有机会处理消息，步骤越往后，处理消息的代价就越大。最好第一步就能解决，`runtime`可以将此方法缓存。如果第二步能解决，就不往后迁移。步骤越往后代价越大。