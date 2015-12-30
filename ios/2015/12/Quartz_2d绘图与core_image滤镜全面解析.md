#Quartz 2D绘图与Core Image滤镜全面解析
##概述
在iOS中可以很容易的开发出绚丽的界面效果，一方面得益于成功系统的设计，另一方面得益于它强大的开发框架。今天我们将围绕iOS中两大图形、图像绘图框架进行介绍：Quartz 2D绘制2D图形和Core Image中强大的滤镜功能。
##1.	Quartz 2D
在iOS中常用的绘图框架就是Quartz 2D，Quartz 2D是Core Graphics框架的一部分，是一个强大的二维图像绘制引擎。Quartz 2D在UIKit中也有很好的封装和集成，我们日常开发时所用到的UIKit中的组件都是由Core Graphics进行绘制的。不仅如此，当我们引入UIKit框架时系统会自动引入Core Graphics框架，并且为了方便开发者使用在UIKit内部还对一些常用的绘图API进行了封装。
在iOS中绘图一般分为以下几个步骤：
1.获取绘图上下文
2.创建并设置路径
3.将路径添加到上下文
4.设置上下文状态
5.绘制路径
6.释放路径
图形上下文CGContextRef