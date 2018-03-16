//
//  MeIOSACL.m
//  MeCloud
//
//  Created by super on 2017/8/10.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "MeIOSACL.h"
#import "MeIOSUser.h"
#import "MeIOSRole.h"
#import "MeACL.h"

@implementation MeIOSACL

- (id)initWithACL:(void *)_acl {
    if (self = [super init]) {
        // 这一步用了赋值运算符重载，执行深拷贝
        object = _acl;
    }
    
    return self;
}

- (void)loadObject {
    [self clearObject];
    object = new MeACL();
}

- (MeACL *)meACL {
    return (MeACL *)object;
}

- (void)setPublicReadAccess {
    self.meACL->setPublicReadAccess();
}

- (void)setPublicWriteAccess {
    self.meACL->setPublicWriteAccess();
}

- (void)setRoleReadAccessWithName:(NSString *)role {
    if (!role || role.length <= 0) return;
    self.meACL->setRoleReadAccess([role UTF8String]);
}

- (void)setRoleWriteAccessWithName:(NSString *)role {
    if (!role || role.length <= 0) return;
    self.meACL->setRoleWriteAccess([role UTF8String]);
}

- (void)setRoleDeleteAccessWithName:(NSString *)role {
    if (!role || role.length <= 0) return;
    self.meACL->setRoleDeleteAccess([role UTF8String]);
}

- (void)setRoleReadAccessWithRole:(MeIOSRole *)role {
    if (!role) return;
    self.meACL->setRoleReadAccess((MeRole *)(role.getObject));
}

- (void)setRoleWriteAccessWithRole:(MeIOSRole *)role {
    if (!role) return;
    self.meACL->setRoleWriteAccess((MeRole *)(role.getObject));
}

- (void)setRoleDeleteAccessWithRole:(MeIOSRole *)role {
    if (!role) return;
    self.meACL->setRoleDeleteAccess((MeRole *)(role.getObject));
}

- (void)setUserReadAccessWithId:(NSString *)userId {
    if (!userId || userId.length <= 0) return;
    self.meACL->setUserReadAccess([userId UTF8String]);
}

- (void)setUserWriteAccessWithId:(NSString *)userId {
    if (!userId || userId.length <= 0) return;
    self.meACL->setUserWriteAccess([userId UTF8String]);
}

- (void)setUserDeleteAccessWithId:(NSString *)userId {
    if (!userId || userId.length <= 0) return;
    self.meACL->setUserDeleteAccess([userId UTF8String]);
}

- (void)setUserReadAccessWithUser:(MeIOSUser *)user {
    if (!user) return;
    self.meACL->setUserReadAccess((MeUser *)(user.getObject));
}

- (void)setUserWriteAccessWithUser:(MeIOSUser *)user {
    if (!user) return;
    self.meACL->setUserWriteAccess((MeUser *)(user.getObject));
}

- (void)setUserDeleteAccessWithUser:(MeIOSUser *)user {
    if (!user) return;
    self.meACL->setUserDeleteAccess((MeUser *)(user.getObject));
}

@end
