//
//  MeIOSException.h
//  MeCloud
//
//  Created by super on 2017/8/10.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface MeIOSException : NSObject

@property (nonatomic, assign) NSInteger errCode;

@property (nonatomic, copy) NSString *errMessage;

@property (nonatomic, copy) NSString *errInfo;

- (id)initWithCode:(NSInteger)code message:(NSString *)message info:(NSString *)info;

- (id)initWithMeException:(void *)exception;

@end
