/**
  Copyright (c) 2014-present, Facebook, Inc.
  All rights reserved.

  This source code is licensed under the BSD-style license found in the
  LICENSE file in the root directory of this source tree. An additional grant
  of patent rights can be found in the PATENTS file in the same directory.
 */

#import <Foundation/Foundation.h>

/**
 @abstract Block called on key-value change notification（键值改变通知回调block）.
 @param observer The observer of the change（观察者）.
 @param object The object changed（被观察的对象）.
 @param change The change dictionary.（发生改变的回调字典）
 */
typedef void (^FBKVONotificationBlock)(id observer, id object, NSDictionary *change);


/**
 @abstract FBKVOController makes Key-Value Observing simpler and safer.
 @discussion FBKVOController adds support for handling key-value changes with blocks and custom actions, as well as the NSKeyValueObserving callback. Notification will never message a deallocated observer. Observer removal never throws exceptions, and observers are removed implicitly on controller deallocation. FBKVOController is also thread safe. When used in a concurrent environment, it protects observers from possible resurrection and avoids ensuing crash. By default, the controller maintains a strong reference to objects observed.
 */
@interface FBKVOController : NSObject

/**
 @abstract Creates and returns an initialized KVO controller instance（创建并初始化KVO控制器实例）.
 @param observer The object notified on key-value change（对象键值改变时需要通知的观察者）.
 @return The initialized KVO controller instance（返回实例对象）.
 */
+ (instancetype)controllerWithObserver:(id)observer;

/**
 @abstract The designated initializer（designated实例化对象）.
 @param observer The object notified on key-value change. The specified observer must support weak references（用来接受对象键值改变的观察者，指定的观察者必须支持若引用）.
 @param retainObserved Flag indicating whether observed objects should be retained（retainObserved标志用来指示是否观察者对象会被保留）.
 @return The initialized KVO controller instance.
 @discussion Use retainObserved = NO when a strong reference between controller and observee would create a retain loop. When not retaining observees, special care must be taken to remove observation info prior to observee dealloc（通知对象和观察者之间存在强引用并且造成循环引用时将retainObserved 设置为 NO， 如果不保留observees，必须特别小心地在dealloc之前删除对observee的观察信息）.
 */
- (instancetype)initWithObserver:(id)observer retainObserved:(BOOL)retainObserved;

/**
 @abstract Convenience initializer（便利构造器）.
 @param observer The object notified on key-value change. The specified observer must support weak references（对象在键值改变时通知的对象，指定观察者必须支持弱引用）.
 @return The initialized KVO controller instance.
 @discussion By default, KVO controller retains objects observed（默认情况下，KVO控制器会保留被观察的对象）.
 */
- (instancetype)initWithObserver:(id)observer;

/// The observer notified on key-value change. Specified on initialization.
@property (atomic, weak, readonly) id observer;

/**
 @abstract Registers observer for key-value change notification.（为键值改变通知注册观察者）
 @param object The object to observe（观察者对象）.
 @param keyPath The key path to observe（需要观察的键值key path）.
 @param options The NSKeyValueObservingOptions to use for observation（键值观察选项NSKeyValueObservingOptions）.
 @param block The block to execute on notification（执行通知回调的结果）.
 @discussion On key-value change, the specified block is called. In order to avoid retain loops, the block must avoid referencing the KVO controller or an owner thereof. Observing an already observed object key path or nil results in no operation.
 （注意，当键值改变时，指定的block会回调，为了避免循环引用，block必须避免引用KVO控制器或者他的拥有者，观察一个已经被观察的对象键值路径或者nil将不会有引起任何操作）
 */
- (void)observe:(id)object keyPath:(NSString *)keyPath options:(NSKeyValueObservingOptions)options block:(FBKVONotificationBlock)block;
/**
 @abstract Registers observer for key-value change notification.
 @param object The object to observe.
 @param keyPath The key path to observe.
 @param options The NSKeyValueObservingOptions to use for observation.
 @param action The observer selector called on key-value change.
 @discussion On key-value change, the observer's action selector is called. The selector provided should take the form of -propertyDidChange, -propertyDidChange: or -propertyDidChange:object:, where optional parameters delivered will be KVO change dictionary and object observed. Observing nil or observing an already observed object's key path results in no operation.
 （键值发生变化时，观察者的动作选择子将会被调用，被提供的选择子必须采用-propertyDidChange, -propertyDidChange: 或者 -propertyDidChange:object:格式命名，字典和被观察对象将会作为可选参数在KVO改变时被投递，观察nil或者一个已经被观察的对象路径将不会有任何操作）
 */
- (void)observe:(id)object keyPath:(NSString *)keyPath options:(NSKeyValueObservingOptions)options action:(SEL)action;

/**
 @abstract Registers observer for key-value change notification.
 @param object The object to observe.
 @param keyPath The key path to observe.
 @param options The NSKeyValueObservingOptions to use for observation.
 @param context The context specified.（指定的上下文）
 @discussion On key-value change, the observer's -observeValueForKeyPath:ofObject:change:context: method is called. Observing an already observed object key path or nil results in no operation.
 (键值变化，观察者的该方法将会被调用)
 */
- (void)observe:(id)object keyPath:(NSString *)keyPath options:(NSKeyValueObservingOptions)options context:(void *)context;


/**
 @abstract Registers observer for key-value change notification.
 @param object The object to observe.
 @param keyPaths The key paths to observe.
 @param options The NSKeyValueObservingOptions to use for observation.
 @param block The block to execute on notification.
 @discussion On key-value change, the specified block is called. Inorder to avoid retain loops, the block must avoid referencing the KVO controller or an owner thereof. Observing an already observed object key path or nil results in no operation.
 */
- (void)observe:(id)object keyPaths:(NSArray *)keyPaths options:(NSKeyValueObservingOptions)options block:(FBKVONotificationBlock)block;

/**
 @abstract Registers observer for key-value change notification.
 @param object The object to observe.
 @param keyPaths The key paths to observe.
 @param options The NSKeyValueObservingOptions to use for observation.
 @param action The observer selector called on key-value change.
 @discussion On key-value change, the observer's action selector is called. The selector provided should take the form of -propertyDidChange, -propertyDidChange: or -propertyDidChange:object:, where optional parameters delivered will be KVO change dictionary and object observed. Observing nil or observing an already observed object's key path results in no operation.
 */
- (void)observe:(id)object keyPaths:(NSArray *)keyPaths options:(NSKeyValueObservingOptions)options action:(SEL)action;

/**
 @abstract Registers observer for key-value change notification.
 @param object The object to observe.
 @param keyPaths The key paths to observe.
 @param options The NSKeyValueObservingOptions to use for observation.
 @param context The context specified.
 @discussion On key-value change, the observer's -observeValueForKeyPath:ofObject:change:context: method is called. Observing an already observed object key path or nil results in no operation.
 */
- (void)observe:(id)object keyPaths:(NSArray *)keyPaths options:(NSKeyValueObservingOptions)options context:(void *)context;


/**
 @abstract Unobserve object key path（移除指定的键值观察）
 @param object The object to unobserve.
 @param keyPath The key path to observe.
 @discussion If not observing object key path, or unobserving nil, this method results in no operation.
 如果没有观察对象的键值或者移除nil的键值观察，该方法不会有任何操作
 */
- (void)unobserve:(id)object keyPath:(NSString *)keyPath;

/**
 @abstract Unobserve all object key paths（移除所有该观察者的键值观察事件）.
 @param object The object to unobserve.
 @discussion If not observing object, or unobserving nil, this method results in no operation.
 */
- (void)unobserve:(id)object;

/**
 @abstract Unobserve all objects.
 @discussion If not observing any objects, this method results in no operation.
 */
- (void)unobserveAll;

@end
