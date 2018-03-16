//
//  MeIOSUser.m
//  MeCloud
//
//  Created by super on 2017/8/11.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "MeIOSUser.h"
#import "MeUser.h"

@implementation MeIOSUser

+ (void)saveLoginUser:(MeIOSUser *)user {
    MeUser::saveLoginUser((MeUser *)(user.getObject));
}

+ (NSString *)encodePassword:(NSString *)username password:(NSString *)password {
    const char *encode = MeUser::encodePassword([username UTF8String], [password UTF8String]);
    NSString *iOSPassword = [NSString stringWithUTF8String:encode];
    delete encode;
    return iOSPassword;
}

+ (NSString *)device {
    return [NSString stringWithUTF8String:MeUser::device()];
}

+ (NSString *)getMeObjectID {
    MeIOSObject *object = [[MeIOSObject alloc] init];
    return object.objectID;
}

- (id)initWithMeUser:(void *)meUser {
    if (self = [super init]) {
        ((MeUser *)object)->copy((MeUser *)meUser, false);
    }
    
    return self;
}

- (id)initWithJSONValue:(MeIOSJSONObject *)jsonValue {
    if (self = [super init]) {
      [self clearObject];
      object = new MeUser((JSONObject *)(jsonValue.getObject));
    }
  
    return self;
}

- (void)loadObject {
    [self clearObject];
    object = new MeUser();
}

- (MeUser *)meUser {
    return (MeUser *)object;
}

- (NSString *)username {
    const char *name = self.meUser->stringValue("username");
    if (name) {
        return [NSString stringWithUTF8String:name];
    }
    
    return nil;
}

- (NSString *)userID {
    const char *_id = self.meUser->stringValue("_id");
    if (_id) {
        return [NSString stringWithUTF8String:_id];
    }
    
    return nil;
}

- (NSString *)deviceID {
  const char *device = self.meUser->stringValue("device");
  if (device) {
    return [NSString stringWithUTF8String:device];
  }
  
  return nil;
}

- (NSString *)description {
    return [NSString stringWithUTF8String:self.meUser->toString()];
}

@end
