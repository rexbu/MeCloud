//
//  MeIOSQuery.m
//  MeCloud
//
//  Created by super on 2017/8/11.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "MeIOSQuery.h"
#import "MeQuery.h"

@implementation MeIOSAggregate

- (id)initWithClassName:(NSString *)name {
    if (self = [super init]) {
        [self clearObject];
        object = new MeAggregateObject([name UTF8String]);
    }
    
    return self;
}

- (MeAggregateObject *)aggregate {
    return (MeAggregateObject *)object;
}

- (void)addWhereEqualWithKey:(NSString *)key value:(NSString *)value {
    if (!key || key.length <= 0) return;
    self.aggregate->whereEqualTo([key UTF8String], [value UTF8String]);
}

- (void)addWhereEqualWithKey:(NSString *)key intValue:(NSInteger)value {
    if (!key || key.length <= 0) return;
    self.aggregate->whereNotEqualTo([key UTF8String], (int)value);
}

- (void)addWhereNotEqualWithKey:(NSString *)key value:(NSString *)value {
    if (!key || key.length <= 0) return;
    self.aggregate->whereNotEqualTo([key UTF8String], [value UTF8String]);

}

- (void)addWhereNotEqualWithKey:(NSString *)key intValue:(NSInteger)value {
    if (!key || key.length <= 0) return;
    self.aggregate->whereNotEqualTo([key UTF8String], (int)value);
    
}

- (void)addWhereGreaterWithKey:(NSString *)key value:(NSString *)value {
    if (!key || key.length <= 0) return;
    self.aggregate->whereGreater([key UTF8String], [value UTF8String]);
}

- (void)addWhereGreaterWithKey:(NSString *)key intValue:(NSInteger)value {
    if (!key || key.length <= 0) return;
    self.aggregate->whereGreater([key UTF8String], (int)value);
}

- (void)addWhereLessWithKey:(NSString *)key value:(NSString *)value {
    if (!key || key.length <= 0) return;
    self.aggregate->whereLess([key UTF8String], [value UTF8String]);
}

- (void)addWhereLessWithKey:(NSString *)key intValue:(NSInteger)value {
    if (!key || key.length <= 0) return;
    self.aggregate->whereLess([key UTF8String], (int)value);
}

- (void)setMethod:(MEIOSAGGREGATEMETHOD)method {
    self.aggregate->setMethod((MEAGGREGATEMETHOD)method);
}

- (void)setResponseKey:(NSString *)key {
    if (!key || key.length <= 0) return;
    self.aggregate->setResponseKey([key UTF8String]);
}

- (void)setDistinctKey:(NSString *)key {
  if (!key || key.length <= 0) return;
  self.aggregate->setDistinctKey([key UTF8String]);
}

@end

@implementation MeIOSQuery

- (id)initWithClassName:(NSString *)name {
    if (self = [super init]) {
        [self clearObject];
        object = new MeQuery([name UTF8String]);
    }
    
    return self;
}

- (void)loadObject {
    [self clearObject];
    object = new MeQuery("#warning 没有查询表名称");
}

- (MeQuery *)meQuery {
    return (MeQuery *)object;
}

- (void)addWhereEqualWithKey:(NSString *)key value:(NSString *)value {
    if (!key || key.length <= 0) return;
    self.meQuery->whereEqualTo([key UTF8String], [value UTF8String]);
}

- (void)addWhereEqualWithKey:(NSString *)key intValue:(NSInteger)value {
    if (!key || key.length <= 0) return;
    self.meQuery->whereEqualTo([key UTF8String], (int)value);
}

- (void)addWhereGreaterWithKey:(NSString *)key value:(NSString *)value {
  if (!key || key.length <= 0) return;
  self.meQuery->whereGreater([key UTF8String], [value UTF8String]);
}

- (void)addWhereGreaterWithKey:(NSString *)key intValue:(NSInteger)value {
  self.meQuery->whereGreater([key UTF8String], value);
}

- (void)addWhereLessWithKey:(NSString *)key value:(NSString *)value {
  self.meQuery->whereLess([key UTF8String], [value UTF8String]);
}

- (void)addWhereLessWithKey:(NSString *)key intValue:(NSInteger)value {
  self.meQuery->whereLess([key UTF8String], value);
}

- (void)addWhereEqualOrWithKey:(NSString *)key value:(NSString *)value {
    if (!key || key.length <= 0) return;
    self.meQuery->whereEqualOr([key UTF8String], [value UTF8String]);
}

- (void)addWhereEqualOrWithKey:(NSString *)key intValue:(NSInteger)value {
    if (!key || key.length <= 0) return;
    self.meQuery->whereEqualOr([key UTF8String], (int)value);
}

- (void)addWhereNotEqualWithKey:(NSString *)key value:(NSString *)value {
    if (!key || key.length <= 0) return;
    self.meQuery->whereNotEqualTo([key UTF8String], [value UTF8String]);
}

- (void)addWhereNotEqualWithKey:(NSString *)key intValue:(NSInteger)value {
    if (!key || key.length <= 0) return;
    self.meQuery->whereNotEqualTo([key UTF8String], (int)value);
}

- (void)addSelectKeys:(NSMutableArray *)array {
    [array enumerateObjectsUsingBlock:^(id  _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        [self addSelectKey:obj];
    }];
}

- (void)addNotSelectKeys:(NSMutableArray *)array {
    [array enumerateObjectsUsingBlock:^(id  _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        [self addNotSelectKey:obj];
    }];
}

- (void)addSelectKey:(NSString *)key {
    if (!key || key.length <= 0) return;
    self.meQuery->addSelectKey([key UTF8String]);
}

- (void)addNotSelectKey:(NSString *)key {
    if (!key || key.length <= 0) return;
    self.meQuery->addNotSelectKey([key UTF8String]);
}

- (void)addAscendSortKeys:(NSString *)key {
    if (!key || key.length <= 0) return;
    self.meQuery->addAscendSortKeys([key UTF8String]);
}

- (void)addDescendSortKeys:(NSString *)key {
    if (!key || key.length <= 0) return;
    self.meQuery->addDescendSortKeys([key UTF8String]);
}

- (void)addLimit:(NSInteger)count {
    self.meQuery->addLimit(count);
}

- (void)addStartId:(NSString *)startId {
    if (!startId || startId.length <= 0) return;
    self.meQuery->addStartId([startId UTF8String]);
}

- (void)addAggregate:(MeIOSAggregate *)aggregate {
    if (!aggregate) return;
    self.meQuery->addAggregateObject((MeAggregateObject *)(aggregate.getObject));
}

- (void)setAggregate:(MeIOSAggregate *)aggregate {
    if (!aggregate) return;
    self.meQuery->setAggregateObject((MeAggregateObject *)(aggregate.getObject));
}

@end
