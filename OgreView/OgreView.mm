//
//  OgreView.m
//  OgreView
//
//  Created by ToyamaTokanava on 11/21/14.
//  Copyright (c) 2014 ToyamaTokanava. All rights reserved.
//

#import "OgreView.h"
#include <Ogre.h>


@interface OgreView ()

@end


@implementation OgreView

- (void)layoutSubviews
{
    // Change the viewport orientation based upon layout request.
    // Note: This only operates on the main viewport, usually the main view.

    if (!Ogre::Root::getSingletonPtr()) {
        return;
    }

    // Get the window using the name that we saved
    RenderWindow *window = static_cast<RenderWindow *>(Ogre::Root::getSingleton().getRenderSystem()->getRenderTarget(mWindowName));

    if (window != NULL) {
        // Get the window size and initialize temp variables
        unsigned int width = (uint)self.bounds.size.width;
        unsigned int height = (uint)self.bounds.size.height;

        // Resize the window
        window->resize(width, height);

        // After rotation the aspect ratio of the viewport has changed, update that as well.
        if(window->getNumViewports() > 0) {
            Ogre::Viewport *viewPort = window->getViewport(0);
            viewPort->getCamera()->setAspectRatio((Real) width / (Real) height);
        }
    }
}

@end
