#pragma once

#include "scene.h"
#include "topico1-physics.h"

#include "transform.h"
#include "mesh.h"

class SceneTopico1 : public Scene
{

public:
    SceneTopico1() 
        : Scene("Topico 1")
    {
        physicsLayer = new Topico1PhysicsLayer(this);
        
        teste = new Object("Teste", "C:/Dev/eletromag/application/resources/electron.obj");
        teste->addComponent<Transform>();
        addObject(teste);
        
    }

    ~SceneTopico1()
    {

    }
    
public:

    Object* teste;

};


