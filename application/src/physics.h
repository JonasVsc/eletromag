#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Object;

class Physics
{
public:

    static void applyVelocity(Object* obj, glm::vec3 direction, float speed);
    static void applyVelocityByCurrentDirection(Object* obj, float speed); // for objects which act like vectors

    static void crossVectorialProduct(float(&res)[3], const float(&vec1)[3], const float(&vec2)[3]);
    static void rotateToTarget(Object* obj, glm::vec3 target);

    static float calcularForcaMagnetica(float carga, float velocidade, float campoMagnetico, float anguloGraus);



private:

    Physics();

};