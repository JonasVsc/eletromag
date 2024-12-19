#pragma once

#include "scene.h"
#include "topico2-physics.h"
#include "topico2-gui.h"

#include "transform.h"
#include "mesh.h"

class SceneTopico2 : public Scene
{

public:
    SceneTopico2() 
        : Scene("Topico 2")
    {
        physicsLayer = new Topico2PhysicsLayer(*this);
        guiLayer = new Topico2GUI(*this);

        init();
    }

    void init()
    {
        apontador = new Object("Apontador", "C:/Dev/eletromag/application/resources/sphere_with_vector.obj");
        apontador->addComponent<Transform>();
        apontador->addComponent<Mesh>();
        apontador->getComponent<Mesh>()->initialColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

        qualquer = new Object("Objeto", "C:/Dev/eletromag/application/resources/electron.obj");
        qualquer->addComponent<Transform>();
        qualquer->getComponent<Transform>()->initialPosition = glm::vec3(-5.0f, 6.0f, 2.0f);
        qualquer->addComponent<Mesh>();
        qualquer->getComponent<Mesh>()->initialColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
        
        addObject(apontador);
        addObject(qualquer);
    }

    ~SceneTopico2()
    {

    }
    
public:

    Object* apontador;
    Object* qualquer;

};


