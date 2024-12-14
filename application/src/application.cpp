#include "application.h"

#include <emscripten.h>

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
}

void Application::run()
{
    while (true)
    {
        // mRenderer.render();

        mWindow.update();
        
        emscripten_sleep(10);
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