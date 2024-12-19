#pragma once

#include "scene.h"
#include "topico3-physics.h"
#include "topico3-gui.h"

#include "transform.h"
#include "mesh.h"

class SceneTopico3 : public Scene
{

public:
    SceneTopico3() 
        : Scene("Topico 3")
    {
        physicsLayer = new Topico3PhysicsLayer(*this);
        guiLayer = new Topico3GUI(*this);

        init();
    }

    void init()
    {
        carga = new Object("carga", "C:/Dev/eletromag/application/resources/electron.obj");
        carga->addComponent<Transform>();
        carga->addComponent<Mesh>();
        carga->getComponent<Mesh>()->initialColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
        
        addObject(carga);
    }

    ~SceneTopico3()
    {

    }
    
public:

    Object* carga;

};


