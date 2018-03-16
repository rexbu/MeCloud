//
//  MeIOSException.m
//  MeCloud
//
//  Created by super on 2017/8/10.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "MeIOSException.h"
#import "MeCloud.h"

@implementation MeIOSException

- (id)initWithCode:(NSInteger)code message:(NSString *)message info:(NSString *)info {
    if (self = [super init]) {
        _errCode = code;
        _errMessage = message;
        _errInfo = info;
    }
    
    return self;
}

- (id)initWithMeException:(void *)exception {
    MeException *meException = (MeException *)exception;
    NSInteger errorCode = meException->errCode();
    NSString *errorMessage = [NSString stringWithUTF8String:meException->errMsg()];
    NSString *info = nil;
    if (meException->has("info")) {
        info = [NSString stringWithUTF8String:meException->stringValue("info")];
    }
    return [[MeIOSException alloc] initWithCode:errorCode message:errorMessage info:info];
}

@end
