#include "app.h"

#include <emscripten.h>

 Application* Application::sInstance = nullptr;

Application::Application()
{
    sInstance = this;
	mWindow.create(640, 480, "WebGPU");
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
        mWindow.update();
        emscripten_sleep(100);
    }
}

void Application::terminate()
{
    mWindow.terminate();
    mRenderer.terminate();
}