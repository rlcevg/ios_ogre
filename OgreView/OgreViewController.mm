//
//  OgreViewController.m
//  OgreView
//
//  Created by ToyamaTokanava on 11/21/14.
//  Copyright (c) 2014 ToyamaTokanava. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import "OgreViewController.h"
#include "OgreController.h"


@interface OgreViewController () {
    OgreController m_Ogre;
}

@property (weak, nonatomic) CADisplayLink *displayLink;

@end


@implementation OgreViewController

#pragma mark - Life cycle

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.

    _selectedModel = @"ogrehead";
    _celShading = YES;
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];

    if (self.displayLink) {
        m_Ogre.resetStats();
        self.displayLink.paused = NO;
        return;
    }

    [self.view.superview layoutIfNeeded];
    self.view.contentScaleFactor = [UIScreen mainScreen].scale;
    UIWindow *keyWindow = [UIApplication sharedApplication].delegate.window;
    [self initOgreWithWindow:keyWindow view:self.view controller:self];

    m_Ogre.displayModel([self.selectedModel UTF8String], self.celShading);
    [self renderOneFrame:self];

    // Reset event times and reallocate the date and displaylink objects
    Ogre::Root::getSingleton().clearEventTimes();

    CADisplayLink *dl = [CADisplayLink displayLinkWithTarget:self selector:@selector(renderOneFrame:)];
    // Defaulting to 2 means that we run at 30 frames per second. For 60 frames, use a value of 1.
    // 30 FPS is usually sufficient and results in lower power consumption.
    dl.frameInterval = 2;
//    [dl addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [dl addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];
    self.displayLink = dl;
}

- (void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];

    self.displayLink.paused = YES;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.

    if ([self isViewLoaded] && (self.view.window == nil)) {
        [self destroy];
    }
}

- (void)dealloc
{
}

#pragma mark - Setters/Getters

- (void)setSelectedModel:(NSString *)selectedModel
{
    if (![selectedModel isEqualToString:_selectedModel]) {
        _selectedModel = selectedModel;
        m_Ogre.displayModel([selectedModel UTF8String], self.celShading);
        m_Ogre.resetStats();
    }
}

- (void)setCelShading:(BOOL)celShading
{
    if (_celShading != celShading) {
        _celShading = celShading;
        m_Ogre.displayModel([self.selectedModel UTF8String], celShading);
        m_Ogre.resetStats();
    }
}

#pragma mark - Helpers

- (void)initOgreWithWindow:(UIWindow *)window view:(UIView *)view controller:(UIViewController *)controller
{
    @autoreleasepool {
        try {
            Ogre::String wndHandle = Ogre::StringConverter::toString((long)window);
            Ogre::String viewHandle = Ogre::StringConverter::toString((long)view);
            Ogre::String vcHandle = Ogre::StringConverter::toString((long)controller);
            if (!m_Ogre.init(wndHandle, viewHandle, vcHandle)) {
                [[[UIAlertView alloc] initWithTitle:NSLocalizedString(@"OGRE", nil)
                                            message:NSLocalizedString(@"Couldn't properly initialize rendering engine!", nil)
                                           delegate:nil
                                  cancelButtonTitle:@"OK"
                                  otherButtonTitles:nil] show];
                Ogre::Root::getSingleton().saveConfig();
            }
            Ogre::Root::getSingleton().getRenderSystem()->_initRenderTargets();
            // Clear event times
            Ogre::Root::getSingleton().clearEventTimes();
        } catch (Ogre::Exception &e) {
            std::cerr << "An exception has occurred: " <<
            e.getFullDescription().c_str() << std::endl;
        }
    }
}

- (void)destroy
{
    [self.displayLink invalidate];
    self.displayLink = nil;
    m_Ogre.release();
}

- (void)renderOneFrame:(id)sender
{
    Ogre::Root::getSingleton().renderOneFrame();
}

#pragma mark - Actions

- (IBAction)handlePan:(UIPanGestureRecognizer *)sender
{
    CGPoint translation = [sender translationInView:self.view];
    m_Ogre.updateCamera(translation.x, translation.y);
    [sender setTranslation:CGPointMake(0, 0) inView:self.view];
}

@end
