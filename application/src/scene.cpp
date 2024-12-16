#include "scene.h"

#include "object.h"

void Scene::addObject(Object* obj)
{
    mObjects.push_back(obj);
}

Object* Scene::getObjectByDebugName(const std::string& debugName)
{
    for(int i = 0; i < mObjects.size(); i++)
    {
        if(mObjects[i]->getDebugName() == debugName)
            return mObjects[i];
    }
    return nullptr;
}
