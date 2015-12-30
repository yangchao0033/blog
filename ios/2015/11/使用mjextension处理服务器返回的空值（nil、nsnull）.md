#使用MJExtension处理服务器返回的空值（nil、NSNull）

```objc
#import "YCModel.h"
#import "MJExtension.h"
@implementation YCModel
	/**使用时只要将该方法粘贴到你的模型.m文件并导入`#import "MJExtension.h"`即可*/
	/**处理当服务器返回字段对应value为NSNull或nil时，对该字段赋为@""字符串，避免程序产生崩溃*/
- (id)newValueFromOldValue:(id)oldValue property:(MJProperty *)property
{
	// 获取旧值
    NSString *strOld = NSStringFromClass([oldValue class]);
    // 获取NSNull的字符串形式
    NSString *strNull = NSStringFromClass([NSNull class]);
    if ([NSStringFromClass(property.type.typeClass) isEqualToString:NSStringFromClass([NSString class])] && ([strOld isEqualToString:strNull] || !strOld)){
        return @"";
    }
    return oldValue;
}
@end
```