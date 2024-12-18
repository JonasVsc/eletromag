#include "application.h"

#include <emscripten.h>
#include <exception>
#include <iostream>
#include "imgui-layer.h"

#include "definitions.h"


int main() 
{
    try 
    {
        Application app;
        app.init();

        Scene topico6("Topico 6");
        Object* sphere = new Object("sphere", "C:/Dev/eletromag/application/resources/electron.obj");
        topico6.addObject(sphere);
        app.insertScene(topico6);
        app.setCurrentScene(topico6);

      

        app.pushLayer(new ImGuiLayer());

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