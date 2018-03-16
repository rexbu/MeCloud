//
//  MeIOSNetManager.h
//  MeCloud
//
//  Created by super on 2017/8/10.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MeIOSObject.h"
#import "MeIOSException.h"
#import "MeIOSUser.h"
#import "MeIOSQuery.h"
#import "MeIOSJoinQuery.h"

typedef void (^MeIOSJSONCallbackBlock)(MeIOSJSONObject *obj, MeIOSException *error, NSInteger size);

typedef void (^MeIOSJSONListCallbackBlock)(NSMutableArray *objs, MeIOSException *error, NSInteger size);

typedef void (^MeIOSCallbackBlock)(MeIOSObject *obj, MeIOSException *error, NSInteger size);

typedef void (^MeIOSListCallbackBlock)(NSMutableArray *objs, MeIOSException *error, NSInteger size);

typedef void (^MeIOSUserCallbackBlock)(MeIOSUser *obj, MeIOSException *error, NSInteger size);

typedef void (^MeIOSDownloadCallbackBlock)(MeIOSObject *obj, NSString *localPath, MeIOSException *error, NSInteger size);

typedef void (^MeIOSHttpFileProgressBlock)(NSUInteger totalWriten, NSUInteger totalExpectWrite);

@interface MeIOSNetManager : NSObject

@property (nonatomic, strong) MeIOSUser *currentUser;

@property (nonatomic, strong, readonly) NSString *baseUrl;

@property (nonatomic, strong, readonly) NSString *cookie;

+ (instancetype)shared;

// 设置MeCloud的域名
- (void)setBaseUrl:(NSString *)url;

// 设置http头
- (void)addHttpHeaderWithKey:(NSString *)key value:(NSString *)value;

// 显示log
- (void)showLog:(BOOL)showed;

// 系统默认是20s
- (void)setHttpTimeout:(NSUInteger)timeout;

// 目前注销只做本地注销
- (void)logout;

// 保存MeIOSObject，会向服务器发送请求
- (void)saveWithIOSObject:(MeIOSObject *)iOSObject complete:(MeIOSCallbackBlock)block;

// 删除一条数据，只支持部分表，业务层定义哪些表可以支持删除
- (void)deleteWithIOSObject:(MeIOSObject *)iOSObject complete:(MeIOSJSONCallbackBlock)block;

// 根据objectId做单条查询
- (void)getIOSObjectWithID:(NSString *)objectID className:(NSString *)classname complete:(MeIOSCallbackBlock)block;

// 批量查询
- (void)getIOSObjects:(MeIOSQuery *)query complete:(MeIOSListCallbackBlock)block;

// 关联查询
- (void)getIOSObjectsWithJoinQuery:(MeIOSJoinQuery *)query complete:(MeIOSListCallbackBlock)block;

// 登录接口
- (void)loginWithUsername:(NSString *)name password: (NSString *)password complete:(MeIOSUserCallbackBlock)block;

// 注册接口
- (void)signUpWithUsername:(NSString *)name password: (NSString *)password complete:(MeIOSUserCallbackBlock)block;

// 更改密码接口
- (void)changePasswordWithUserName:(NSString *)name new: (NSString *)newPassword complete:(MeIOSUserCallbackBlock)block;

// 下载文件，url可以为的http或者https的绝对链接，也可以支持自己上传到阿里服务器的相对链接，相对链接的值为objectId
- (void)downloadFile:(NSString *)url type:(NSString *)filename complete:(MeIOSDownloadCallbackBlock)complete progress:(MeIOSHttpFileProgressBlock)progress;

// MeCloud上传二进制数据
- (void)uploadData:(NSData *)data type:(NSString *)type complete:(MeIOSCallbackBlock)complete progress:(MeIOSHttpFileProgressBlock)progress;

// MeCloud上传文件
- (void)uploadFile:(NSString *)path type:(NSString *)type complete:(MeIOSCallbackBlock)complete progress:(MeIOSHttpFileProgressBlock)progress;

// 特定接口上传二进制数据
- (void)postDataWithUrl:(NSString *)url data:(NSData *)data complete:(MeIOSCallbackBlock)complete progress:(MeIOSHttpFileProgressBlock)progress;

#pragma --mark 特定Api的接口
// 单独post/put api的请求，提交表单数据
- (void)saveWithUrl:(NSString *)url json:(MeIOSJSONObject *)json complete:(MeIOSJSONCallbackBlock)block;

- (void)getWithUrl:(NSString *)url json:(MeIOSJSONObject *)json complete:(MeIOSJSONCallbackBlock)block;

// 获取短信验证码
- (void)requestSMSAuthCode:(NSString *)phone complete:(MeIOSJSONCallbackBlock)block;

// 磁盘缓存数据
- (void)saveJSONToCache:(MeIOSJSONObject *)document key:(NSString *)key;

// 从磁盘缓存读取数据
- (MeIOSJSONObject *)getJSONFromCache:(NSString *)key;

// 从磁盘缓存删除数据
- (void)removeJSONFromCache:(NSString *)key;

@end
