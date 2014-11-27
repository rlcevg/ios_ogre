//
//  OgreView.h
//  OgreView
//
//  Created by ToyamaTokanava on 11/21/14.
//  Copyright (c) 2014 ToyamaTokanava. All rights reserved.
//

#import "Config.h"

#ifdef OGRE_STATIC_GLES2

    #import "EAGL/OgreEAGL2View.h"

    @interface OgreView : EAGL2View

    @end

#elif defined(OGRE_STATIC_GLES)

    #import "EAGL/OgreEAGLView.h"

    @interface OgreView : EAGLView

    @end

#endif