//
//  MeIOSQuery.h
//  MeCloud
//
//  Created by super on 2017/8/11.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MeIOSJSONObject.h"

typedef enum {
    IOSCOUNT = 1,
    IOSID,
    IOSLIST,
    IOSJSONOBJECT,
    IOSDISTINCTCOUNT
} MEIOSAGGREGATEMETHOD;

@interface MeIOSAggregate : MeIOSJSONObject

- (id)initWithClassName:(NSString *)name;

- (void)addWhereEqualWithKey:(NSString *)key value:(NSString *)value;

- (void)addWhereEqualWithKey:(NSString *)key intValue:(NSInteger)value;

- (void)addWhereNotEqualWithKey:(NSString *)key value:(NSString *)value;

- (void)addWhereNotEqualWithKey:(NSString *)key intValue:(NSInteger)value;

- (void)addWhereGreaterWithKey:(NSString *)key value:(NSString *)value;

- (void)addWhereGreaterWithKey:(NSString *)key intValue:(NSInteger)value;

- (void)addWhereLessWithKey:(NSString *)key value:(NSString *)value;

- (void)addWhereLessWithKey:(NSString *)key intValue:(NSInteger)value;

- (void)setMethod:(MEIOSAGGREGATEMETHOD)method;

- (void)setResponseKey:(NSString *)key;

- (void)setDistinctKey:(NSString *)key;

@end

@interface MeIOSQuery : MeIOSJSONObject

- (id)initWithClassName:(NSString *)name;

// where equal 条件
- (void)addWhereEqualWithKey:(NSString *)key value:(NSString *)value;

- (void)addWhereEqualWithKey:(NSString *)key intValue:(NSInteger)value;

- (void)addWhereEqualOrWithKey:(NSString *)key value:(NSString *)value;

- (void)addWhereEqualOrWithKey:(NSString *)key intValue:(NSInteger)value;

- (void)addWhereGreaterWithKey:(NSString *)key value:(NSString *)value;

- (void)addWhereGreaterWithKey:(NSString *)key intValue:(NSInteger)value;

- (void)addWhereLessWithKey:(NSString *)key value:(NSString *)value;

- (void)addWhereLessWithKey:(NSString *)key intValue:(NSInteger)value;

// where notEqual 条件
- (void)addWhereNotEqualWithKey:(NSString *)key value:(NSString *)value;

- (void)addWhereNotEqualWithKey:(NSString *)key intValue:(NSInteger)value;

// 选择那些字段
- (void)addSelectKeys:(NSMutableArray *)array;

// 不选择那些字段
- (void)addNotSelectKeys:(NSMutableArray *)array;

// 增加选择某个字段
- (void)addSelectKey:(NSString *)key;

// 增加放弃选择某个字段
- (void)addNotSelectKey:(NSString *)key;

// 升序
- (void)addAscendSortKeys:(NSString *)key;

// 倒序
- (void)addDescendSortKeys:(NSString *)key;

// 分页
- (void)addLimit:(NSInteger)count;

// 偏移量
- (void)addStartId:(NSString *)startId;

- (void)addAggregate:(MeIOSAggregate *)aggregate;

- (void)setAggregate:(MeIOSAggregate *)aggregate;

@end
