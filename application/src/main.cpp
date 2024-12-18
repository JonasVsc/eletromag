#include "application.h"

#include <emscripten.h>
#include <exception>
#include <iostream>
#include "imgui-layer.h"

#include "definitions.h"
#include "transform.h"


int main() 
{
    try 
    {
        Application app;
        app.init();

        Scene scene("Topico 6");
        Object* sphere = new Object("sphere", "C:/Dev/eletromag/application/resources/electron.obj");
        sphere->addComponent<Transform>();

        scene.addObject(sphere);
        app.insertScene(scene);
        app.setCurrentScene(scene);

      

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