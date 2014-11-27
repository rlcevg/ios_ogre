//
//  ListViewController.h
//  OgreView
//
//  Created by ToyamaTokanava on 11/25/14.
//  Copyright (c) 2014 ToyamaTokanava. All rights reserved.
//

#import <UIKit/UIKit.h>


@protocol ListViewControllerDelegate <NSObject>

- (void)selectModel:(NSString *)modelName;

@end


@interface ListViewController : UITableViewController

@property (weak, nonatomic) id <ListViewControllerDelegate> delegate;
@property (strong, nonatomic) NSArray *models;

@end
