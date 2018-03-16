//
//  MeIOSJoinQuery.m
//  MeCloud
//
//  Created by super on 2017/9/18.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "MeIOSJoinQuery.h"
#import "MeJoinQuery.h"

@implementation MeIOSJoinQuery

- (id)initWithClassName:(NSString *)name {
    if (self = [super init]) {
        [self clearObject];
        object = new MeJoinQuery([name UTF8String]);
    }
    
    return self;
}

- (id)initWithNestClassName:(NSString *)ClassName {
    if (self = [super init]) {
        [self clearObject];
        object = new MeJoinQuery([ClassName UTF8String], true);
    }
    
    return self;
}

- (MeJoinQuery *)meJoinQuery {
    return (MeJoinQuery *)object;
}

- (void)addSelectKey:(NSString *)key {
    if (!key || key.length <= 0) return;
    self.meJoinQuery->addSelectKey([key UTF8String]);
}

- (void)addNotSelectKey:(NSString *)key {
    if (!key || key.length <= 0) return;
    self.meJoinQuery->addNotSelectKey([key UTF8String]);
}

- (void)addLimit:(NSUInteger)count {
    self.meJoinQuery->addLimit((int)count);
}

- (void)addAscendSortKeys:(NSString *)key {
    if (!key || key.length <= 0) return;
    self.meJoinQuery->addAscend([key UTF8String]);
}

- (void)addDescendSortKeys:(NSString *)key {
    if (!key || key.length <= 0) return;
    self.meJoinQuery->addDescend([key UTF8String]);
}

- (void)matchEqualToWithKey:(NSString *)key value:(NSString *)value {
    if (!key || key.length <= 0) return;
    self.meJoinQuery->matchEqualTo([key UTF8String], [value UTF8String]);
}

- (void)matchEqualToWithKey:(NSString *)key intValue:(NSInteger)value {
  if (!key || key.length <= 0) return;
  self.meJoinQuery->matchEqualTo([key UTF8String], (int)value);
}

- (void)matchGreatThanWithKey:(NSString *)key value:(NSString *)value {
    if (!key || key.length <= 0) return;
    self.meJoinQuery->matchGreater([key UTF8String], [value UTF8String]);
}

- (void)matchGreatThanWithKey:(NSString *)key intValue:(NSInteger)value {
  if (!key || key.length <= 0) return;
  self.meJoinQuery->matchGreater([key UTF8String], (int)value);
}

- (void)matchLessThanWithKey:(NSString *)key value:(NSString *)value {
    if (!key || key.length <= 0) return;
    self.meJoinQuery->matchLess([key UTF8String], [value UTF8String]);
}

- (void)matchLessThanWithKey:(NSString *)key intValue:(NSInteger)value {
  if (!key || key.length <= 0) return;
  self.meJoinQuery->matchLess([key UTF8String], (int)value);
}

- (void)addForeignTable:(NSString *)fromTable foreignKey:(NSString *)foreignKey localKey:(NSString *)localKey document:(NSString *)document {
    if (!fromTable || fromTable.length <= 0 || !foreignKey || foreignKey.length <= 0 || !localKey || localKey.length <= 0 || !document || document.length <= 0) return;
    self.meJoinQuery->addForeignTable([fromTable UTF8String], [foreignKey UTF8String], [localKey UTF8String], [document UTF8String]);
}

- (void)addJoinQuery:(MeIOSJoinQuery *)joinQuery {
    if (!joinQuery) return;
    self.meJoinQuery->addMeJoinQuery((MeJoinQuery *)(joinQuery.getObject));
}

@end
