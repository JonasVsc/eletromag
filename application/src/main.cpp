#include "application.h"

#include <exception>

int main(int argc, char** argv) 
{
    Application app;
    
    app.initialize();

    auto callback = [](void* arg) 
    {
        Application* pApp = reinterpret_cast<Application*>(arg);
        pApp->mainLoop(); 
    };

    emscripten_set_main_loop_arg(callback, &app, 0, true);
        
    app.terminate();
    return 0;
}

