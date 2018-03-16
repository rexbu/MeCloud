//
//  MeIOSObject.h
//  MeCloud
//
//  Created by super on 2017/8/10.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MeIOSJSONObject.h"
#import "MeIOSACL.h"

@interface MeIOSObject : MeIOSJSONObject

- (id)initWithClassName:(NSString *)className;

- (id)initWithMeObject:(void *)_object;

- (id)initWithObjectID:(NSString *)objectID className:(NSString *)className;

- (NSString *)className;

- (NSString *)objectID;

- (void)setClassName:(NSString *)className;

- (NSMutableArray *)meObjectArrayWithKey:(NSString *)key;

- (MeIOSObject *)meObjectWithKey:(NSString *)key;

// 设置权限
- (void)setACL:(MeIOSACL *)acl;

// 获取权限
- (MeIOSACL *)getACL;

- (void)setObject:(MeIOSObject *)iosObject key:(NSString *)key;

- (void)setObjects:(NSArray *)iosObject key:(NSString *)key;

- (void)addObject:(MeIOSObject *)iosObject key:(NSString *)key;

@end
