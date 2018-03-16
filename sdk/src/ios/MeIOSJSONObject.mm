//
//  MeIOSJSONObject.m
//  MeCloud
//
//  Created by super on 2017/8/10.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "MeIOSJSONObject.h"
#include "JSON.h"

using namespace mc;

@implementation MeIOSJSONObject

- (id)init {
    if (self = [super init]) {
        [self loadObject];
    }
    
    return self;
}

- (id)initWithString:(NSString *)string {
    if (self = [super init]) {
        object = new JSONObject([string UTF8String]);
    }
    
    return self;
}

- (id)initWithJSONObject:(void *)_object {
    if (self = [super init]) {
        [self clearObject];
        object = new JSONObject((JSONObject *)_object, false);
    }
    
    return self;
}

- (void)loadObject {
    [self clearObject];
    object = new JSONObject();
}

- (NSString *)jsonString {
    return [NSString stringWithUTF8String:self.jsonObject->toString()];
}

- (BOOL)empty {
    return self.jsonObject->empty();
}

- (void)clearObject {
    if (object) {
        delete (JSONObject *)object;
        object = NULL;
    }
}

- (void *)getObject {
    return object;
}

- (JSONObject *)jsonObject {
    return (JSONObject *)object;
}

- (NSString *)stringValue {
    return [NSString stringWithUTF8String:self.jsonObject->toString()];
}

- (NSString *)stringValue:(NSString *)name {
    if (!name || name.length <= 0) return nil;
    
    if (self.jsonObject->stringValue([name UTF8String])) {
        return [NSString stringWithUTF8String:self.jsonObject->stringValue([name UTF8String])];
    }
    
    return nil;
}

- (void)setStringValue:(NSString *)value key:(NSString *)key {
    if (!key || key.length <= 0) return;
    NSString *newValue = @"";
    if (value) {
        newValue = value;
    }
    self.jsonObject->put([key UTF8String], [newValue UTF8String]);
}

- (double)doubleValue:(NSString *)name {
    if (!name || name.length <= 0) return 0;
    return self.jsonObject->doubleValue([name UTF8String]);
}

- (void)setDoubleValue:(double)value key:(NSString *)key {
    if (!key || key.length <= 0) return;
    self.jsonObject->put([key UTF8String], value);
}

- (NSInteger)integerValue:(NSString *)name {
    if (!name || name.length <= 0) return 0;
    return self.jsonObject->longValue([name UTF8String]);
}

- (void)setIntegerValue:(long)value key:(NSString *)key {
    if (!key || key.length <= 0) return;
    self.jsonObject->put([key UTF8String], value);
}

- (long long)longValue:(NSString *)name {
  if (!name || name.length <= 0) return 0;
  return self.jsonObject->longValue([name UTF8String]);
}

- (void)setLongValue:(long long)value key:(NSString *)key {
  if (!key || key.length <= 0) return;
  self.jsonObject->put([key UTF8String], (long)value);
}

- (BOOL)boolValue:(NSString *)name {
    if (!name || name.length <= 0) return false;
    return self.jsonObject->boolValue([name UTF8String]);
}

- (void)setBoolValue:(BOOL)value key:(NSString *)key {
    self.jsonObject->putBool([key UTF8String], value);
}

- (MeIOSJSONObject *)jsonObject:(NSString *)key {
    if (!key || key.length <= 0) return nil;
    JSONObject json = self.jsonObject->jsonValue([key UTF8String]);
    if (!json.empty()) {
        return [[MeIOSJSONObject alloc] initWithJSONObject:&json];
    }
    
    return nil;
}

- (void)setJSONObject:(MeIOSJSONObject *)iosObject key:(NSString *)key {
  if (!iosObject || !key || key.length <= 0) return;
  self.jsonObject->put([key UTF8String], (JSONObject *)(iosObject.getObject));
}

- (NSMutableArray *)integerArrayWithKey:(NSString *)key {
    NSMutableArray *array = [NSMutableArray array];
    JSONArray jsonArray = self.jsonObject->arrayValue([key UTF8String]);
    for (int i = 0; i < jsonArray.size(); i++) {
        int *value = jsonArray.intValue(i);
        NSNumber *number = [NSNumber numberWithDouble:*value];
        [array addObject:number];
    }
    
    return array;
}

- (NSMutableArray *)doubleArrayWithKey:(NSString *)key {
    NSMutableArray *array = [NSMutableArray array];
    JSONArray jsonArray = self.jsonObject->arrayValue([key UTF8String]);
    for (int i = 0; i < jsonArray.size(); i++) {
        double *value = jsonArray.doubleValue(i);
        NSNumber *number = [NSNumber numberWithDouble:*value];
        [array addObject:number];
    }
    
    return array;
}

- (NSMutableArray *)stringArrayWithKey:(NSString *)key {
    NSMutableArray *array = [NSMutableArray array];
    JSONArray jsonArray = self.jsonObject->arrayValue([key UTF8String]);
    for (int i = 0; i < jsonArray.size(); i++) {
        char *value = jsonArray.stringValue(i);
        NSString *string = [NSString stringWithUTF8String:value];
        [array addObject:string];
    }
    
    return array;
}

- (NSMutableArray *)jsonArrayWithKey:(NSString *)key {
    NSMutableArray *array = [NSMutableArray array];
    JSONArray jsonArray = self.jsonObject->arrayValue([key UTF8String]);
    for (int i = 0; i < jsonArray.size(); i++) {
        JSONObject json = jsonArray.jsonValue(i);
        MeIOSJSONObject *object = [[MeIOSJSONObject alloc] initWithJSONObject:&json];
        [array addObject:object];
    }
    
    return array;
}

- (void)dealloc {
    delete (JSONObject *)object;
}

@end
