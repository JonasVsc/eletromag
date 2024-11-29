#include <GLFW/glfw3.h>

const uint32_t kWidth = 512;
const uint32_t kHeight = 512;

void start()
{
    if(!glfwInit())
        return;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(kWidth, kHeight, "WebGPU window", nullptr ,nullptr);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        // TODO: Render a triangle using WebGPU
    }
}


int main()
{
    start();
    return 0;
}