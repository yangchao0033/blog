#关于@autoreleasepool的测试及使用
* 不用autoreleasepool
```objc
- (void)doSomething {
    NSMutableArray *collection = @[].mutableCopy;
    for (int i = 0; i < 10e6; ++i) {
        NSString *str = [NSString stringWithFormat:@"hi + %d", i];
        [collection addObject:str];
    }
    NSLog(@"finished!");
}
```  
![不使用](https://github.com/yangchao0033/blog/blob/gh-pages/ios/2015/11/image/Snip20151108_8.png)  

* 使用autoreleasepool
```objc
- (void)doSomething {
    NSMutableArray *collection = @[].mutableCopy;
    for (int i = 0; i < 10e6; ++i) {
        @autoreleasepool {
       	NSString *str = [NSString stringWithFormat:@"hi + %d", i];
        [collection addObject:str];
        }
    }
    NSLog(@"finished!");
}
```  
![使用autoreleasepool](https://github.com/yangchao0033/blog/blob/gh-pages/ios/2015/11/image/Snip20151108_9.png)  

综上，当需要在程序中创建大量的临时变量时（大量也可指数量多，不确定，比如从数据库中读取数据时）,很容易使内存产生峰值又回到内存低谷，这样对程序的性能会产生很大影响，而使用自动释放池后，峰值明显有所下降。  
`原因：`由于在mian函数中已经为我们添加了自动释放池，如下所示：  
```objc
#import <UIKit/UIKit.h>
#import "AppDelegate.h"

int main(int argc, char * argv[]) {
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}
```
而当autoreleasepool的右括号结束后，会对整个autoreleasepool做一次倾倒。但是这会使其中局部产生的大量零时变量无法及时释放，直到for循环结束后抵达main中autoreleasepool的右括号位置才能释放。这样会在局部位置发生内存峰值。
官方提出的解决方案是，在大量产生局部变量的位置用autoreleasepool代码块进行包装。比如for循环中要执行的语句，这样每次for循环结束后就会及时收回临时变量占用的内存空间。  
# 思考：如何在指定的次数时，及时释放自动释放池中的临时变量

```
使用mrc下的特性
在编译选项中，为MRC的程序添加-fno-objc-arc标记，表明在编译时，该文件使用MRC编译
如果要在MRC项目中添加ARC的文件，可以使用 -fobjc-arc 标记即可
```
这里的需求我们可以使用MRC时代的一个重量级的自动释放池，通常用来创建偶尔需要释放清空的池。
```objc
- (void)doSomething {
    int count = 0;
    NSMutableArray *collection = @[].mutableCopy;
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    for (int i = 0; i < 10e6; ++i) {
            NSString *str = [NSString stringWithFormat:@"hi + %d", i];
        [collection addObject:str];
        if (++count == 100) {
            /** 每一百次倾倒一次池子 */
            [pool drain];
            count = 0;
        }
    }
    /** 用来倾倒当i的个数不是100的倍数时，比如读取数据库数据时,数据总数为不确定值 */
    [pool drain];
    NSLog(@"finished!");
}
```