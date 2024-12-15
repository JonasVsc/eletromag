#include "application.h"

#include <emscripten.h>
#include <exception>
#include <iostream>
#include "imgui-layer.h"

int main() 
{
    try 
    {
        Application app;
        app.init();

        Scene teste("Cena de teste");

        app.setCurrentScene(teste);
        Object obj1("Objeto de teste 1", "C:/Dev/eletromag/application/resources/sphere_with_vector.obj");
        obj1.setPosition(glm::vec3(0.0f, 50.0f, 0.0f));
        obj1.setColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        app.mCurrentScene->addObject(obj1);
      

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