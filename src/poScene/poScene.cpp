//
//  poScene.cpp
//  BasicTest
//
//  Created by Stephen Varga on 3/17/14.
//
//
#include "cinder/app/App.h"

#include "poScene.h"
#include "poNodeContainer.h"


namespace po {
    SceneRef Scene::create()
    {
        return create(po::NodeContainer::create());
    }
    
    
    SceneRef Scene::create(NodeContainerRef rootNode)
    {
        SceneRef scene(new Scene(rootNode));
        scene->getRootNode()->setScene(scene);
        return scene;
    }
    
    Scene::Scene(NodeContainerRef rootNode)
    : mRootNode(rootNode)
    , mAutoCam(true)
    , eventCenter(EventCenter::create())
    {
    }
    
    Scene::~Scene()
    {
    }
    
    void Scene::setDrawOffset(ci::Vec2f offset) {
        eventCenter->setInteractionOffset(offset);
    }
    
    #pragma mark -
    
    void Scene::update()
    {
        //Send a copy of all over our children to be processed
        processTrackingQueue();
        eventCenter->processEvents(allChildren);
        
        mRootNode->updateTree();
        
        if(mAutoCam)
            mCamera.setOrtho( 0, ci::app::getWindowWidth(), ci::app::getWindowHeight(), 0, -1, 1 );
    }
    
    void Scene::draw()
    {
        drawOrderCounter = 0;
        
        if (mAutoCam) {
			ci::gl::setMatricesWindow(ci::app::getWindowSize());
        }
        
        mRootNode->drawTree();

    }
    
    #pragma mark -
    
    uint32_t Scene::getNextDrawOrder()
    {
        return drawOrderCounter++;
    }
    
    void Scene::setRootNode(NodeContainerRef node)
    {
        if(node) {
            mRootNode->removeScene();
            mRootNode = node;
            mRootNode->setScene(shared_from_this());
        }
    }
    
    
    
    #pragma mark -
    void Scene::trackChildNode(NodeRef node) {
        mTrackingQueue[node] = true;
    }
    
    void Scene::untrackChildNode(NodeRef node) {
        mTrackingQueue[node] = false;
    }
    
    void Scene::processTrackingQueue()
    {
        for (auto &kv : mTrackingQueue) {
            std::vector<NodeRef>::iterator iter = std::find(allChildren.begin(), allChildren.end(), kv.first);
            if(kv.second && iter == allChildren.end()) {
                allChildren.push_back(kv.first);
            }
            
            else {
                if(iter != allChildren.end())
                    allChildren.erase(iter);
            }
        }
        
        mTrackingQueue.clear();
    }
}
