#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Window
{
public:

    Window* create(unsigned int width, unsigned int height, const char* name);

    ~Window();

    void init();

    void update();

    void terminate();

    inline void* getNativeWindow() const { return mWindow;  }

    struct WindowData
    {
        unsigned int width = 1280, height = 720;
        
        const char* title = "Pitanguinha";
    };

    WindowData mData;

private:

    GLFWwindow* mWindow;



};