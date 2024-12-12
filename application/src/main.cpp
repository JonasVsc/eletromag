#include "application.h"

#include <emscripten.h>
#include <exception>
#include <iostream>

int main() 
{
    try 
    {
        Application app;
        app.init();

        auto callback = [](void* arg) 
        {
            Application* pApp = reinterpret_cast<Application*>(arg);
            pApp->run(); 
        };

        emscripten_set_main_loop_arg(callback, &app, 0, true);

        app.terminate();
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}