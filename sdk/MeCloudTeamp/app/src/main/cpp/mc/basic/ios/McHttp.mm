/**
 * file :	Http.mm
 * author :	bushaofeng
 * create :	2016-08-25 17:20
 * func : 
 * history:
 */

#include "McHttp.h"
#import <Foundation/Foundation.h>
using namespace mc;

void* http_header_init(void* p){
    http_header_t* h = (http_header_t*)p;
    string_init(&h->first);
    string_init(&h->second);
    return h;
}
void http_header_set(http_header_t* header, const char* key, const char* value){
    header->first.append(&header->first, key);
    header->second.append(&header->second, value);
}
void http_header_destroy(void* p){
    http_header_t* h = (http_header_t*)p;
    
    string_destroy(&h->first);
    string_destroy(&h->second);
}

#pragma --mark "NSURLSession回调"
@interface McURLSessionDelegate : NSObject<NSURLSessionDelegate>
@property(nonatomic, assign)DownCallback* downloadCallback;
@property(nonatomic, assign)HttpCallback* httpCallback;
@property(nonatomic, retain)NSMutableData* responseData;
@end

#pragma --mark "IOSEnv"
namespace mc {

class IOSEnv{
public:
    IOSEnv(){
        m_session_config = [NSURLSessionConfiguration defaultSessionConfiguration];
        m_delegate_queue = [[NSOperationQueue alloc] init];
    }
    NSURLSessionConfiguration*  m_session_config;
    NSOperationQueue*           m_delegate_queue;
};

}

#pragma --mark "HttpSession"

HttpSession::HttpSession(){
    vector_init(&m_headers);
    m_interval = 60;
    m_iosenv = new IOSEnv();
    
//    if (m_config!=NULL) {
//        NSMutableDictionary* dict = [[NSMutableDictionary alloc] init];
//        for (int i=0; i<m_config->headerCount(); i++) {
//            http_header_t* header = m_config->header(i);
//            [dict setObject:[NSString stringWithUTF8String:header->second.mem] forKey:[NSString stringWithUTF8String:header->first.mem]];
//        }
//        [m_session_config setHTTPAdditionalHeaders:dict];
//    }
}

void HttpSession::addHttpHeader(const char* key, const char* value){
    http_header_t   header;
    http_header_init(&header);
    http_header_set(&header, key, value);
    vector_add(&m_headers, header);
}

void HttpSession::get(const char* curl, HttpCallback* callback){
    http(curl, "GET", NULL, 0, callback);
}

void HttpSession::post(const char* url, const char* body, uint32_t length, HttpCallback* callback){
    http(url, "POST", body, length, callback);
}

void HttpSession::put(const char* url, const char* body, uint32_t length, HttpCallback* callback){
    http(url, "PUT", body, length, callback);
}

void HttpSession::del(const char* url, HttpCallback* callback){
    http(url, "DELETE", NULL, 0, callback);
}

void HttpSession::http(const char* curl, const char* method, const char* body, uint32_t length, HttpCallback* callback){
    NSString* url = [NSString stringWithUTF8String:curl];
    NSURL *nsurl = [[NSURL alloc] initWithString:[url stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];
    NSMutableURLRequest* request = [NSMutableURLRequest requestWithURL:nsurl cachePolicy:NSURLRequestReloadIgnoringLocalCacheData timeoutInterval:m_interval];
    //[requet addValue:appKey forHTTPHeaderField:@"X-MeCloud-AppKey"];
    [request setHTTPMethod:[NSString stringWithUTF8String:method]];
    for (int i=0; i<vector_count(&m_headers); i++) {
        http_header_t* header = vector_get(&m_headers, i);
        [request addValue:[NSString stringWithUTF8String:header->second.mem] forHTTPHeaderField:[NSString stringWithUTF8String:header->first.mem]];
    }
    if (body!=NULL) {
        [request setHTTPBody:[NSData dataWithBytes:body length:length]];
    }
    
    McURLSessionDelegate* delegate = [[McURLSessionDelegate alloc] init];
    delegate.httpCallback = callback;
    NSURLSession* httpSession = [NSURLSession sessionWithConfiguration:m_iosenv->m_session_config delegate:delegate delegateQueue:m_iosenv->m_delegate_queue];
    
    NSURLSessionDataTask* task = [httpSession dataTaskWithRequest:request];
    [task resume];
}

void HttpSession::download(const char* url, DownCallback* callback){
    NSURL *nsurl = [[NSURL alloc] initWithString:[NSString stringWithUTF8String:url]];
    NSMutableURLRequest* request = [NSMutableURLRequest requestWithURL:nsurl];
    [request setHTTPMethod:@"GET"];
    for (int i=0; i<vector_count(&m_headers); i++) {
        http_header_t* header = vector_get(&m_headers, i);
        [request addValue:[NSString stringWithUTF8String:header->second.mem] forHTTPHeaderField:[NSString stringWithUTF8String:header->first.mem]];
    }
    
    McURLSessionDelegate* delegate = [[McURLSessionDelegate alloc] init];
    delegate.downloadCallback = callback;
    NSURLSession* downloadSession = [NSURLSession sessionWithConfiguration:m_iosenv->m_session_config delegate:delegate delegateQueue:m_iosenv->m_delegate_queue];
    NSURLSessionDownloadTask* task = [downloadSession downloadTaskWithRequest:request];
    
    [task resume];
}

HttpSession::~HttpSession(){
    delete m_iosenv;
    for (int i=0; i<vector_count(&m_headers); i++) {
        http_header_destroy(vector_get(&m_headers, i));
    }
    vector_destroy(&m_headers);
}

@implementation McURLSessionDelegate

-(id)init{
    self = [super init];
    _responseData = [[NSMutableData alloc] init];
    return self;
}

/**
 *  2.下载进度变化的时候被调用。多次调用。（iOS8可以不实现）
 *
 *  @param bytesWritten              本次写入的字节数
 *  @param totalBytesWritten         已经写入的字节数
 *  @param totalBytesExpectedToWrite 总的下载字节数
 */
- (void)URLSession:(NSURLSession *)session downloadTask:(NSURLSessionDownloadTask *)downloadTask didWriteData:(int64_t)bytesWritten totalBytesWritten:(int64_t)totalBytesWritten totalBytesExpectedToWrite:(int64_t)totalBytesExpectedToWrite
{
    if (_downloadCallback!=NULL)
    {
        _downloadCallback->progress(bytesWritten, totalBytesWritten, totalBytesExpectedToWrite);
    }
}
/**
 * 下载完成后被调用的方法（iOS7和iOS8都必须实现）
 */
- (void)URLSession:(NSURLSession *)session downloadTask:(NSURLSessionDownloadTask *)downloadTask didFinishDownloadingToURL:(NSURL *)location{
    if (_downloadCallback!=NULL)
    {
        _downloadCallback->done(200, BS_SUCCESS, [[location path] UTF8String]);
    }
    
    session = nil;
    downloadTask = nil;
    _responseData = nil;
}
/**
 *  3.断点续传的时候被调用的方法。(一般上面都不用写，iOS8可以不实现)
 */
- (void)URLSession:(NSURLSession *)session downloadTask:(NSURLSessionDownloadTask *)downloadTask didResumeAtOffset:(int64_t)fileOffset expectedTotalBytes:(int64_t)expectedTotalBytes
{
    
}

// 1. http回调请求
-(void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask didReceiveResponse:(NSURLResponse *)response completionHandler:(void (^)(NSURLSessionResponseDisposition))completionHandler
{
    //注意：需要使用completionHandler回调告诉系统应该如何处理服务器返回的数据
    //默认是取消的
    /*
            59         NSURLSessionResponseCancel = 0,        默认的处理方式，取消
            60         NSURLSessionResponseAllow = 1,         接收服务器返回的数据
            61         NSURLSessionResponseBecomeDownload = 2,变成一个下载请求
            62         NSURLSessionResponseBecomeStream        变成一个流
            63      */
    completionHandler(NSURLSessionResponseAllow);
}

//2.接收到服务器返回数据的时候会调用该方法，如果数据较大那么该方法可能会调用多次
-(void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask didReceiveData:(NSData *)data
{
    //拼接服务器返回的数据
    [self.responseData appendData:data];
}

//3.当请求完成(成功|失败)的时候会调用该方法，如果请求失败，则error有值
-(void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task didCompleteWithError:(NSError *)error
{
    if (error!=nil) {
        NSLog(@"Mc HTTP Error: %@", error);
        if (_httpCallback!=NULL)
        {
            _httpCallback->done((int)error.code, BS_INVALID, (char*)[error.domain UTF8String]);
        }
    }
    else if (_httpCallback!=NULL)
    {
        NSString* string = [[NSString alloc] initWithData:_responseData  encoding:NSUTF8StringEncoding];
        // callback->done(200, BS_SUCCESS, (char*)[data bytes]);
        _httpCallback->done(200, BS_SUCCESS, (char*)string.UTF8String);
    }
    
    session = nil;
    task = nil;
    _responseData = nil;
}

@end
