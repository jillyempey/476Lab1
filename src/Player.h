#pragma once

#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include <vector>
#include <memory>

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

class Player;

class Player {
    public:
        Player();
        int dogsCollected;
        float phi;
        float theta;
        vec3 eye;
        vec3 lookAtPoint;
        vec3 upVector;
    
        float speed;
        
        vec3 gaze;
        vec3 w;
        vec3 u;
        vec3 v;
        void move();
        void moveForward();
        void moveBackwards();
        void moveLeft();
        void moveRight();
        void scrollCallback(double deltaX, double deltaY);
    
    private:
        

};

#endif
