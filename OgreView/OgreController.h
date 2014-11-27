//
//  OgreController.h
//  OgreView
//
//  Created by ToyamaTokanava on 11/22/14.
//  Copyright (c) 2014 ToyamaTokanava. All rights reserved.
//

#ifndef __OgreView__OgreController__
#define __OgreView__OgreController__

#include <Ogre.h>
#include <OgreFileSystemLayer.h>
#include <OgreOverlaySystem.h>

#include "Config.h"
#include <OgreStaticPluginLoader.h>

#ifdef INCLUDE_RTSHADER_SYSTEM
    #include "SGTRListener.h"
#endif

#include <SdkTrays.h>


class OgreController : public Ogre::FrameListener {
public:
    OgreController();
    virtual ~OgreController();

    bool init(Ogre::String &wndHandle, Ogre::String &viewHandle, Ogre::String &vcHandle);
    void release();
    void resetStats();

    bool isInitialized();
    void displayModel(Ogre::String modelName, bool celShading);
    void updateCamera(Ogre::Real dx, Ogre::Real dy);

protected:
    Ogre::Root *m_pRoot;                        // owner
    Ogre::FileSystemLayer *m_pFileSystemLayer;  // owner
    Ogre::OverlaySystem *m_pOverlaySystem;      // owner
    Ogre::StaticPluginLoader m_StaticPluginLoader;
    Ogre::RenderWindow *m_pRenderWindow;

#ifdef INCLUDE_RTSHADER_SYSTEM
    Ogre::RTShader::ShaderGenerator          *mShaderGenerator;      // The Shader generator instance.
    ShaderGeneratorTechniqueResolverListener *mMaterialMgrListener;  // Shader generator material manager listener.
#endif

    Ogre::SceneManager *m_pSceneManager;
    Ogre::Camera *m_pCamera;
    Ogre::Viewport *m_pViewport;

    OgreBites::SdkTrayManager *m_pTrayManager;  // tray interface manager

    void createScene();
    void destroyScene();
    void createCameraView();
    void destroyCameraView();

    virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt);

private:
    bool m_Initialized;
    Ogre::SceneNode *m_pLightPivot;

#ifdef INCLUDE_RTSHADER_SYSTEM
    void initRTSS();
    bool initialiseRTShaderSystem(Ogre::SceneManager* sceneMgr);
    void destroyRTShaderSystem();
#endif
};

#endif /* defined(__OgreView__OgreController__) */
