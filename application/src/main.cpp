#include "application.h"

#include <emscripten.h>
#include <exception>
#include <iostream>

#include "imgui-layer.h"

#include "definitions.h"
#include "transform.h"
#include "mesh.h"

#include "topico1-scene.h"


int main() 
{
    try 
    {
        Application app;
        app.init();


        SceneTopico1 topico1;
        app.setCurrentScene(topico1);

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