//
//  SGTRListener.h
//  OgreView
//
//  Created by ToyamaTokanava on 11/26/14.
//  Copyright (c) 2014 ToyamaTokanava. All rights reserved.
//

#ifndef __OgreView__SGTRListener__
#define __OgreView__SGTRListener__

#include "OgreRTShaderSystem.h"


/** This class demonstrates basic usage of the RTShader system.
 It sub class the material manager listener class and when a target scheme callback
 is invoked with the shader generator scheme it tries to create an equivalent shader
 based technique based on the default technique of the given material.
 */
class ShaderGeneratorTechniqueResolverListener : public Ogre::MaterialManager::Listener
{
public:
    ShaderGeneratorTechniqueResolverListener(Ogre::RTShader::ShaderGenerator* pShaderGenerator);

    /** This is the hook point where shader based technique will be created.
     It will be called whenever the material manager won't find appropriate technique
     that satisfy the target scheme name. If the scheme name is out target RT Shader System
     scheme name we will try to create shader generated technique for it.
     */
    virtual Ogre::Technique* handleSchemeNotFound(unsigned short schemeIndex,
                                                  const Ogre::String& schemeName, Ogre::Material* originalMaterial, unsigned short lodIndex,
                                                  const Ogre::Renderable* rend);

protected:	
    Ogre::RTShader::ShaderGenerator*	mShaderGenerator;			// The shader generator instance.		
};

#endif /* defined(__OgreView__SGTRListener__) */
