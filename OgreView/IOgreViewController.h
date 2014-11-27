//
//  IOgreViewController.h
//  OgreView
//
//  Created by ToyamaTokanava on 11/25/14.
//  Copyright (c) 2014 ToyamaTokanava. All rights reserved.
//

#ifndef OgreView_IOgreViewController_h
#define OgreView_IOgreViewController_h


@protocol IOgreViewController <NSObject>

@property (strong, nonatomic) NSString *selectedModel;
@property (assign, nonatomic) BOOL celShading;

- (void)destroy;

@end


#endif
