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
        Object obj1("Sphere + Vector", "C:/Dev/eletromag/application/resources/sphere_with_vector.obj");
        Object obj2("Vector", "C:/Dev/eletromag/application/resources/arrow.obj");
        Object obj3("Electron", "C:/Dev/eletromag/application/resources/electron.obj");
        Object obj4("Field", "C:/Dev/eletromag/application/resources/field.obj");

        obj1.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        obj1.setColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

        obj2.setPosition(glm::vec3(15.0f, 0.0f, -15.0f));
        obj2.setColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

        obj3.setPosition(glm::vec3(30.0f, 0.0f, -30.0f));
        obj3.setColor(glm::vec4(0.2f, 0.6f, 1.0f, 1.0f));

        obj4.setPosition(glm::vec3(-45.0f, 0.0f, -45.0f));
        obj4.setColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

        app.mCurrentScene->addObject(obj1);
        app.mCurrentScene->addObject(obj2);
        app.mCurrentScene->addObject(obj3);
        app.mCurrentScene->addObject(obj4);
      

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