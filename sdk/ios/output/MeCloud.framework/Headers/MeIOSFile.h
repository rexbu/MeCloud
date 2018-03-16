//
//  MeIOSFile.h
//  MeCloud
//
//  Created by super on 2017/9/12.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "MeIOSObject.h"
#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>

@interface MeIOSFile : MeIOSObject

- (id)initWithMeObject:(MeIOSObject *)iosObject;

- (NSString *)imageUrlWithSize:(CGSize)size;

- (NSString *)imageUrl;

- (NSString *)imageCropUrl:(CGRect)rect;

@end
