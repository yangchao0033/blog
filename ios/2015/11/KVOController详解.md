# [KVOController](https://github.com/facebook/KVOController)
（附源码注释：[查看地址](https://github.com/yangchao0033/blog/tree/gh-pages/ios/2015/11/KVOController%E6%BA%90%E7%A0%81%E6%B3%A8%E9%87%8A)）

[![Build Status](https://travis-ci.org/facebook/KVOController.png?branch=master)](https://travis-ci.org/facebook/KVOController)
[![Coverage Status](https://coveralls.io/repos/facebook/KVOController/badge.svg?branch=master)](https://coveralls.io/r/facebook/KVOController?branch=master)
[![Version](https://cocoapod-badges.herokuapp.com/v/KVOController/badge.png)](http://cocoadocs.org/docsets/KVOController)
[![Platform](https://cocoapod-badges.herokuapp.com/p/KVOController/badge.png)](http://cocoadocs.org/docsets/KVOController)
[![Carthage compatible](https://img.shields.io/badge/Carthage-compatible-4BC51D.svg?style=flat)](https://github.com/Carthage/Carthage)

KVO是一个对于基于MVC模式的应用来讲用于层级间同行的非常有用的技术，KVOController是一个基于Cocoa并且久经试验的KVO实现，它提供了一个简单，现代化的API，并且是线程安全的。

优点如下：
- Notification使用blocks，自定义响应动作，或者NSKeyValueObserving回调。
- 无异常移除观察者。
- 内含控制器delloc通知移除实现
- 线程安全针对观察者复活提供特别防卫。– [rdar://15985376](http://openradar.appspot.com/radar?id=5305010728468480).

更多KVO详情可以参考苹果官方文档[Introduction to Key-Value Observing](https://developer.apple.com/library/mac/documentation/Cocoa/Conceptual/KeyValueObserving/KeyValueObserving.html).

## 使用
iOS和OS X的Example都包含了该项目，以下是其中一种使用模式：

```objective-c
// 给观察者创建KVO控制器
FBKVOController *KVOController = [FBKVOController controllerWithObserver:self];
self.KVOController = KVOController;

// 观察clock的date属性
[self.KVOController observe:clock keyPath:@"date" options:NSKeyValueObservingOptionInitial|NSKeyValueObservingOptionNew block:^(ClockView *clockView, Clock *clock, NSDictionary *change) {

  // update clock view with new value
  // 用新值更新clock的view
  clockView.date = change[NSKeyValueChangeNewKey];
}];
```

以上的例子很简单的就完成了.一个clock的view创建一个KVO控制器用来监视clock的date属性，block回调被用来初始化和改变通知，由于对`KVOController`强引用，观察者会在控制器delloc的同时进行移除，不需要进行其他移除操作。

注意：指定观察者必须支持弱引用。该归零弱引用用来防止一个释放的观察者实例处理通知。
#### NSObject Category
框架提供更简单的用法，仅仅需要`#import <KVOController/FBKVOController.h>`(cocoaPod导入),就可以向所有对象添加一个`KVOController`属性
```objc
[self.KVOController observe:clock keyPath:@"date" options:NSKeyValueObservingOptionInitial|NSKeyValueObservingOptionNew action:@selector(updateClockWithDateChange:)];
```

## 特性

KVOController受益于现在的Objective-C运行时高级特性，包括ARC和弱引用集和，需求：
- iOS 6 or later.
- OS X 10.7 or later.

## 安装
安装并使用[CocoaPods](https://github.com/cocoapods/cocoapods)，并且添加以下命令到你项目的Podfile文件中：

```ruby
pod 'KVOController'
```

安装并使用[Carthage](https://github.com/carthage/carthage)，添加以下命令到你的项目Cartfile文件中：

```
github "facebook/KVOController"
```
或者，将 FBKVOController.h 和 FBKVOController.m 文件拖拽到你的Xcode项目中，勾选`copy files if needed`选项，对于iOS应用，你可以考虑选择链接KVOController项目的静态库。

已经使用CocoaPods 或者 Carthage方式导入框架的项目，可以在Objective-C代码中添加以下导入语句：
```objective-c
#import <KVOController/FBKVOController.h>
```

## 测试
使用CocoaPods管理单元测试包含的依赖，请先安装CocoaPods如果你还没有的话，然后再命令行中导航到KVOController根目录中，并且输入：
```sh
pod install
```
这将安装并添加测试使用的依赖库`OCHamcrest`和`OCMockito`，重新打开Xcode的KVOController工作空间然后使用`⌘U`进行测试。

## 证书
KVOController基于BSD证书发布，详情请参照LICENSE文件。