//
//  MeIOSJSONObject.h
//  MeCloud
//
//  Created by super on 2017/8/10.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface MeIOSJSONObject : NSObject
{
    // C++ 多态
    void *object;
}

@property (nonatomic, copy, readonly) NSString *jsonString;

@property (nonatomic, assign, readonly) BOOL empty;

- (id)initWithString:(NSString *)string;

- (id)initWithJSONObject:(void *)_object;

// 子类需要重载
- (void)loadObject;

// 禁止用这个函数
- (void)clearObject;

- (void *)getObject;

- (NSString *)stringValue:(NSString *)name;

- (void)setStringValue:(NSString *)value key:(NSString *)key;

- (double)doubleValue:(NSString *)name;

- (void)setDoubleValue:(double)value key:(NSString *)key;

- (NSInteger)integerValue:(NSString *)name;

- (void)setIntegerValue:(NSInteger)value key:(NSString *)key;

- (long long)longValue:(NSString *)name;

- (void)setLongValue:(long long)value key:(NSString *)key;

- (BOOL)boolValue:(NSString *)name;

- (void)setBoolValue:(BOOL)value key:(NSString *)key;

- (MeIOSJSONObject *)jsonObject:(NSString *)key;

- (void)setJSONObject:(MeIOSJSONObject *)iosObject key:(NSString *)key;

- (NSMutableArray *)integerArrayWithKey:(NSString *)key;

- (NSMutableArray *)stringArrayWithKey:(NSString *)key;

- (NSMutableArray *)doubleArrayWithKey:(NSString *)key;

- (NSMutableArray *)jsonArrayWithKey:(NSString *)key;

@end
