#include "scene.h"

#include "object.h"

void Scene::addObject(Object& obj)
{
    mObjects.push_back(obj);
}