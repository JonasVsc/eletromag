#pragma once

#include "window.h"
#include "wgpu-renderer.h"

class Application
{
public:

    Application();

    void init();

    void run();

    void terminate();

    inline Window& getWindow() { return mWindow; }

    static inline Application& get() { return *sInstance; }

private:

    Window mWindow;

    Renderer mRenderer;

    static Application* sInstance;
};