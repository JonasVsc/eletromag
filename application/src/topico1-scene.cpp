#include "topico1-scene.h"

#include "scene.h"

#include "transform.h"

SceneTopico1::SceneTopico1() 
: Scene("SceneTopico1")
{
    physicsLayer = new Topico1PhysicsLayer(this);
    Object teste("Teste", "C:\\Dev\\eletromag\\application\\resources\\electron.obj");
    // teste->addComponent<Transform>();
    addObject(teste);
}

SceneTopico1::~SceneTopico1()
{

}
