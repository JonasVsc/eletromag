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

        Scene scene1("Tópico 1");
        Object obj1("Sphere + Vector", "C:/Dev/eletromag/application/resources/sphere_with_vector.obj");
        Object obj2("Vector", "C:/Dev/eletromag/application/resources/arrow.obj");
        Object obj3("Electron", "C:/Dev/eletromag/application/resources/electron.obj");
        Object obj4("Field", "C:/Dev/eletromag/application/resources/field.obj");

        obj1.setPosition(0.0f, 0.0f, 0.0f);
        obj1.setColor(0.0f, 0.0f, 1.0f, 1.0f);
        obj2.setPosition(15.0f, 0.0f, -15.0f);
        obj2.setColor(0.0f, 1.0f, 0.0f, 1.0f);
        obj3.setPosition(30.0f, 0.0f, -30.0f);
        obj3.setColor(0.2f, 0.6f, 1.0f, 1.0f);
        obj4.setPosition(-45.0f, 0.0f, -45.0f);
        obj4.setColor(1.0f, 0.0f, 0.0f, 1.0f);

        scene1.addObject(obj1);
        scene1.addObject(obj2);
        scene1.addObject(obj3);
        scene1.addObject(obj4);

        app.insertScene(scene1);

        Scene scene2("Tópico 2");

        obj1.setPosition(0.0f, 0.0f, 0.0f);
        obj1.setColor(0.0f, 0.0f, 0.0f, 1.0f);
        obj2.setPosition(15.0f, 0.0f, -5.0f);
        obj2.setColor(0.0f, 1.0f, 0.0f, 1.0f);
        obj3.setPosition(12.0f, 0.0f, -3.0f);
        obj3.setColor(0.2f, 0.1f, 1.0f, 1.0f);
        obj4.setPosition(-4.0f, 2.0f, -4.0f);
        obj4.setColor(1.0f, 0.0f, 1.0f, 1.0f);

        scene2.addObject(obj1);
        scene2.addObject(obj2);
        scene2.addObject(obj3);
        scene2.addObject(obj4);
        
        app.insertScene(scene2);
      

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