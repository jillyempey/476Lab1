#pragma once

#ifndef DOG_H_INCLUDED
#define DOG_H_INCLUDED

#include <vector>
#include <memory>

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

class Dog;

class Dog {
    public:
        vec3 position;
        vec3 orientation;
        float velocity;
        float theta;
        float pathRadius;
        float modelRadius;
        float offset;
        int id;
    private:
        

};

#endif