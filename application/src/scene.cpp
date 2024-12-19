#include "scene.h"

#include "application.h"
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

void Scene::render(WGPURenderPassEncoder renderPass)
{
    for (Object* obj : mObjects)
    {
        obj->update();

        obj->render();

        wgpuRenderPassEncoderSetPipeline(renderPass, obj->getRenderPipeline());

        wgpuRenderPassEncoderSetBindGroup(renderPass, 0, obj->getBindGroup(), 0, nullptr);

        wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, obj->getVertexBuffer(), 0, wgpuBufferGetSize(obj->getVertexBuffer()));

        wgpuRenderPassEncoderDraw(renderPass, obj->getVertexCount(), 1, 0, 0);
    }

    if(physicsLayer == nullptr)
        return;

    physicsLayer->onUpdate(renderPass);

}
