//
//  MeIOSJoinQuery.h
//  MeCloud
//
//  Created by super on 2017/9/18.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "MeIOSJSONObject.h"
#import <Foundation/Foundation.h>

@interface MeIOSJoinQuery : MeIOSJSONObject

- (id)initWithClassName:(NSString *)name;

// 嵌套query
- (id)initWithNestClassName:(NSString *)ClassName;

- (void)addSelectKey:(NSString *)key;

- (void)addNotSelectKey:(NSString *)key;

- (void)addLimit:(NSUInteger)count;

// 确保加入的先后顺序，级别越高，理解关系型中的数据库select排序顺序
- (void)addAscendSortKeys:(NSString *)key;

- (void)addDescendSortKeys:(NSString *)key;

- (void)matchEqualToWithKey:(NSString *)key value:(NSString *)value;

- (void)matchEqualToWithKey:(NSString *)key intValue:(NSInteger)value;

- (void)matchGreatThanWithKey:(NSString *)key value:(NSString *)value;

- (void)matchGreatThanWithKey:(NSString *)key intValue:(NSInteger)value;

- (void)matchLessThanWithKey:(NSString *)key value:(NSString *)value;

- (void)matchLessThanWithKey:(NSString *)key intValue:(NSInteger)value;

// 重点（关联表）
- (void)addForeignTable:(NSString *)fromTable foreignKey:(NSString *)foreignKey localKey:(NSString *)localKey document:(NSString *)document;

// 嵌套查询,最好不要用，如果用到多表嵌套的话需要服务端优化表的结构或者走特定接口的方式，因为嵌套表越多性能越受影响
- (void)addJoinQuery:(MeIOSJoinQuery *)joinQuery;

@end
