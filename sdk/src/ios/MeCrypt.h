//
//  MeCrypt.h
//  MeCloud
//
//  Created by super on 2017/9/11.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface MeCrypt : NSObject

+ (NSData *)encodeWithData:(NSData *)data;

+ (NSData *)decodeWithBytes:(NSData *)data;

@end
