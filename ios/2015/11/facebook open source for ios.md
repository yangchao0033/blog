#Facebook Open Source For iOS
2015年11月27日 上午10:16
##[Framework](https://code.facebook.com/projects/ios/framework)
###AsyncDisplayKit
> `AsyncDisplayKit` 是一个用来保持即使在大量复杂用户界面的情况下依然保持应用可以平滑运行并且快速响应的iOS框架。他最初的设计目的是服务于Facebook的Paper的.并且接手于pop的基于物理的动画效果。而且和UIKit的动力和传统的应用设计一样强大。

###shimmer

###ComponentKit

> `ComponentKit` 是一个Objective-C++ 的iOS界面框架并很大程度上是受React得启发的。
`ComponentKit` 使用了函数式的声明方式构建UI，它让你更专注于你的UI应该长什么样子，而不必要关心构建他的步骤。
他强调一种从不可变模型到不可变组件的单项数据流来描述界面（views）如何被配置。
他确实利用这个描述构建了视图的层次结构。
该框架的构建是为了服务于Facebook的新闻Feed模块，而现在被贯穿于Facebook应用所有模块中。

###pop
`优雅的高性能动画实现框架`
##[compilation](https://code.facebook.com/projects/ios/compilation)
###buck
###xctool
[buck&xctool相关介绍](http://www.infoq.com/cn/news/2013/05/Facebook-buck-xctool-build)
##[language Tools](https://code.facebook.com/projects/ios/language)
###infer
##[development Tools](https://code.facebook.com/projects/ios/development)
###tweaks
###infer
###chisel
##[design Tools](https://code.facebook.com/projects/ios/design)
###origami
##[source Control](https://code.facebook.com/projects/ios/sourcecontrol)
###PathPicker
>用于提供更方便的git命令行实用功能，比如结果筛选，搜索等更好用的方法
当解析完输入后，PathPicker会提供给用户一个非常nice的界面用于选择你感兴趣的文件。然后你可以用你最喜欢的编辑器打开他们，或者执行任意的命令。