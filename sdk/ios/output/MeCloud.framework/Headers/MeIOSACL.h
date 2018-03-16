//
//  MeIOSACL.h
//  MeCloud
//
//  Created by super on 2017/8/10.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MeIOSJSONObject.h"

@class MeIOSUser;
@class MeIOSRole;

@interface MeIOSACL : MeIOSJSONObject

- (id)initWithACL:(void *)acl;

// 设置公开读权限，所有人都可以读
- (void)setPublicReadAccess;

// 设置公开写权限，所有人都可以写
- (void)setPublicWriteAccess;

// 设置该role角色可以读，param为role name
- (void)setRoleReadAccessWithName:(NSString *)role;

// 设置该role角色可以写，param为role name
- (void)setRoleWriteAccessWithName:(NSString *)role;

// 设置该role数据可删除，注意（这数据得要服务器数据表支持删除功能）
- (void)setRoleDeleteAccessWithName:(NSString *)role;

// 设置该role角色可以读，param为MeIOSRole object
- (void)setRoleReadAccessWithRole:(MeIOSRole *)role;

// 设置该role角色可以写，param为MeIOSRole object
- (void)setRoleWriteAccessWithRole:(MeIOSRole *)role;

// 设置该role数据可删除，注意（这数据得要服务器数据表支持删除功能）
- (void)setRoleDeleteAccessWithRole:(MeIOSRole *)role;

// 设置该user可以读，param为userId
- (void)setUserReadAccessWithId:(NSString *)userId;

// 设置该user可以写，param为userId
- (void)setUserWriteAccessWithId:(NSString *)userId;

// 设置该user数据可删除，注意（这数据得要服务器数据表支持删除功能）
- (void)setUserDeleteAccessWithId:(NSString *)userId;

// 设置该user可以写，param为MeIOSUser object
- (void)setUserReadAccessWithUser:(MeIOSUser *)user;

// 设置该user可以写，param为MeIOSUser object
- (void)setUserWriteAccessWithUser:(MeIOSUser *)user;

// 设置该user数据可删除，注意（这数据得要服务器数据表支持删除功能）
- (void)setUserDeleteAccessWithUser:(MeIOSUser *)user;

@end
