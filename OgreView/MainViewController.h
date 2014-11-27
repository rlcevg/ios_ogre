//
//  MainViewController.h
//  OgreView
//
//  Created by ToyamaTokanava on 11/24/14.
//  Copyright (c) 2014 ToyamaTokanava. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface MainViewController : UIViewController

- (IBAction)switchShading:(UISwitch *)sender;
- (IBAction)unwindToMain:(UIStoryboardSegue *)unwindSegue;
- (IBAction)doClose:(UIButton *)sender;

@end
