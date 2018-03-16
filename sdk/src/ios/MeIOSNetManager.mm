//
//  MeIOSNetManager.m
//  MeCloud
//
//  Created by super on 2017/8/10.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "MeIOSNetManager.h"
#import "MeDownloadFile.h"
#import "MeUploadFile.h"
#import "MeIOSCallback.h"
#import "MeHttpFileCallback.h"
#import "MeIOSHttpDataCallback.h"
#import "MeSMS.h"
#import <vector>
#import <string>
#import <iterator>
#import "MeUser.h"
#import "MeQuery.h"
#import "MeJoinQuery.h"

using namespace mc;

@implementation MeIOSNetManager

+ (instancetype)shared {
    static MeIOSNetManager *shared = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        shared = [[MeIOSNetManager alloc] init];
    });

    return shared;
}

- (NSString *)baseUrl {
  return [NSString stringWithUTF8String:MeCloud::shareInstance()->restUrl()];
}

- (void)setBaseUrl:(NSString *)url {
    MeCloud::setBaseUrl([url UTF8String]);
}

- (NSString *)cookie {
    const char *cookie = MeCloud::shareInstance()->cookie();
    if (!cookie) {
        return nil;
    }
    
    return [NSString stringWithUTF8String:cookie];
}

- (void)addHttpHeaderWithKey:(NSString *)key value:(NSString *)value {
    if (!key || key.length <= 0 || !value || value.length <= 0) return;
    MeCloud::shareInstance()->addHttpHeader([key UTF8String], [value UTF8String]);
}

- (void)showLog:(BOOL)showed {
    MeCloud::showLog(showed);
}

- (void)setHttpTimeout:(NSUInteger)timeout {
    MeCloud::setTimeout((uint32_t)timeout);
}

- (MeIOSUser *)currentUser {
    if (!_currentUser) {
        MeUser *meUser = MeUser::currentUser();
        if (meUser) {
            _currentUser = [[MeIOSUser alloc] initWithMeUser: meUser];
        }
    }
    
    return _currentUser;
}

- (void)logout {
    _currentUser = nil;
    MeUser::currentUser()->logout();
}

- (void)saveWithIOSObject:(MeIOSObject *)iOSObject complete:(MeIOSCallbackBlock)block {
    MeObject *meObject = (MeObject *)(iOSObject.getObject);
    meObject->save(^(MeObject *obj, MeException *error, uint32_t size) {
        if (!error) {
            block(iOSObject, nil, size);
        } else {
            block(nil, [[MeIOSException alloc] initWithMeException:error], size);
        }
    });
}

- (void)deleteWithIOSObject:(MeIOSObject *)iOSObject complete:(MeIOSJSONCallbackBlock)block {
    MeBlockCallback* callback = new MeBlockCallback();
    MeCallbackBlock cloudBlock = ^(MeObject* obj, MeException* error, uint32_t size) {
        if (!error) {
            MeIOSJSONObject *json = [[MeIOSJSONObject alloc]initWithJSONObject:obj];
            block(json, nil, size);
        } else {
            block(nil, [[MeIOSException alloc] initWithMeException:error], size);
        }
    };
    callback->setBlock(cloudBlock);
    MeObject *meObject = (MeObject *)(iOSObject.getObject);
    meObject->deleteObject(callback);
}

- (void)saveWithUrl:(NSString *)url json:(MeIOSJSONObject *)json complete:(MeIOSJSONCallbackBlock)block {
    MeBlockCallback* callback = new MeBlockCallback();
    MeCallbackBlock cloudBlock = ^(MeObject* obj, MeException *error, uint32_t size) {
        if (!error) {
            MeIOSJSONObject *json = [[MeIOSJSONObject alloc]initWithJSONObject:obj];
            block(json, nil, size);
        } else {
            block(nil, [[MeIOSException alloc] initWithMeException:error], size);
        }
    };
    callback->setBlock(cloudBlock);

    MeCloud::shareInstance()->post([url UTF8String], [json.jsonString UTF8String], callback);
}

- (void)getIOSObjectWithID:(NSString *)objectID className:(NSString *)classname complete:(MeIOSCallbackBlock)block {
    if (!objectID || objectID.length < BS_UNIQUE_ID_EFFECT_LENGTH || !classname || classname.length <= 0) {
        return;
    }
    MeIOSQuery *query = [[MeIOSQuery alloc] initWithClassName:classname];
    MeQuery *meQuery = (MeQuery *)query.getObject;
    meQuery->get([objectID UTF8String], ^(MeObject *obj, MeException *error, uint32_t size) {
        if (!error) {
            MeIOSObject *iOSObject = [[MeIOSObject alloc] initWithMeObject:obj];
            block(iOSObject, nil, size);
        } else {
            block(nil, [[MeIOSException alloc] initWithMeException:error], size);
        }
    });
}

- (void)getIOSObjects:(MeIOSQuery *)query complete:(MeIOSListCallbackBlock)block {
    MeQuery *meQuery = (MeQuery *)query.getObject;
    meQuery->find(^(MeObject *obj, MeException *error, uint32_t size) {
        if (!error) {
            NSMutableArray *array = [[NSMutableArray alloc] init];
            for (int i = 0; i < size; i++) {
                MeIOSObject *iOSObject = [[MeIOSObject alloc] initWithMeObject:&obj[i]];
                [array addObject:iOSObject];
            }
            block(array, nil, size);
        } else {
            block(nil, [[MeIOSException alloc] initWithMeException:error], size);
        }
    });
}

- (void)getIOSObjectsWithJoinQuery:(MeIOSJoinQuery *)query complete:(MeIOSListCallbackBlock)block {
    MeJoinQuery *joinQuery = (MeJoinQuery *)query.getObject;
    MeBlockCallback* callback = new MeBlockCallback();
    MeCallbackBlock cloudBlock = ^(MeObject* obj, MeException *error, uint32_t size) {
        if (!error) {
            NSMutableArray *array = [[NSMutableArray alloc] init];
            for (int i = 0; i < size; i++) {
                MeIOSObject *iOSObject = [[MeIOSObject alloc] initWithMeObject:&obj[i]];
                [array addObject:iOSObject];
            }
            block(array, nil, size);
        } else {
            block(nil, [[MeIOSException alloc] initWithMeException:error], size);
        }
    };
    
    callback->setBlock(cloudBlock);
    joinQuery->find(callback);
}

- (void)getWithUrl:(NSString *)url json:(MeIOSJSONObject *)json complete:(MeIOSJSONCallbackBlock)block {
    MeBlockCallback* callback = new MeBlockCallback();
    MeCallbackBlock cloudBlock = ^(MeObject* obj, MeException *error, uint32_t size) {
        if (!error) {
            MeIOSJSONObject *object = [[MeIOSJSONObject alloc] initWithJSONObject:obj];
            block(object, nil, size);
        } else {
            block(nil, [[MeIOSException alloc] initWithMeException:error], size);
        }
    };
    
    callback->setBlock(cloudBlock);
    
    map<string, string> param;
    if (json) {
        JSONObject *object = (JSONObject *)(json.getObject);
        param = object->stringdict();
    }
    
    MeCloud::shareInstance()->get([url UTF8String], param, callback);
}

- (void)loginWithUsername:(NSString *)name password: (NSString *)password complete:(MeIOSUserCallbackBlock)block {
    MeIOSUser *user = [[MeIOSUser alloc] init];
    MeUser *meUser = (MeUser *)(user.getObject);
    meUser->login([name UTF8String], [password UTF8String], ^(MeObject *obj, MeException *error, uint32_t size) {
        if (!error) {
            block(user, nil, size);
        } else {
            block(nil, [[MeIOSException alloc] initWithMeException:error], size);
        }
    });
}

- (void)signUpWithUsername:(NSString *)name password: (NSString *)password complete:(MeIOSUserCallbackBlock)block {
    MeIOSUser *user = [[MeIOSUser alloc] init];
    MeUser *meUser = (MeUser *)(user.getObject);
    meUser->signup([name UTF8String], [password UTF8String], ^(MeObject *obj, MeException *error, uint32_t size) {
        if (!error) {
            block(user, nil, size);
        } else {
            block(nil, [[MeIOSException alloc] initWithMeException:error], size);
        }
    });
}

- (void)changePasswordWithUserName:(NSString *)name new: (NSString *)newPassword  complete:(MeIOSUserCallbackBlock)block {
    MeIOSUser *user = [[MeIOSUser alloc] init];
    MeUser *meUser = (MeUser *)(user.getObject);
    meUser->changePassword([name UTF8String], [newPassword UTF8String], ^(MeObject *obj, MeException *error, uint32_t size) {
        if (!error) {
            block(user, nil, size);
        } else {
            block(nil, [[MeIOSException alloc] initWithMeException:error], size);
        }
    });
}

- (void)downloadFile:(NSString *)url type:(NSString *)filename complete:(MeIOSDownloadCallbackBlock)complete progress:(MeIOSHttpFileProgressBlock)progress {
    MeDownloadFile *file = new MeDownloadFile();
    file->setDownloadUrl([url UTF8String]);
    file->setFilename([filename UTF8String]);
    file->download(^(MeFile *obj, MeException *error, uint32_t size) {
        if (!error) {
            MeIOSObject * object = [[MeIOSObject alloc] initWithMeObject:obj];
            complete(object, [NSString stringWithUTF8String:file->filePath()], nil, size);
        } else {
            complete(nil, nil, [[MeIOSException alloc] initWithMeException:error], size);
        }
        
        delete file;
    }, ^(uint64_t writen, uint64_t total_writen, uint64_t total_expect_write) {
        if (progress) {
            progress((NSUInteger)total_writen, (NSUInteger)total_expect_write);
        }
    });
}

- (void)uploadFile:(NSString *)path type:(NSString *)type complete:(MeIOSCallbackBlock)complete progress:(MeIOSHttpFileProgressBlock)progress {
    MeUploadFile *file = new MeUploadFile();
    file->put("type", [type UTF8String]);
    file->setUploadFilePath([path UTF8String]);
    file->upload(^(MeFile *obj, MeException *error, uint32_t size) {
        if (!error) {
            MeIOSObject * object = [[MeIOSObject alloc] initWithMeObject:obj];
            complete(object, nil, size);
        } else {
            complete(nil, [[MeIOSException alloc] initWithMeException:error], size);
        }
        delete file;
    }, ^(uint64_t writen, uint64_t total_writen, uint64_t total_expect_write) {
        if (progress) {
            progress((NSUInteger)total_writen, (NSUInteger)total_expect_write);
        }
    });
}

- (void)uploadData:(NSData *)data type:(NSString *)type complete:(MeIOSCallbackBlock)complete progress:(MeIOSHttpFileProgressBlock)progress {
    MeUploadFile *file = new MeUploadFile();
    file->put("type", [type UTF8String]);
    file->setUploadData((byte *)[data bytes], (int)(data.length));
    file->upload(^(MeFile *obj, MeException *error, uint32_t size) {
    if (!error) {
      MeIOSObject * object = [[MeIOSObject alloc] initWithMeObject:obj];
      complete(object, nil, size);
    } else {
      complete(nil, [[MeIOSException alloc] initWithMeException:error], size);
    }
    delete file;
  }, ^(uint64_t writen, uint64_t total_writen, uint64_t total_expect_write) {
    if (progress) {
      progress((NSUInteger)total_writen, (NSUInteger)total_expect_write);
    }
  });
}

- (void)postDataWithUrl:(NSString *)url data:(NSData *)data complete:(MeIOSCallbackBlock)complete progress:(MeIOSHttpFileProgressBlock)progress {
    MeIOSHttpDataCallback *callback = new MeIOSHttpDataCallback();
    MeCallbackBlock block = ^(MeObject* obj, MeException *error, uint32_t size) {
        if (!error) {
            MeIOSObject *json = [[MeIOSObject alloc]initWithJSONObject:obj];
            complete(json, nil, size);
        } else {
            complete(nil, [[MeIOSException alloc] initWithMeException:error], size);
        }
    };
    callback->setBlock(block);

    if (progress) {
        MeHttpFileProgressBlock progressBlock = ^(uint64_t writen, uint64_t total_writen, uint64_t total_expect_write) {
            progress((NSUInteger)total_writen, (NSUInteger)total_expect_write);
        };
        
        callback->setProgressBlock(progressBlock);
    }

    MeCloud::shareInstance()->postData([url UTF8String], (byte *)[data bytes], (int)data.length, callback);
}

- (void)requestSMSAuthCode:(NSString *)phone complete:(MeIOSJSONCallbackBlock)complete {
    MeBlockCallback* callback = new MeBlockCallback();
    MeCallbackBlock cloudBlock = ^(MeObject* obj, MeException *error, uint32_t size) {
        if (!error) {
            MeIOSJSONObject *json = [[MeIOSJSONObject alloc]initWithJSONObject:obj];
            complete(json, nil, size);
        } else {
            complete(nil, [[MeIOSException alloc] initWithMeException:error], size);
        }
    };
    callback->setBlock(cloudBlock);
    
    MeSMS::sendSMS([phone UTF8String], callback);
}

// 发送私信
- (void)sendMessage:(MeIOSObject *)object complete:(MeIOSCallbackBlock)block {
    
}

// 磁盘缓存数据
- (void)saveJSONToCache:(MeIOSJSONObject *)json key:(NSString *)key {
    if (!key) {
        return;
    }
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        MeCloud::shareInstance()->saveJSONToCache((JSONObject *)(json.getObject), [key UTF8String]);
    });
}

// 从磁盘缓存读取数据
- (MeIOSJSONObject *)getJSONFromCache:(NSString *)key {
    if (!key) {
        return nil;
    }
    
    JSONObject *json = MeCloud::shareInstance()->getJSONFromCache([key UTF8String]);
    if (json == NULL) {
        return NULL;
    }
    MeIOSJSONObject *object = [[MeIOSJSONObject alloc] initWithJSONObject:json];
    delete json;
    
    return object;
}

- (void)removeJSONFromCache:(NSString *)key {
    if (!key) {
        return;
    }
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        MeCloud::shareInstance()->removeJSONFromCache([key UTF8String]);
    });
}

@end
