#include "application.h"

#include <emscripten.h>

#include "electron.h"

Application* Application::sInstance = nullptr;

Application::Application()
{
    sInstance = this;
	mWindow.create(1280, 720, "WebGPU");
}

void Application::init()
{
    mWindow.init();
    mRenderer.init();
    mPipeline.init();
}

void Application::run()
{

    Object obj;
    obj.configure("C:/Dev/eletromag/application/resources/sphere_with_vector.obj");

    std::vector<Object> objects;
    objects.push_back(obj);

    while (true)
    {
        mRenderer.render(objects, mLayerStack);

        mWindow.update();
        
        emscripten_sleep(10); // wait 10ms for browser
    }
}

void Application::terminate()
{
    mWindow.terminate();
    mRenderer.terminate();
}

void Application::pushLayer(Layer* layer)
{
    mLayerStack.pushLayer(layer);
    layer->onAttach();
}

void Application::pushOverlay(Layer* overlay)
{
    mLayerStack.pushOverlay(overlay);
    overlay->onAttach();
}