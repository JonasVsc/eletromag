#include "application.h"

#include <emscripten.h>
#include <exception>
#include <iostream>

#include "imgui-layer.h"

#include "definitions.h"
#include "transform.h"
#include "mesh.h"

#include "topico1-scene.h"
#include "topico2-scene.h"
#include "topico3-scene.h"


int main() 
{
    try 
    {
        Application app;
        app.init();


        SceneTopico1 topico1;
        app.insertScene(topico1);

        SceneTopico2 topico2;
        app.insertScene(topico2);

        SceneTopico3 topico3;
        app.insertScene(topico3);


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