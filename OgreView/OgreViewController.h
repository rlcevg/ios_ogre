//
//  OgreViewController.h
//  OgreView
//
//  Created by ToyamaTokanava on 11/21/14.
//  Copyright (c) 2014 ToyamaTokanava. All rights reserved.
//

#import "Config.h"
#import "IOgreViewController.h"


#ifdef OGRE_STATIC_GLES2

#import "EAGL/OgreEAGL2ViewController.h"
@interface OgreViewController : EAGL2ViewController <IOgreViewController>

#elif defined(OGRE_STATIC_GLES)

#import "EAGL/OgreEAGLViewController.h"
@interface OgreViewController : EAGLViewController <IOgreViewController>

#endif

@property (strong, nonatomic) NSString *selectedModel;
@property (assign, nonatomic) BOOL celShading;

- (void)destroy;

- (IBAction)handlePan:(UIPanGestureRecognizer *)sender;

@end
