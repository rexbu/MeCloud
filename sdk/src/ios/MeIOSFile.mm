//
//  MeIOSFile.m
//  MeCloud
//
//  Created by super on 2017/9/12.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "MeIOSFile.h"
#import "MeFile.h"

@implementation MeIOSFile

- (id)initWithMeObject:(MeIOSObject *)iosObject {
    if (self = [super init]) {
        object = new MeFile((MeObject *)iosObject.getObject, false);
    }
    
    return self;
}

- (id)initWithObjectID:(NSString *)objectID className:(NSString *)className {
  if (self = [super init]) {
    object = new MeFile([objectID UTF8String], [className UTF8String]);
  }
  
  return self;
}

- (MeFile *)meFile {
    return (MeFile *)object;
}

- (NSString *)imageUrlWithSize:(CGSize)size {
    return [NSString stringWithUTF8String:self.meFile->imageUrl((int)size.width, (int)size.height)];
}

- (NSString *)imageUrl {
    return [NSString stringWithUTF8String:self.meFile->imageUrl()];
}

- (NSString *)imageCropUrl:(CGRect)rect {
    return [NSString stringWithUTF8String:self.meFile->imageCropUrl(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height)];
}

@end
