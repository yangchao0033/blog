/**
  Copyright (c) 2014-present, Facebook, Inc.
  All rights reserved.

  This source code is licensed under the BSD-style license found in the
  LICENSE file in the root directory of this source tree. An additional grant
  of patent rights can be found in the PATENTS file in the same directory.
 */

#import <Foundation/Foundation.h>
#import "FBKVOController.h"

@interface NSObject (FBKVOController)

/**
 @abstract Lazy-loaded FBKVOController for use with any object(为任何对象提供懒加载的FBKVOController对象)
 @return FBKVOController associated with this object, creating one if necessary（返回一并关联一个FBKVOController对象，如果有必要创建一个）
 @discussion This makes it convenient to simply create and forget a FBKVOController, and when this object gets dealloc'd, so will the associated controller and the observation info（这个会很方便的创建一个FBKVOController对象，当该对象销毁时，他的相关控制器和所观察的信息也会随之销毁）.
 */
@property (nonatomic, strong) FBKVOController *KVOController;
@property (nonatomic, strong) FBKVOController *KVOControllerNonRetaining;

@end
