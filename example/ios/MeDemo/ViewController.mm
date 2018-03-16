//
//  ViewController.m
//  MeDemo
//
//  Created by Visionin on 17/7/4.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "ViewController.h"
#import "MeHeader.h"
#import "UIView+MeViewSizeExtension.h"
#import "MeIOSJSONObject.h"

@interface ViewController ()

@property (nonatomic, strong) UIImageView *imageView;
@property (nonatomic, strong) UITextView *textView;
@property (nonatomic, assign) CGFloat contentTop;
@property (nonatomic, assign) CGFloat contentLeft;

@property (nonatomic, strong) MeIOSJSONObject *object;
@property (nonatomic, strong) MeIOSJSONObject *object1;

@property (nonatomic, strong) MeIOSUser *currentUser;
@property (nonatomic, strong) NSData *imageData;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    _contentLeft = 20;
    
    [[MeIOSNetManager shared] setHttpTimeout:3];
    _currentUser = [MeIOSNetManager shared].currentUser;
    [[MeIOSNetManager shared] showLog:YES];
    [self initActionView];
    [self initContentView];
  
    [[MeIOSNetManager shared] getWithUrl:@"http://n01.me-yun.com:9000/profile/59ef0437ca71437ccf7d7fbb/list" json:nil complete:^(MeIOSJSONObject *obj, MeIOSException *error, NSInteger size) {
      NSMutableArray *array = [obj integerArrayWithKey:@"action_id"];
    }];
  
  MeIOSQuery *query = [[MeIOSQuery alloc] init];
  MeIOSAggregate *aggregate = [[MeIOSAggregate alloc] initWithClassName:@"Followee"];
  [aggregate addWhereEqualWithKey:@"user" value:@"59c79e48ca714365e3cf1784"];
  [aggregate setResponseKey:@"followeeCount"];
  [aggregate setMethod:IOSCOUNT];
  [query addAggregate:aggregate];
  [[MeIOSNetManager shared] getIOSObjects:query complete:^(NSMutableArray *objs, MeIOSException *error, NSInteger size) {
    
  }];
  
//    [[MeIOSNetManager shared] postDataWithUrl:@"http://n01.me-yun.com:9000/search/like/1" data:UIImageJPEGRepresentation([UIImage imageNamed:@"6.jpeg"], 0.5) complete:^(MeIOSObject *obj, MeIOSException *error, NSInteger size) {
//
//    } progress:^(NSUInteger totalWriten, NSUInteger totalExpectWrite) {
//
//    }];
}

- (void)initActionView {
    CGFloat margin = 6;
    CGFloat buttonHeight = 32;
    // 登录
    UIButton *loginButton = [[UIButton alloc]initWithFrame:CGRectMake(_contentLeft, 30, MeScreenWidth - _contentLeft * 2, buttonHeight)];
    loginButton.titleLabel.font = [UIFont boldSystemFontOfSize:16];
    [loginButton setTitle:@"登录" forState:UIControlStateNormal];
    if (_currentUser) {
        loginButton.tag = 1;
        [loginButton setTitle:@"注销" forState:UIControlStateNormal];
    }
    [loginButton setBackgroundColor:[UIColor greenColor]];
    [loginButton setTitleColor:[UIColor blueColor] forState:UIControlStateNormal];
    [loginButton addTarget:self action:@selector(loginAction:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:loginButton];
    
    // 注册
    UIButton *signUpButton = [[UIButton alloc]initWithFrame:CGRectMake(_contentLeft, loginButton.bottom + margin, MeScreenWidth - _contentLeft * 2, buttonHeight)];
    signUpButton.titleLabel.font = [UIFont boldSystemFontOfSize:16];
    [signUpButton setTitle:@"注册" forState:UIControlStateNormal];
    [signUpButton setBackgroundColor:[UIColor greenColor]];
    [signUpButton setTitleColor:[UIColor blueColor] forState:UIControlStateNormal];
    [signUpButton addTarget:self action:@selector(signUpAction:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:signUpButton];
    
    // 上传测试
    UIButton *uploadButton = [[UIButton alloc]initWithFrame:CGRectMake(loginButton.left, signUpButton.bottom + margin, loginButton.width / 2 - 5, buttonHeight)];
    uploadButton.backgroundColor = [UIColor lightGrayColor];
    uploadButton.titleLabel.font = [UIFont boldSystemFontOfSize:16];
    [uploadButton setTitle:@"上传文件" forState:UIControlStateNormal];
    [uploadButton setTitleColor:[UIColor redColor] forState:UIControlStateNormal];
    [uploadButton addTarget:self action:@selector(uploadFile:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:uploadButton];
    
    // 下载测试
    UIButton *downloadButton = [[UIButton alloc]initWithFrame:CGRectMake(uploadButton.right + margin, uploadButton.top, uploadButton.width, uploadButton.height)];
    downloadButton.backgroundColor = [UIColor lightGrayColor];
    downloadButton.titleLabel.font = [UIFont boldSystemFontOfSize:16];
    [downloadButton setTitle:@"下载文件" forState:UIControlStateNormal];
    [downloadButton setTitleColor:[UIColor redColor] forState:UIControlStateNormal];
    [downloadButton addTarget:self action:@selector(downloadFile:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:downloadButton];
    
    // 保存数据
    UIButton *saveButton = [[UIButton alloc]initWithFrame:CGRectMake(uploadButton.left, uploadButton.bottom + margin, uploadButton.width, uploadButton.height)];
    saveButton.backgroundColor = [UIColor lightGrayColor];
    saveButton.titleLabel.font = [UIFont boldSystemFontOfSize:16];
    [saveButton setTitle:@"保存数据" forState:UIControlStateNormal];
    [saveButton setTitleColor:[UIColor redColor] forState:UIControlStateNormal];
    [saveButton addTarget:self action:@selector(createTestObject) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:saveButton];
    
    // 查询某条数据
    UIButton *queryButton = [[UIButton alloc]initWithFrame:CGRectMake(downloadButton.left, saveButton.top, uploadButton.width, uploadButton.height)];
    queryButton.backgroundColor = [UIColor lightGrayColor];
    queryButton.titleLabel.font = [UIFont boldSystemFontOfSize:16];
    [queryButton setTitle:@"根据id查询数据" forState:UIControlStateNormal];
    [queryButton setTitleColor:[UIColor redColor] forState:UIControlStateNormal];
    [queryButton addTarget:self action:@selector(queryObject) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:queryButton];
    
    // 查寻列表数据
    UIButton *queryListButton = [[UIButton alloc]initWithFrame:CGRectMake(saveButton.left, saveButton.bottom + margin, saveButton.width, saveButton.height)];
    queryListButton.backgroundColor = [UIColor lightGrayColor];
    queryListButton.titleLabel.font = [UIFont boldSystemFontOfSize:16];
    [queryListButton setTitle:@"查询列表数据" forState:UIControlStateNormal];
    [queryListButton setTitleColor:[UIColor redColor] forState:UIControlStateNormal];
    [queryListButton addTarget:self action:@selector(queryList) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:queryListButton];
    
    // 修改某条数据
    UIButton *modifyButton = [[UIButton alloc]initWithFrame:CGRectMake(downloadButton.left, queryListButton.top, queryListButton.width, queryListButton.height)];
    modifyButton.backgroundColor = [UIColor lightGrayColor];
    modifyButton.titleLabel.font = [UIFont boldSystemFontOfSize:16];
    [modifyButton setTitle:@"修改数据" forState:UIControlStateNormal];
    [modifyButton setTitleColor:[UIColor redColor] forState:UIControlStateNormal];
    [modifyButton addTarget:self action:@selector(modifyAction) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:modifyButton];
    
    // 创建角色
    UIButton *createRoleButton = [[UIButton alloc]initWithFrame:CGRectMake(uploadButton.left, queryListButton.bottom + margin, uploadButton.width, uploadButton.height)];
    createRoleButton.backgroundColor = [UIColor lightGrayColor];
    createRoleButton.titleLabel.font = [UIFont boldSystemFontOfSize:16];
    [createRoleButton setTitle:@"创建角色" forState:UIControlStateNormal];
    [createRoleButton setTitleColor:[UIColor redColor] forState:UIControlStateNormal];
    [createRoleButton addTarget:self action:@selector(createRole) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:createRoleButton];
    
    // 查询角色
    UIButton *modifyRoleButton = [[UIButton alloc]initWithFrame:CGRectMake(downloadButton.left, createRoleButton.top, uploadButton.width, uploadButton.height)];
    modifyRoleButton.backgroundColor = [UIColor lightGrayColor];
    modifyRoleButton.titleLabel.font = [UIFont boldSystemFontOfSize:16];
    [modifyRoleButton setTitle:@"查询角色" forState:UIControlStateNormal];
    [modifyRoleButton setTitleColor:[UIColor redColor] forState:UIControlStateNormal];
    [modifyRoleButton addTarget:self action:@selector(findRoleAction) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:modifyRoleButton];
    
    // 查询角色
    UIButton *addUserToRoleButton = [[UIButton alloc]initWithFrame:CGRectMake(createRoleButton.left, createRoleButton.bottom + margin, uploadButton.width, uploadButton.height)];
    addUserToRoleButton.backgroundColor = [UIColor lightGrayColor];
    addUserToRoleButton.titleLabel.font = [UIFont boldSystemFontOfSize:16];
    [addUserToRoleButton setTitle:@"为角色增加成员" forState:UIControlStateNormal];
    [addUserToRoleButton setTitleColor:[UIColor redColor] forState:UIControlStateNormal];
    [addUserToRoleButton addTarget:self action:@selector(addUserToRoleAction) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:addUserToRoleButton];
    
    UIButton *sendMessageButton = [[UIButton alloc]initWithFrame:CGRectMake(modifyRoleButton.left, addUserToRoleButton.top, uploadButton.width, uploadButton.height)];
    sendMessageButton.backgroundColor = [UIColor lightGrayColor];
    sendMessageButton.titleLabel.font = [UIFont boldSystemFontOfSize:16];
    [sendMessageButton setTitle:@"发送私信" forState:UIControlStateNormal];
    [sendMessageButton setTitleColor:[UIColor redColor] forState:UIControlStateNormal];
    [sendMessageButton addTarget:self action:@selector(sendMessage) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:sendMessageButton];
    
    // 获取短信验证码
    UIButton *smsButton = [[UIButton alloc]initWithFrame:CGRectMake(addUserToRoleButton.left, addUserToRoleButton.bottom + margin, uploadButton.width, uploadButton.height)];
    smsButton.backgroundColor = [UIColor lightGrayColor];
    smsButton.titleLabel.font = [UIFont boldSystemFontOfSize:16];
    [smsButton setTitle:@"短信验证码" forState:UIControlStateNormal];
    [smsButton setTitleColor:[UIColor redColor] forState:UIControlStateNormal];
    [smsButton addTarget:self action:@selector(requestSMSAuth) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:smsButton];
    
    // 获取短信验证码
    UIButton *joinQueryButton = [[UIButton alloc]initWithFrame:CGRectMake(addUserToRoleButton.left, sendMessageButton.bottom + margin, uploadButton.width, uploadButton.height)];
    joinQueryButton.backgroundColor = [UIColor lightGrayColor];
    joinQueryButton.titleLabel.font = [UIFont boldSystemFontOfSize:16];
    [joinQueryButton setTitle:@"关联查询" forState:UIControlStateNormal];
    [joinQueryButton setTitleColor:[UIColor redColor] forState:UIControlStateNormal];
    [joinQueryButton addTarget:self action:@selector(joinquery) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:joinQueryButton];
    
    _contentTop = joinQueryButton.bottom + margin;
}

- (void)initContentView {
    _imageView = [[UIImageView alloc]initWithFrame:CGRectMake(_contentLeft, _contentTop, MeScreenWidth - _contentLeft * 2, self.view.height - _contentLeft - _contentTop)];
    _imageView.contentMode = UIViewContentModeScaleAspectFit;
    _imageView.clipsToBounds = true;
    [self.view addSubview:_imageView];
    
    _textView = [[UITextView alloc]initWithFrame:_imageView.frame];
    _textView.editable = false;
    _textView.font = [UIFont systemFontOfSize:12];
    _textView.backgroundColor = [UIColor clearColor];
    [self.view addSubview:_textView];
}

- (void)signUpAction:(UIButton *)button {
    [self resetContent];
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"MeCloud 注册" message:nil preferredStyle:(UIAlertControllerStyleAlert)];
    [alertController addTextFieldWithConfigurationHandler:^(UITextField *textField) {
        textField.placeholder = @"账号";
        NSNotificationCenter *center = [NSNotificationCenter defaultCenter];
        [center addObserver:self selector:@selector(notice:) name:UITextFieldTextDidChangeNotification object:textField];
    }];
    [alertController addTextFieldWithConfigurationHandler:^(UITextField *textField) {
        textField.placeholder = @"密码";
        textField.secureTextEntry = YES;
    }];
    // 在“好的”按钮按下时，我们让程序读取文本框中的值。
    UIAlertAction *okAction = [UIAlertAction actionWithTitle:@"好的" style:(UIAlertActionStyleDefault) handler:^(UIAlertAction *action) {
        // 获得文本框
        UITextField *login = alertController.textFields.firstObject;
        UITextField *password = alertController.textFields[1];
    
        [[MeIOSNetManager shared] signUpWithUsername:login.text password:password.text complete:^(MeIOSUser *obj, MeIOSException *err, NSInteger size) {
            if (!obj) {
                self.textView.text = err.description;
            } else {
                self.textView.text = obj.description;
                [self messageAlert:@"注册成功"];
            }
        }];
        
        // 移除通知观察者
        [[NSNotificationCenter defaultCenter] removeObserver:self name:UITextFieldTextDidChangeNotification object:nil];
    }];
    
    okAction.enabled = NO;
    UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:@"取消" style:(UIAlertActionStyleCancel) handler:^(UIAlertAction *action) {
        // 移除通知观察者
        [[NSNotificationCenter defaultCenter] removeObserver:self name:UITextFieldTextDidChangeNotification object:nil];
    }];
    
    [alertController addAction:okAction];
    [alertController addAction:cancelAction];
    [self presentViewController:alertController animated:YES completion:nil];
}

- (void)loginAction:(UIButton *)button {
    [self resetContent];
    if (button.tag == 1) {
        [[MeIOSNetManager shared] logout];
        button.tag = 0;
        [button setTitle:@"登录" forState:UIControlStateNormal];
        return;
    }
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"MeCloud 注册" message:nil preferredStyle:(UIAlertControllerStyleAlert)];
    [alertController addTextFieldWithConfigurationHandler:^(UITextField *textField) {
        textField.placeholder = @"账号";
        NSNotificationCenter *center = [NSNotificationCenter defaultCenter];
        [center addObserver:self selector:@selector(notice:) name:UITextFieldTextDidChangeNotification object:textField];
    }];
    [alertController addTextFieldWithConfigurationHandler:^(UITextField *textField) {
        textField.placeholder = @"密码";
        textField.secureTextEntry = YES;
    }];
    // 在“好的”按钮按下时，我们让程序读取文本框中的值。
    UIAlertAction *okAction = [UIAlertAction actionWithTitle:@"好的" style:(UIAlertActionStyleDefault) handler:^(UIAlertAction *action) {
        // 获得文本框
        UITextField *login = alertController.textFields.firstObject;
        UITextField *password = alertController.textFields[1];
        
        [[MeIOSNetManager shared] loginWithUsername:login.text password:password.text complete:^(MeIOSUser *obj, MeIOSException *err, NSInteger size) {
            if (!obj) {
                self.textView.text = err.errMessage;
            } else {
                self.textView.text = obj.description;
                self.currentUser = obj;
                [self messageAlert:@"登录成功"];
                button.tag = 1;
                [button setTitle:@"注销" forState:UIControlStateNormal];
            }
        }];
        
        // 移除通知观察者
        [[NSNotificationCenter defaultCenter] removeObserver:self name:UITextFieldTextDidChangeNotification object:nil];
    }];
    
    okAction.enabled = NO;
    UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:@"取消" style:(UIAlertActionStyleCancel) handler:^(UIAlertAction *action) {
        // 移除通知观察者
        [[NSNotificationCenter defaultCenter] removeObserver:self name:UITextFieldTextDidChangeNotification object:nil];
    }];
    
    [alertController addAction:okAction];
    [alertController addAction:cancelAction];
    [self presentViewController:alertController animated:YES completion:nil];
}

- (void)uploadFile: (UIButton *)button {
    [self resetContent];
    button.enabled = false;
//    NSString *path = [[NSBundle mainBundle] pathForResource:@"Screenshot_1500695711" ofType:@"jpg"];
    UIImage *image = [UIImage imageNamed:@"Screenshot_1500695711.jpg"];
    NSData *data = UIImageJPEGRepresentation(image, 1.0);
    [[MeIOSNetManager shared] uploadData:data type:@"jpg" complete:^(MeIOSObject *obj, MeIOSException *err, NSInteger size) {
        if (err) {
            self.textView.text = err.description;
        } else {
            self.textView.text = obj.description;
        }
        button.enabled = true;
    } progress:^(NSUInteger totalWriten, NSUInteger totalExpectWrite) {
        self.textView.text = [NSString stringWithFormat:@"完成%.2f", (double)totalWriten / totalExpectWrite];
    }];
}

- (void)downloadFile: (UIButton *)button {
    [self resetContent];
    [[MeIOSNetManager shared] downloadFile:@"http://n01.me-yun.com:8000/captcha/11111111111" type:@"1hrG1NH6" complete:^(MeIOSObject *obj, NSString *localPath, MeIOSException *err, NSInteger size) {
        if (err) {
            self.textView.text = err.description;
        } else {
            self.imageView.image = [UIImage imageWithContentsOfFile:localPath];
            self.textView.text = obj.description;
            self.textView.textColor = [UIColor whiteColor];
        }
        button.enabled = true;
    } progress:^(NSUInteger totalWriten, NSUInteger totalExpectWrite) {
        self.textView.text = [NSString stringWithFormat:@"完成%.2f", (double)totalWriten / totalExpectWrite];
    }];
}

- (void)queryObject {
    [self resetContent];
//    MeIOSQuery *query = [[MeIOSQuery alloc] initWithClassName:@"Test"];
//    [query addWhereEqualWithKey:@"_id" value:@"59af85edca7143621f9f42a8"];
    [[MeIOSNetManager shared] getIOSObjectWithID:@"2db19359ded468a20e0be754" className:@"Test" complete:^(MeIOSObject *obj, MeIOSException *err, NSInteger size) {
        if (err) {
            self.textView.text = err.description;
        } else {
            self.textView.text = obj.description;
        }
    }];
}

- (void)queryList {
    [self resetContent];
    MeIOSQuery *query = [[MeIOSQuery alloc] initWithClassName:@"UserInfo"];
    [query addWhereEqualWithKey:@"user" value:@"59f2f432ca7143036df6d7a9"];
//    [query addWhereEqualWithKey:@"username" value:@"ccc 0"];
//    [query addAscendSortKeys:@"upateAt"];
//    [query addNotSelectKey:@"acl"];
//    [query addLimit:10];
//    [query addOffset:10];
    
    [[MeIOSNetManager shared] getIOSObjects:query complete:^(NSMutableArray *objs, MeIOSException *err, NSInteger size) {
        if (!err) {
            [objs enumerateObjectsUsingBlock:^(id  _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
                self.textView.text = [self.textView.text stringByAppendingFormat:@"\n%@", [(MeIOSObject *)obj description]];
            }];
        } else {
            self.textView.text = err.description;
        }
    }];
}

- (void)createTestObject {
    [self resetContent];
//    "{\n\t\"file\":\t\"59ccc91fb0d653600ea84057\",\n\t\"mediaType\":\t0,\n\t\"width\":\t1024,\n\t\"height\":\t1024,\n\t\"upload\":\t\"59cb592bca714337f29ff5d1\",\n\t\"anonymous\":\ttrue,\n\t\"albumName\":\t\"\",\n\t\"publish\":\tfalse\n}"
  
    MeIOSObject *object = [[MeIOSObject alloc] initWithClassName:@"Media"];
    [object setStringValue:@"59ccc91fb0d653600ea84057" key:@"file"];
    [object setStringValue:@"59cb592bca714337f29ff5d1" key:@"upload"];
    [object setIntegerValue:1024 key:@"width"];
    [object setIntegerValue:1024 key:@"height"];
    [object setIntegerValue:0 key:@"mediaType"];
    [object setStringValue:@"" key:@"albumName"];
    [object setBoolValue:false key:@"publish"];
  
    [[MeIOSNetManager shared] saveWithIOSObject:object complete:^(MeIOSObject *obj, MeIOSException *err, NSInteger size) {
        if (obj) {
            self.textView.text = obj.description;
        } else {
            self.textView.text = err.errMessage;
        }
    }];
}

- (void)modifyAction {
    [self resetContent];
    MeIOSObject *object = [[MeIOSObject alloc] initWithClassName:@"Test"];
    [object setStringValue:@"无限引力" key:@"company😄"];
    _textView.text = object.jsonString;
    [[MeIOSNetManager shared] saveWithIOSObject:object complete:^(MeIOSObject *response, MeIOSException *err, NSInteger size) {
        if (err) {
            self.textView.text = object.jsonString;
        } else {
            self.textView.text = object.jsonString;
        }
    }];
}

- (void)createRole {
    [self resetContent];
    
    MeIOSRole *role = [[MeIOSRole alloc] initWithRoleName:@"supercai0"];
    if (_currentUser) {
        [role setUser:_currentUser];
    }
    
    [[MeIOSNetManager shared] saveWithIOSObject:role complete:^(MeIOSObject *obj, MeIOSException *err, NSInteger size) {
        if (err) {
            self.textView.text = err.description;
        } else {
            self.textView.text = obj.description;
        }
    }];
}

- (void)findRoleAction {
    [self resetContent];
    
    MeIOSJSONObject *object = [[MeIOSJSONObject alloc] init];
    [object setIntegerValue:10 key:@"limit"];
    [object setIntegerValue:4 key:@"skip"];
//    [[MeIOSNetManager shared] getWithUrl:@"1.0/class/Role" json:object complete:^(NSMutableArray *objects, MeIOSException *err, NSInteger size) {
//        if (objects) {
//            self.textView.text = objects.description;
//        } else {
//            self.textView.text = err.errMessage;
//        }
//    }];
    
//    MeIOSQuery *query = [[MeIOSQuery alloc] initWithClassName:@"Role"];
//    [query addLimit:10];
//    [[MeIOSNetManager shared] queryIOSObject:query complete:^(NSMutableArray *objs, MeIOSException *err, NSInteger size) {
//        if (!err) {
//            [objs enumerateObjectsUsingBlock:^(id  _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
//                self.textView.text = [self.textView.text stringByAppendingFormat:@"\n%@", [(MeIOSObject *)obj description]];
//            }];
//        } else {
//            self.textView.text = err.description;
//        }
//    }];
}

- (void)addUserToRoleAction {
    [self resetContent];
    
    MeIOSRole *role = [[MeIOSRole alloc] initWithRoleName:@"supercai0"];
    [role setUserWIthId:@"597adeddca71431a7dc0948f"];
    [[MeIOSNetManager shared] saveWithIOSObject:role complete:^(MeIOSObject *obj, MeIOSException *err, NSInteger size) {
        if (err) {
            self.textView.text = err.description;
        } else {
            self.textView.text = obj.description;
        }
    }];
}

- (void)sendMessage {
    [self resetContent];
    
    MeIOSJSONObject *object = [[MeIOSJSONObject alloc] init];
    [object setStringValue:@"5976af10ca714310f09f836b" key:@"uid"];
    [object setStringValue:@"hello" key:@"c"];
    [object setStringValue:@"chat" key:@"t"];
    
    [[MeIOSNetManager shared] saveWithUrl:@"1.0/push/sendMessage" json:object complete:^(MeIOSJSONObject *obj, MeIOSException *err, NSInteger size) {
        if (!err) {
            self.textView.text = obj.description;
        } else {
            self.textView.text = err.errMessage;
        }
    }];

}

- (void)requestSMSAuth {
    [[MeIOSNetManager shared] requestSMSAuthCode:@"13301320619" complete:^(MeIOSJSONObject *obj, MeIOSException *err, NSInteger size) {
        if (!err) {
            self.textView.text = obj.description;
        } else {
            self.textView.text = err.errMessage;
        }
    }];
}

- (void)joinquery {
//    MeIOSJoinQuery *query = [[MeIOSJoinQuery alloc] initWithClassName:@"Media"];
//    [query matchEqualToWithKey:@"_id" value:@"596cac72b4b33e28c0e746f8"];
//    [query addForeignTable:@"BackupUser" foreignKey:@"_id" localKey:@"backupUser" document:@"backupUser"];
//    [[MeIOSNetManager shared] getIOSObjectsWithJoinQuery:query complete:^(NSMutableArray *objs, MeIOSException *error, NSInteger size) {
//        NSLog(@"%@", objs.description);
//    }];
    
    MeIOSQuery *query = [[MeIOSQuery alloc] init];
    MeIOSAggregate *aggregate = [[MeIOSAggregate alloc] initWithClassName:@"Followee"];
    [aggregate addWhereEqualWithKey:@"user" value:@"59c79e48ca714365e3cf1784"];
    [aggregate setResponseKey:@"followeeCount"];
    [aggregate setMethod:IOSCOUNT];
    [query addAggregate:aggregate];
    
    MeIOSAggregate *relation = [[MeIOSAggregate alloc] initWithClassName:@"Followee"];
    [relation addWhereEqualWithKey:@"user" value:@"59c79e48ca714365e3cf1784"];
    [relation addWhereEqualWithKey:@"followee" value:@"59c8bf96ca71432f37932a7b"];
    [relation setResponseKey:@"follow"];
    [relation setMethod:IOSID];
    [query addAggregate:relation];
    
    MeIOSAggregate *fans = [[MeIOSAggregate alloc] initWithClassName:@"Followee"];
    [fans addWhereEqualWithKey:@"user" value:@"59c8bf96ca71432f37932a7b"];
    [fans addWhereEqualWithKey:@"followee" value:@"59c79e48ca714365e3cf1784"];
    [fans setResponseKey:@"fans"];
    [fans setMethod:IOSID];
    [query addAggregate:fans];
    
    MeIOSAggregate *relationAggregate = [[MeIOSAggregate alloc] initWithClassName:@"Followee"];
    [relationAggregate addWhereEqualWithKey:@"user" value:@"59c79e48ca714365e3cf1784"];
    [relationAggregate addWhereEqualWithKey:@"followee" value:@"59c8bf96ca71432f37932a7b"];
    [relationAggregate setResponseKey:@"followee"];
    [relationAggregate setMethod:IOSJSONOBJECT];
    [query addAggregate:relationAggregate];
    
    MeIOSAggregate *list = [[MeIOSAggregate alloc] initWithClassName:@"Followee"];
    [list addWhereEqualWithKey:@"user" value:@"59c79e48ca714365e3cf1784"];
    [list setResponseKey:@"list"];
    [list setMethod:IOSLIST];
    [query addAggregate:list];
    [[MeIOSNetManager shared] getIOSObjects:query complete:^(NSMutableArray *objs, MeIOSException *error, NSInteger size) {
        NSLog(@"%@", objs.description);
    }];
}

- (void)resetContent {
    _textView.text = @"";
    _imageView.image = nil;
    self.textView.textColor = [UIColor blackColor];
}

- (void)messageAlert:(NSString *)message {
    UIAlertController *contrller = [UIAlertController alertControllerWithTitle:@"提示" message:message preferredStyle:UIAlertControllerStyleAlert];
    [contrller addAction:[UIAlertAction actionWithTitle:@"知道了" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
    }]];
    
    [self presentViewController:contrller animated:false completion:nil];
}

// 检查“登录”文本框的内容
-(void)notice:(NSNotification *)notification{
    UIAlertController *alertController = (UIAlertController *)self.presentedViewController;
    if (alertController){
        UITextField *login = alertController.textFields.firstObject;
        UIAlertAction *okAction = alertController.actions.firstObject;
        okAction.enabled = ![login.text isEqual:@""];
    }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
