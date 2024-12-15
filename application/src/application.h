#pragma once

#include "window.h"
#include "wgpu-renderer2.h"
#include "wgpu-pipeline.h"

#include "layer.h"
#include "layer-stack.h"

class Application
{
public:

    Application();

    void init();

    void run();

    void terminate();

    void pushLayer(Layer* layer);

    void pushOverlay(Layer* overlay);

    inline Window& getWindow() { return mWindow; }

    inline Renderer2& getRenderer() { return mRenderer; }

    inline Pipeline& getPipeline() { return mPipeline; }

    static inline Application& get() { return *sInstance; }

private:

    Window mWindow;

    Renderer2 mRenderer;

    Pipeline mPipeline;

    bool running;

    LayerStack mLayerStack;

private:

    static Application* sInstance;
};