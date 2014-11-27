//
//  MainViewController.m
//  OgreView
//
//  Created by ToyamaTokanava on 11/24/14.
//  Copyright (c) 2014 ToyamaTokanava. All rights reserved.
//

#import "MainViewController.h"
#import "IOgreViewController.h"
#import "ListViewController.h"


@interface MainViewController () <ListViewControllerDelegate>

@property (weak, nonatomic) id <IOgreViewController> ogreVC;
@property (weak, nonatomic) ListViewController *listVC;

@end


@implementation MainViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view.

    self.listVC.models = @[@"ogrehead", @"fish", @"penguin"];
    self.listVC.delegate = self;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    NSString *segueName = segue.identifier;
    if ([segueName isEqualToString: @"EmbedOgre"]) {
        self.ogreVC = (id <IOgreViewController>)segue.destinationViewController;
    }
    else if ([segueName isEqualToString:@"EmbedList"]) {
        self.listVC = (ListViewController *)segue.destinationViewController;
    }
}

#pragma mark - ListViewControllerDelegate

- (void)selectModel:(NSString *)modelName
{
    self.ogreVC.selectedModel = modelName;
}

#pragma mark - Actions

- (IBAction)switchShading:(UISwitch *)sender
{
    self.ogreVC.celShading = sender.on;
}

- (IBAction)unwindToMain:(UIStoryboardSegue *)unwindSegue
{
}

- (IBAction)doClose:(UIButton *)sender
{
    [self dismissViewControllerAnimated:YES completion:^{
        [self.ogreVC destroy];
    }];
}

@end
