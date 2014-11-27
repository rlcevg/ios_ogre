//
//  OgreController.cpp
//  OgreView
//
//  Created by ToyamaTokanava on 11/22/14.
//  Copyright (c) 2014 ToyamaTokanava. All rights reserved.
//

#include "OgreController.h"
#include <macUtils.h>


OgreController::OgreController() :
    m_pRoot(nullptr),
    m_pFileSystemLayer(nullptr),
    m_pOverlaySystem(nullptr),
    m_pRenderWindow(nullptr),
    m_pSceneManager(nullptr),
    m_pCamera(nullptr),
    m_pViewport(nullptr),
    m_pTrayManager(nullptr),
#ifdef INCLUDE_RTSHADER_SYSTEM
    mShaderGenerator(nullptr),
    mMaterialMgrListener(nullptr),
#endif
    m_Initialized(false)
{
    m_pLightPivot = nullptr;
}

OgreController::~OgreController()
{
    if (m_Initialized) {
        release();
    }
}

bool OgreController::init(Ogre::String &wndHandle, Ogre::String &viewHandle, Ogre::String &vcHandle)
{
    bool result = true;
    Ogre::String rootpath = Ogre::macBundlePath() + "/";
    m_pFileSystemLayer = OGRE_NEW Ogre::FileSystemLayer(OGRE_VERSION_NAME);

    // create root
    Ogre::String logPath = Ogre::StringUtil::BLANK;
#ifdef DEBUG
    logPath = m_pFileSystemLayer->getWritablePath("ogre.log");
#endif
    m_pRoot = OGRE_NEW Ogre::Root(Ogre::StringUtil::BLANK, m_pFileSystemLayer->getWritablePath("ogre.cfg"), logPath);
    m_StaticPluginLoader.load();
    m_pOverlaySystem = OGRE_NEW Ogre::OverlaySystem();

    result &= m_pRoot->restoreConfig();

    // create window
    m_pRoot->initialise(false);
    Ogre::NameValuePairList parms;
    parms["externalWindowHandle"] = wndHandle;
    parms["externalViewHandle"] = viewHandle;
    parms["externalViewControllerHandle"] = vcHandle;
    Ogre::ConfigOptionMap &options = m_pRoot->getRenderSystem()->getConfigOptions();
    Ogre::ConfigOptionMap::iterator opt;
    if ((opt = options.find("FSAA")) != options.end()) {
        parms["FSAA"] = opt->second.currentValue;
    }  // have to pass FSAA manually :( @see OgreEAGL2Support.mm, OgreGLES2Support.cpp, OgreEAGL2Window.mm
    m_pRenderWindow = m_pRoot->createRenderWindow("Render", -1, -1, false, &parms);

    // load resources
    Ogre::String secName, typeName, archName;
    Ogre::ConfigFile cf;
    cf.load(rootpath + "resources.cfg");

    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
    while (seci.hasMoreElements()) {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i) {
            typeName = i->first;
            archName = i->second;
            // OS X does not set the working directory relative to the app,
            // In order to make things portable on OS X we need to provide
            // the loading with it's own bundle path location
            if (!Ogre::StringUtil::startsWith(archName, "/", false)) {  // only adjust relative dirs
                archName = Ogre::String(rootpath + archName);
            }
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
        }
    }
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    m_pRoot->addFrameListener(this);

    // create tray manager
    m_pTrayManager = new OgreBites::SdkTrayManager("TrayManager", m_pRenderWindow, OgreBites::InputContext(), nullptr);
    m_pTrayManager->showFrameStats(OgreBites::TL_BOTTOMLEFT);
    m_pTrayManager->showLogo(OgreBites::TL_BOTTOMRIGHT);
    m_pTrayManager->hideCursor();

    createScene();
    createCameraView();

#ifdef INCLUDE_RTSHADER_SYSTEM
    initRTSS();
#endif

    m_Initialized = true;
    return result;
}

void OgreController::release()
{
#ifdef INCLUDE_RTSHADER_SYSTEM
    mShaderGenerator->removeSceneManager(m_pSceneManager);
    destroyRTShaderSystem();
#endif

//    destroyCameraView();
//    destroyScene();

    if (m_pTrayManager) {
        delete m_pTrayManager;
        m_pTrayManager = nullptr;
    }

//    m_pRoot->removeFrameListener(this);
//    m_pRoot->destroyRenderTarget(m_pRenderWindow);
    if (m_pOverlaySystem) {
        OGRE_DELETE m_pOverlaySystem;
        m_pOverlaySystem = nullptr;
    }
    if (m_pRoot) {
        OGRE_DELETE m_pRoot;
        m_pRoot = nullptr;
    }
    if (m_pFileSystemLayer) {
        OGRE_DELETE m_pFileSystemLayer;
        m_pFileSystemLayer = nullptr;
    }
    m_StaticPluginLoader.unload();

    m_Initialized = false;
}

void OgreController::resetStats()
{
    m_pRoot->clearEventTimes();
    m_pRenderWindow->resetStatistics();
}

void OgreController::createScene()
{
    m_pSceneManager = m_pRoot->createSceneManager(Ogre::ST_GENERIC, "SceneManager");
    m_pSceneManager->addRenderQueueListener(m_pOverlaySystem);
    m_pSceneManager->setAmbientLight(Ogre::ColourValue(0.6f, 0.6f, 0.6f));

    // create a basic point light with an offset
    Ogre::Light *light = m_pSceneManager->createLight();
    light->setPosition(20, 40, 50);

    // attach the light to a pivot node
    m_pLightPivot = m_pSceneManager->getRootSceneNode()->createChildSceneNode();
    m_pLightPivot->attachObject(light);
}

void OgreController::destroyScene()
{
    m_pSceneManager->removeRenderQueueListener(m_pOverlaySystem);
    m_pRoot->destroySceneManager(m_pSceneManager);
}

void OgreController::createCameraView()
{
    m_pCamera = m_pSceneManager->createCamera("Camera");
    m_pCamera->setPosition(Ogre::Vector3(0, 60, 60));
    m_pCamera->lookAt(Ogre::Vector3(0, 0, 0));
    m_pCamera->setNearClipDistance(1);

    m_pViewport = m_pRenderWindow->addViewport(m_pCamera);
    m_pViewport->setBackgroundColour(Ogre::ColourValue::White);

    m_pCamera->setAspectRatio(Ogre::Real(m_pViewport->getActualWidth()) / Ogre::Real(m_pViewport->getActualHeight()));

    m_pRenderWindow->update();
}

void OgreController::destroyCameraView()
{
    m_pSceneManager->destroyCamera(m_pCamera);
}

bool OgreController::isInitialized()
{
    return m_Initialized;
}

void OgreController::displayModel(Ogre::String modelName, bool celShading)
{
    Ogre::Quaternion orient;
    if (m_pSceneManager->hasEntity("MyHead")) {
        m_pSceneManager->destroyEntity("MyHead");
        orient = m_pSceneManager->getSceneNode("ModelNode")->getOrientation();
        m_pSceneManager->getRootSceneNode()->removeAndDestroyChild("ModelNode");
    }

    Ogre::Entity *modelEnt = m_pSceneManager->createEntity("MyHead", modelName + ".mesh");
    Ogre::SceneNode *modelNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode("ModelNode");
    modelNode->setOrientation(orient);
    modelNode->attachObject(modelEnt);

    if (celShading) {
        modelEnt->setMaterialName("Examples/CelShading");

        enum ShaderParam { SP_SHININESS = 1, SP_DIFFUSE, SP_SPECULAR };
        Ogre::Vector4 vals[][3] = {
            {Ogre::Vector4(35, 0, 0, 0), Ogre::Vector4(1, 0.3, 0.3, 1), Ogre::Vector4(1,   0.6, 0.6, 1)},  // eyes
            {Ogre::Vector4(10, 0, 0, 0), Ogre::Vector4(0, 0.5, 0,   1), Ogre::Vector4(0.3, 0.5, 0.3, 1)},  // skin
            {Ogre::Vector4(25, 0, 0, 0), Ogre::Vector4(1, 1,   0,   1), Ogre::Vector4(1,     1, 0.7, 1)},  // earring
            {Ogre::Vector4(20, 0, 0, 0), Ogre::Vector4(1, 1,   0.7, 1), Ogre::Vector4(1,     1,   1, 1)},  // teeth
        };
        for (int i = 0; i < 4; i++) {
            try {
                Ogre::SubEntity *sub = modelEnt->getSubEntity(i);
                sub->setCustomParameter(SP_SHININESS, vals[i][SP_SHININESS - 1]);
                sub->setCustomParameter(SP_DIFFUSE, vals[i][SP_DIFFUSE - 1]);
                sub->setCustomParameter(SP_SPECULAR, vals[i][SP_SPECULAR - 1]);
            } catch (...) {
                break;
            }
        }
    }

    if (modelName == "fish") {
        modelNode->scale(8, 8, 8);
    }
}

bool OgreController::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    m_pTrayManager->frameRenderingQueued(evt);

    m_pLightPivot->yaw(Ogre::Degree(evt.timeSinceLastFrame * 30));
    m_pSceneManager->getSceneNode("ModelNode")->yaw(Ogre::Degree(evt.timeSinceLastFrame * 10));

    return true;
}

void OgreController::updateCamera(Ogre::Real dx, Ogre::Real dy)
{
    // CS_ORBIT camera. @see SdkCameraMan.h::injectMouseMove
    Ogre::SceneNode *target = m_pSceneManager->getSceneNode("ModelNode");
    Ogre::Real dist = (m_pCamera->getPosition() - target->_getDerivedPosition()).length();

    m_pCamera->setPosition(target->_getDerivedPosition());

    m_pCamera->yaw(Ogre::Degree(-dx * 0.5f));
    m_pCamera->pitch(Ogre::Degree(-dy * 0.5f));

    m_pCamera->moveRelative(Ogre::Vector3(0, 0, dist));
}

#ifdef INCLUDE_RTSHADER_SYSTEM

void OgreController::initRTSS()
{
    initialiseRTShaderSystem(m_pSceneManager);
    Ogre::MaterialPtr baseWhite = Ogre::MaterialManager::getSingleton().getByName("BaseWhite", Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);
    baseWhite->setLightingEnabled(false);
    mShaderGenerator->createShaderBasedTechnique("BaseWhite",
                                                 Ogre::MaterialManager::DEFAULT_SCHEME_NAME,
                                                 Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
    mShaderGenerator->validateMaterial(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME,
                                       "BaseWhite");
    baseWhite->getTechnique(0)->getPass(0)->setVertexProgram(baseWhite->getTechnique(1)->getPass(0)->getVertexProgram()->getName());
    baseWhite->getTechnique(0)->getPass(0)->setFragmentProgram(baseWhite->getTechnique(1)->getPass(0)->getFragmentProgram()->getName());

    // creates shaders for base material BaseWhiteNoLighting using the RTSS
    mShaderGenerator->createShaderBasedTechnique("BaseWhiteNoLighting",
                                                 Ogre::MaterialManager::DEFAULT_SCHEME_NAME,
                                                 Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
    mShaderGenerator->validateMaterial(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME,
                                       "BaseWhiteNoLighting");
    Ogre::MaterialPtr baseWhiteNoLighting = Ogre::MaterialManager::getSingleton().getByName("BaseWhiteNoLighting", Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);
    baseWhiteNoLighting->getTechnique(0)->getPass(0)->setVertexProgram(baseWhiteNoLighting->getTechnique(1)->getPass(0)->getVertexProgram()->getName());
    baseWhiteNoLighting->getTechnique(0)->getPass(0)->setFragmentProgram(baseWhiteNoLighting->getTechnique(1)->getPass(0)->getFragmentProgram()->getName());
}

/*-----------------------------------------------------------------------------
 | Initialize the RT Shader system.
 -----------------------------------------------------------------------------*/
bool OgreController::initialiseRTShaderSystem(Ogre::SceneManager* sceneMgr)
{
    if (Ogre::RTShader::ShaderGenerator::initialize())
    {
        mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();

        mShaderGenerator->addSceneManager(sceneMgr);

        // Setup core libraries and shader cache path.
        Ogre::StringVector groupVector = Ogre::ResourceGroupManager::getSingleton().getResourceGroups();
        Ogre::StringVector::iterator itGroup = groupVector.begin();
        Ogre::StringVector::iterator itGroupEnd = groupVector.end();
        Ogre::String shaderCoreLibsPath;
        Ogre::String shaderCachePath;

        for (; itGroup != itGroupEnd; ++itGroup)
        {
            Ogre::ResourceGroupManager::LocationList resLocationsList = Ogre::ResourceGroupManager::getSingleton().getResourceLocationList(*itGroup);
            Ogre::ResourceGroupManager::LocationList::iterator it = resLocationsList.begin();
            Ogre::ResourceGroupManager::LocationList::iterator itEnd = resLocationsList.end();
            bool coreLibsFound = false;

            // Try to find the location of the core shader lib functions and use it
            // as shader cache path as well - this will reduce the number of generated files
            // when running from different directories.
            for (; it != itEnd; ++it)
            {
                if ((*it)->archive->getName().find("RTShaderLib") != Ogre::String::npos)
                {
                    shaderCoreLibsPath = (*it)->archive->getName() + "/";
                    shaderCachePath = shaderCoreLibsPath;
                    coreLibsFound = true;
                    break;
                }
            }
            // Core libs path found in the current group.
            if (coreLibsFound)
                break;
        }

        // Core shader libs not found -> shader generating will fail.
        if (shaderCoreLibsPath.empty())
            return false;

        // Create and register the material manager listener.
        mMaterialMgrListener = new ShaderGeneratorTechniqueResolverListener(mShaderGenerator);
        Ogre::MaterialManager::getSingleton().addListener(mMaterialMgrListener);
    }

    return true;
}

/*-----------------------------------------------------------------------------
 | Destroy the RT Shader system.
 -----------------------------------------------------------------------------*/
void OgreController::destroyRTShaderSystem()
{
    // Restore default scheme.
    Ogre::MaterialManager::getSingleton().setActiveScheme(Ogre::MaterialManager::DEFAULT_SCHEME_NAME);

    // Unregister the material manager listener.
    if (mMaterialMgrListener != NULL)
    {
        Ogre::MaterialManager::getSingleton().removeListener(mMaterialMgrListener);
        delete mMaterialMgrListener;
        mMaterialMgrListener = NULL;
    }

    // Destroy RTShader system.
    if (mShaderGenerator != NULL)
    {
        Ogre::RTShader::ShaderGenerator::destroy();
        mShaderGenerator = NULL;
    }
}

#endif // INCLUDE_RTSHADER_SYSTEM
