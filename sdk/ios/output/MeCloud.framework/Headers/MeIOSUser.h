//
//  MeIOSUser.h
//  MeCloud
//
//  Created by super on 2017/8/11.
//  Copyright © 2017年 Rex. All rights reserved.
//


#import "MeIOSObject.h"

@interface MeIOSUser : MeIOSObject

@property (nonatomic, copy, readonly) NSString *userID;

@property (nonatomic, copy, readonly) NSString *deviceID;

@property (nonatomic, copy, readonly) NSString *username;

- (id)initWithMeUser:(void *)meUser;

- (id)initWithJSONValue:(MeIOSJSONObject *)jsonValue;

+ (void)saveLoginUser:(MeIOSUser *)user;

+ (NSString *)encodePassword:(NSString *)username password:(NSString *)password;

+ (NSString *)device;

+ (NSString *)getMeObjectID;

@end
