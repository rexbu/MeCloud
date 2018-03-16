//
//  UIView+MeViewSizeExtension.m
//  MeDemo
//
//  Created by super on 2017/8/2.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "UIView+MeViewSizeExtension.h"

@implementation UIView (MeViewSizeExtension)

- (CGPoint)origin {
    return self.frame.origin;
}

- (void)setOrigin:(CGPoint)origin {
    CGRect frame = CGRectMake(origin.x, origin.y, self.size.width, self.size.height);
    self.frame = frame;
}

- (CGSize)size {
    return self.frame.size;
}

- (void)setSize:(CGSize)size {
    CGRect frame = CGRectMake(self.origin.x, self.origin.y, size.width, size.height);
    self.frame = frame;
}

- (CGFloat)width {
    return self.size.width;
}

- (void)setWidth:(CGFloat)width {
    CGSize size = CGSizeMake(width, self.height);
    self.size = size;
}

- (CGFloat)height {
    return self.size.height;
}

- (void)setHeight:(CGFloat)height {
    CGSize size = CGSizeMake(self.width, height);
    self.size = size;
}

- (CGFloat)left {
    return self.origin.x;
}

- (void)setLeft:(CGFloat)left {
    CGPoint origin = CGPointMake(left, self.top);
    self.origin = origin;
}

- (CGFloat)right {
    return self.left + self.width;
}

- (void)setRight:(CGFloat)right {
    CGPoint origin = CGPointMake(right - self.width, self.top);
    self.origin = origin;
}

- (CGFloat)top {
    return self.origin.y;
}

- (void)setTop:(CGFloat)top {
    CGPoint origin = CGPointMake(self.left, top);
    self.origin = origin;
}

- (CGFloat)bottom {
    return self.top + self.height;
}

- (void)setBottom:(CGFloat)bottom {
    CGPoint origin = CGPointMake(self.left, bottom - self.height);
    self.origin = origin;
}

- (CGFloat)centerX {
    return self.center.x;
}

- (void)setCenterX:(CGFloat)centerX {
    self.center = CGPointMake(centerX, self.centerY);
}

- (CGFloat)centerY {
    return self.center.y;
}


- (void)setCenterY:(CGFloat)centerY {
    self.center = CGPointMake(self.centerX, centerY);
}

@end
