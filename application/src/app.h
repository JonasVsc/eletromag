#pragma once

#include "window.h"
#include "wgpu_renderer.h"

class Application
{
public:

    void run();

    void terminate();

    static inline Application& get() {  static Application instance; return instance; };


private:

    Application();

    Application(const Application&) = delete;

    Application& operator=(const Application&) = delete;

private:

    Window* mWindow;

    Renderer mRenderer;


};