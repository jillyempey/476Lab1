#pragma once

#ifndef DOG_H_INCLUDED
#define DOG_H_INCLUDED

#include <vector>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Drawable.h"

using namespace std;
using namespace glm;

class Dog;

class Dog : public Drawable {
    public:
    
        void move();
        void draw(shared_ptr<MatrixStack> Model);
        vec3 position;
        vec3 orientation;
        vec3 lookAt;
        float speed;
        float theta;
        float pathRadius;
        float modelRadius;
        float offset;
        int id;
        bool isCollected;
        vec3 dogMiddle;
        std::shared_ptr<Program> prog;
    
    private:
        

};

#endif
