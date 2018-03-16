//
//  MeIOSRole.h
//  MeCloud
//
//  Created by super on 2017/8/11.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MeIOSObject.h"
#import "MeIOSUser.h"

@interface MeIOSRole : MeIOSObject

- (id)initWithRoleName:(NSString *)roleName;

// 为角色增加一个用户，参数为MeIOSUser object
- (void)setUser:(MeIOSUser *)user;

// 为角色增加一个用户，参数为userId
- (void)setUserWIthId:(NSString *)userId;

@end
