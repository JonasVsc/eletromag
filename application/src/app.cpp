#include "app.h"

#include <emscripten.h>

Application::Application()
{
	mWindow = new Window;
	mWindow->create(1280, 720, "Pitanguinha");
	mWindow->init();
}

void Application::run()
{
    while (true)
    {
        mWindow->update();
        emscripten_sleep(100);
    }
}

void Application::terminate()
{
    mWindow->terminate();
}