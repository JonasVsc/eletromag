#pragma once

#include "scene.h"
#include "topico2-physics.h"

#include "transform.h"
#include "mesh.h"

class SceneTopico2 : public Scene
{

public:
    SceneTopico2() 
        : Scene("Topico 2")
    {
        physicsLayer = new Topico2PhysicsLayer(this);
        
        teste = new Object("Teste 2", "C:/Dev/eletromag/application/resources/electron.obj");
        teste->addComponent<Transform>();
        teste->addComponent<Mesh>();
        addObject(teste);
        
    }

    ~SceneTopico2()
    {

    }
    
public:

    Object* teste;

};


