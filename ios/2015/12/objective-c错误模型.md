---
layout: post
title: "Objective-C错误模型"
date: 2016-01-04 11:04:51 +0800
comments: true
categories: ios
keywords: objective-c,NSError
description: 详细描述objective-c的错误处理泛型和异常处理
---

## 为什么不使用异常处理机制？
事实上，OC是具备异常处理机制的，但是具体情况下不会启用该机制。原因是ARC下默认不是“异常安全”的。
	理由是,如果抛出异常，那么本应该在作用域末尾释放的对象在现在不能自动释放了。 如果想生成“异常安全”的代码，需要设置编译标志“-fobjc-arc-exceptions”，并且需要引入额外的代码，在不抛出异常时，也照样执行这段代码。
及时不适用ARC，也很难在抛出异常后不会导致内存泄露。例如：
```objc
	id resource = [YCResource alloc] init] retain];
	if (/*有异常发生*/) { 
	@throw [NSException exceptionWithName:@"/*异常名称*/" reason@"/*异常原因*/" userInfo:nil];
	}
	[resource callSomeMethod];
	[resource release];	
```
以上代码：如果一旦有异常抛出，那么之后的代码都不会被执行，资源resource对象也就无法被释放。
虽然可以在异常发生前释放资源对象，但是如果需要释放的资源对象非常多，之后又有新的资源加入，往往会忘了在异常前释放新加入的资源。
OC现在所采取的方法是只有在极其罕见的异常下，才抛出异常，异常抛出后不必考虑恢复问题，此时程序应该直接退出，这样就不用在编写复杂的“异常安全”代码了。
现在异常只用于及其严重的错误（致命错误）。
其中一个用途：OC中没有抽象类，所以无法定义抽象方法，此时，如果你想定义一套抽象 的API，那么可以在方法的实现中抛出异常，告诉使用者需要覆写该方法：
```objc
- (void)absructMethd {
	NSString exceptionReason = [NSString stringWithFormat:@"%@ must be overridden", NSStringFromSelector(_cmd)];
	@throw [NSException exceptionWithName:NSInternalInnconsistencyException reson:exceptionReason userInfo:nil];
}
```

##如何处理“不那么严重（非致命）”的错误？
OC的范式是：另方法返回0/nil，或者使用NSError。

##如何使用`NSError`？
NSErrow使用起来非常灵活，我们使用它告诉调用者发生了什么错误。
NSError构成：
* Error domain(错误范围)
描述错误发生的范围。例如处理URL的子系统，当url解析出现错误，就可以使用`NSURLErrorDomain`来表示错误范围。
* Error code（错误码）
独有的错误代码，用以表明具体发生了何种错误，一般用枚举定义，HTTP请求中可以存储状态码。
* User info（用户信息）
有关错误的额外附加信息，可以是一段本地化描述，也可能是用来存储该错误是由其他错误引起的，最终生成一条完整的错误链。

其应用场景类型：
* 在代理协议中返回NSError错误。
```objc
- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError - *)error;
```
这样，接口定义者和使用者都可以选择是否输入并处理该错误。
* 在方法中用对象指针来监测错误。
```objc
- (BOOL)doSomething:(NSError **)error;
/*使用方法*/
NSError *error = nil;
BOOL ret = [slef doSomething:&error];
if (ret) {
		// 处理错误
}
```
这种方法一般都会返回BOOL值，用来判断是否操作成功，如果只是判断成功状态，则只要判断返回值并且error参数传为nil就行，而error指针可以用来输出错误信息。
`
笔记：实际上ARC时，编译器会把NSError**转换为NSError*__autorelease*`,当doSomething方法执行完毕后，会将调用者创建的对象自动释放掉。这是为了防止调用者不一定能确保会释放掉NSError对象，必须要求他自己可以自动释放掉，所以加入autorelease。这样就与打部分返回值具备的语义相同了。 

doSomething方法的实现原理：
```objc
- (BOOL)doSomething:(NSError *)error {
		if(/*发生error*/) {
				if(error) {
				/*给外部传入参数初始化*/
				*error = [NSError errorWithDomain:domain code:code userinfo:userInfo];
				}
				return NO;
		} else {
				return YES;
		}
}
```
在给error解引用--`*error`之前，必须保证error不为空。 

一般情况下，给error对象填入恰当的错误范围，错误码，错误信息后，调用者就可以更精准的定位错误。
我们一般对错误范围使用NSString全局变量进行定义。错误码常采用枚举，例如：
```objc
extern NSSring *const YCErrorDomain;
typedef NS_ENUM（NSInteger, YCError）{
	YCErrorUnknow	= -1,
	YCErrorInternalIncosistency  = 100,
	YCErrorGeneralFault  = 105,
	YCErrorBadInput  = 500,
};
NSString *const YCErrorDomain = @"YCErrorDomain";
```
