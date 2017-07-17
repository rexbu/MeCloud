//
//  ViewController.m
//  MeDemo
//
//  Created by Visionin on 17/7/4.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "ViewController.h"
#include "Me.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    MeUser* user = new MeUser();
    user->signup("aaaa", "bbbb", ^(MeObject *obj, MeException *err, uint32_t size) {
        NSLog(@"%@", [NSString stringWithUTF8String:obj->toString()]);
    });
    MeObject* obj = new MeObject("Test");
    obj->put("key", "key");
    obj->put("value", "val");
    
    obj->save(^(MeObject* obj, MeException* err, uint32_t size){
        NSLog(@"%@", [NSString stringWithUTF8String:obj->toString()]);
    });
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


@end
