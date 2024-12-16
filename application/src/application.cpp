#include "application.h"


Application* Application::sInstance = nullptr;

double Application::deltaTime = 0.0f;
double Application::lastFrame = 0.0f;

////////////////////////////////////////////////////////////////////////////
// Public Methods /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

Application::Application()
    : mCurrentScene(new Scene("None"))
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
        calcDeltaTime();

        mRenderer.render(*mCurrentScene, mLayerStack);

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

void Application::selectScene(const std::string sceneName)
{
    auto it = mScenes.find(sceneName);
    if (it != mScenes.end())
    {
        Scene& scene = it->second;
        mCurrentScene = &it->second;
        std::cout << "[INFO] Cena selecionada: " << sceneName << '\n';
    }
    else
    {
        std::cerr << "[ERROR] " << sceneName << " not found" << '\n';
    }
}

void Application::setCurrentScene(Scene& scene) 
{ 
    mCurrentScene = &scene; 
}

void Application::insertScene(const Scene& scene)
{
    mScenes.insert(std::make_pair(scene.mDebugName, scene));
}



////////////////////////////////////////////////////////////////////////////
// Private Methods /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

void Application::calcDeltaTime()
{
    deltaTime = glfwGetTime() - lastFrame;
	lastFrame = glfwGetTime();
}
