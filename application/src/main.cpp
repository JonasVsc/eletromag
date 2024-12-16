#include "application.h"

#include <emscripten.h>
#include <exception>
#include <iostream>
#include "imgui-layer.h"

#include "definitions.h"

#include "eletron.h"
#include "field.h"

int main() 
{
    try 
    {
        Application app;
        app.init();

        Scene teste1("TESTE 1");

        teste1.addObject(new Charge("Charge", "C:/Dev/eletromag/application/resources/electron.obj"));
        teste1.addObject(new Field("Magnetic Field", "C:/Dev/eletromag/application/resources/field.obj"));
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