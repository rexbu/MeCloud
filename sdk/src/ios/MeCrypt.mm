//
//  MeCrypt.m
//  MeCloud
//
//  Created by super on 2017/9/11.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "MeCrypt.h"
#import "MeCloud.h"

@implementation MeCrypt

+ (NSData *)encodeWithData:(NSData *)data {
    Byte *bytes = (Byte *)malloc([data length]);
    [data getBytes:bytes length:[data length]];
    
    Byte *cryptoBytes = MeCloud::shareInstance()->crypto(bytes, (int)[data length]);
    
    NSData *cryptoData = [[NSData alloc] initWithBytes:cryptoBytes length:data.length];
    free(cryptoBytes);
    free(bytes);
    return cryptoData;
}

+ (NSData *)decodeWithBytes:(NSData *)data {
    Byte *bytes = (Byte *)malloc([data length]);
    [data getBytes:bytes length:[data length]];
    
    Byte *decodeBytes = MeCloud::shareInstance()->decrypt(bytes, (int)data.length);
    NSData *decodeByteData = [[NSData alloc] initWithBytes:decodeBytes length:data.length];
    
    free(bytes);
    free(decodeBytes);
    return decodeByteData;
}

@end
