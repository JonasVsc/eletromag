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
        physicsLayer = new Topico1PhysicsLayer(*this);
        
        init();
    }

    void init()
    {

        eletron = new Object("Carga", "C:/Dev/eletromag/application/resources/electron.obj");
        eletron->addComponent<Transform>();
        eletron->addComponent<Mesh>();
        eletron->getComponent<Mesh>()->initialColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

        campoMagnetico = new Object("Campo Magnético", "C:/Dev/eletromag/application/resources/field.obj");
        campoMagnetico->addComponent<Transform>();
        campoMagnetico->addComponent<Mesh>();
        campoMagnetico->getComponent<Mesh>()->initialColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

        
        forcaMagnetica = new Object("Força Magnética", "C:/Dev/eletromag/application/resources/arrow.obj");
        forcaMagnetica->addComponent<Transform>();
        forcaMagnetica->addComponent<Mesh>();
        forcaMagnetica->getComponent<Mesh>()->initialColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

        
        addObject(eletron);
        addObject(campoMagnetico);
        addObject(forcaMagnetica);
    }

    ~SceneTopico1()
    {

    }
    
public:

    Object* eletron;
    Object* campoMagnetico;
    Object* forcaMagnetica;

};


