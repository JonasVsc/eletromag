#include "window.h"

#include<iostream>

Window* Window::create(unsigned int width, unsigned int height, const char* name)
{
	mData.width = width;
	mData.height = height;
	mData.title = name;

	return new Window();
}

Window::~Window()
{
    terminate();
}

void Window::init()
{
    if(!glfwInit())
        throw std::runtime_error("[ERROR] failed to init glfw");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    mWindow = glfwCreateWindow(mData.width, mData.height, mData.title, nullptr, nullptr);

    if(mWindow == nullptr)
        throw std::runtime_error("[ERROR] failed to create window");

    glfwMakeContextCurrent(mWindow);
    glfwSetWindowUserPointer(mWindow, &mData);

    glfwSetErrorCallback([](int error_code, const char* description)
    {
        std::cerr << "[ERROR] (" << error_code << ") " << description << '\n';
    });

    // TODO: glfw callbacks
    glfwSetCursorPosCallback(mWindow, [](GLFWwindow* window, double xpos, double ypos)
    {
        // ("Cursor moved callback!");
    });

    glfwSetKeyCallback(mWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        switch (action)
        {
        case GLFW_PRESS:
            std::cout << "[INFO] " << "Key " << key << " Pressed" << '\n';
            break;
        case GLFW_RELEASE:
            std::cout << "[INFO] " << "Key " << key << " Released" << '\n';
            break;
        }
    });

    glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* window, int button, int action, int mods)
    {
        switch (action)
        {
        case GLFW_PRESS:
            std::cout << "[INFO] " << "Mouse button " << button << " Pressed" << '\n';
            break;
        case GLFW_RELEASE:
            std::cout << "[INFO] " << "Mouse button " << button << " Released" << '\n';
            break;
        }
    });
}

void Window::update()
{
    glfwPollEvents();
}

void Window::terminate()
{
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}