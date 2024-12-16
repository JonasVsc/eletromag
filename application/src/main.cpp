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

        Object eletron("Eletron", "C:/Dev/eletromag/application/resources/sphere_with_vector.obj");
        eletron.setPosition(0.0f, 0.0f, 0.0f);
        eletron.setColor(0.0f, 0.0f, 1.0f, 1.0f);
        
        Object vector("Vector", "C:/Dev/eletromag/application/resources/arrow.obj");
        vector.setPosition(0.0f, 0.0f, 0.0f);
        vector.setColor(0.0f, 1.0f, 0.0f, 1.0f);

        Object field("Magnectic Field", "C:/Dev/eletromag/application/resources/field.obj");
        field.setPosition(0.0f, 0.0f, 0.0f);
        field.setColor(1.0f, 0.0f, 0.0f, 1.0f);

        Scene topico1("Tópico 1");
        topico1.addObject(eletron);
        topico1.addObject(vector);
        topico1.addObject(field);
        app.insertScene(topico1);

        Scene teste1("TESTE 1");
        teste1.addObject(eletron);
        app.insertScene(teste1);

      

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