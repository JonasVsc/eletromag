#pragma once

#include "scene.h"
#include "topico1-physics.h"
#include "topico1-gui.h"

#include "transform.h"
#include "mesh.h"

class SceneTopico1 : public Scene
{

public:
    SceneTopico1() 
        : Scene("Topico 1")
    {
        physicsLayer = new Topico1PhysicsLayer(*this);
        guiLayer = new Topico1GUI(*this);
        
        init();
    }

    void init()
    {
        cargaEletrica = new Object("Carga", "C:/Dev/eletromag/application/resources/old_sphere_with_vector.obj");
        cargaEletrica->addComponent<Transform>();
        cargaEletrica->getComponent<Transform>()->initialRotation = glm::vec3(0.0f, 0.0f, 0.0f);
        cargaEletrica->addComponent<Mesh>();
        cargaEletrica->getComponent<Mesh>()->initialColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

        campoMagnetico = new Object("Campo Magnético", "C:/Dev/eletromag/application/resources/old_field.obj");
        campoMagnetico->addComponent<Transform>();
        campoMagnetico->addComponent<Mesh>();
        campoMagnetico->getComponent<Mesh>()->initialColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

        
        forcaMagnetica = new Object("Força Magnética", "C:/Dev/eletromag/application/resources/old_arrow.obj");
        forcaMagnetica->addComponent<Transform>();
        forcaMagnetica->getComponent<Transform>()->initialRotation = glm::vec3(0.0f, 0.0f, 90.0f);
        forcaMagnetica->addComponent<Mesh>();
        forcaMagnetica->getComponent<Mesh>()->initialColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

        
        addObject(cargaEletrica);
        addObject(campoMagnetico);
        addObject(forcaMagnetica);
    }

    ~SceneTopico1()
    {

    }
    
public:

    Object* cargaEletrica;
    Object* campoMagnetico;
    Object* forcaMagnetica;

    float intensidadeCarga = 0.0f;

    glm::vec3 campoMagneticoDirection = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 forcaMagneticaDirection = glm::vec3(0.0f, 1.0f, 0.0f);

};


