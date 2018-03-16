//
//  MeIOSObject.m
//  MeCloud
//
//  Created by super on 2017/8/10.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "MeIOSObject.h"
#import "MeIOSJSONObject.h"
#import "MeObject.h"

@implementation MeIOSObject

- (id)initWithClassName:(NSString *)className {
    if (self = [super init]) {
        [self clearObject];
        object = new MeObject([className UTF8String]);
    }
    
    return self;
}

- (id)initWithMeObject:(void *)_object {
    if (self = [super init]) {
        [self clearObject];
        object = new MeObject((MeObject *)_object, false);
    }
    
    return self;
}

- (id)initWithObjectID:(NSString *)objectID className:(NSString *)className {
    if (self = [super init]) {
        [self clearObject];
        object = new MeObject([objectID UTF8String], [className UTF8String]);
    }
    
    return self;
}

- (void)loadObject {
    [self clearObject];
    object = new MeObject();
}

- (void *)getMeObject {
    return object;
}

- (MeObject *)meObject {
    return (MeObject *)object;
}

- (void)setMeObject:(MeObject *)_object {
    if (object) {
        delete (MeObject *)object;
        object = NULL;
    }
    // 深拷贝一份
    object = new MeObject(_object, false);
}

- (NSString *)className {
    return [NSString stringWithUTF8String:self.meObject->className()];
}

- (void)setClassName:(NSString *)className {
    if (!className || className.length <= 0) {
      return;
    }
  
    self.meObject->setClassName([className UTF8String]);
}

- (NSString *)objectID {
    return [NSString stringWithUTF8String:self.meObject->objectId()];
}

- (void)setACL:(MeIOSACL *)acl {
    self.meObject->setACL((MeACL *)(acl.getObject));
}

- (MeIOSACL *)getACL {
    MeACL acl = self.meObject->getACL();
    return [[MeIOSACL alloc]initWithACL:&acl];
}

- (MeIOSObject *)meObjectWithKey:(NSString *)key {
    MeObject object = self.meObject->objectValue([key UTF8String]);
    return [[MeIOSObject alloc] initWithMeObject: &object];
}

- (void)setObject:(MeIOSObject *)iosObject key:(NSString *)key {
    if (!key || key.length <= 0 || !iosObject) return;
    self.meObject->set([key UTF8String], (MeObject *)(iosObject.getObject));
}

- (NSMutableArray *)meObjectArrayWithKey:(NSString *)key {
    if (!key || key.length <= 0) return nil;
    JSONArray array = self.meObject->arrayValue([key UTF8String]);
    NSMutableArray *jsonArray = [NSMutableArray array];
    for (int i = 0; i < array.size(); i++) {
        JSONObject json = array.jsonValue(i);
        MeObject *childObject = new MeObject();
        childObject->copy(&json, false);
        [jsonArray addObject:[[MeIOSObject alloc] initWithMeObject:childObject]];
        delete childObject;
    }
    
    return jsonArray;
}

- (void)setObjects:(NSArray *)iosObjects key:(NSString *)key {
    if (!key || key.length <= 0 || !iosObjects || iosObjects.count <= 0) return;
    JSONArray *array = new JSONArray();
    __block NSString *className = @"";
    [iosObjects enumerateObjectsUsingBlock:^(id  _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        MeIOSObject *object = (MeIOSObject *)obj;
        MeObject *newObject = new MeObject((MeObject *)(object.getObject), false);
        array->append(newObject);
        className = object.className;
    }];
    
    self.meObject->set([key UTF8String], array, [className UTF8String]);
    delete array;
}

- (void)addObject:(MeIOSObject *)iosObject key:(NSString *)key {
    if (!key || key.length <= 0 || !iosObject) return;
    self.meObject->add([key UTF8String], (MeObject *)(iosObject.getObject));
}

@end
