#pragma once

#include "window.h"
#include "wgpu-renderer2.h"
#include "layer-stack.h"
#include "camera.h"

#include <string>
#include <unordered_map>

class Application
{
public:

    Application();

    void init();

    void run();

    void terminate();

    void pushLayer(Layer* layer);

    void pushOverlay(Layer* overlay);

    void selectScene(const std::string sceneName);

    inline Window& getWindow() { return mWindow; }

    inline Renderer2& getRenderer() { return mRenderer; }

    inline Camera& getMainCamera() { return mMainCamera; }

    void setCurrentScene(Scene& scene);

    void insertScene(const Scene& scene);

    static inline Application& get() { return *sInstance; }

private:

    Window mWindow;

    Renderer2 mRenderer;


    Camera mMainCamera;

public:

    static bool sRunningSimulation;

    static double deltaTime;
    static double lastFrame;

    LayerStack mLayerStack;

    std::unordered_map<std::string, Scene> mScenes;
    Scene* mCurrentScene;

private:

    static void calcDeltaTime();

    static Application* sInstance;
};