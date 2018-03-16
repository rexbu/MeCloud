//
//  MeIOSRole.m
//  MeCloud
//
//  Created by super on 2017/8/11.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "MeIOSRole.h"
#import "MeRole.h"

@implementation MeIOSRole

- (id)initWithRoleName:(NSString *)roleName {
    if (self = [super init]) {
        [self clearObject];
        object = new MeRole([roleName UTF8String]);
    }
    
    return self;
}

- (MeRole *)meRole {
    return (MeRole *)object;
}

- (void) loadObject {
    [self clearObject];
    object = new MeRole("#warning 没有角色名称");
}

- (void)setUser:(MeIOSUser *)user {
    self.meRole->setUser((MeUser *)(user.getObject));
}

- (void)setUserWIthId:(NSString *)userId {
    self.meRole->setUser([userId UTF8String]);
}

@end
