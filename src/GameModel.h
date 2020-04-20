#pragma once

#ifndef GAMEMODEL_H_INCLUDED
#define GAMEMODEL_H_INCLUDED

#include <vector>
#include <memory>

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Dog.h"
#include "Plane.h"

using namespace std;
using namespace glm;

class GameModel;

class GameModel {
    public:
        GameModel();
        vector<Dog> dogs;
        Plane plane;
        shared_ptr<Program> bf_prog;

        vector<shared_ptr<Shape>> allShapesDog;
        vector<shared_ptr<Shape>> allShapesPlane;
        double dogSpawnIntervalLow;
        int maxNumDogs;
        int dogsCollected;
        vec3 dogMiddle;
        
        float gridWidth;
        float gridLength;
        float gridWidthMin;
        float gridWidthMax;
        float gridLengthMin;
        float gridLengthMax;
        
        void checkForCollisions(int dogIndex, float radius, vec3 position);
        void collectDog(int dogIndex);
        void generateDogs(int numDogs, vector<shared_ptr<Shape>> allShapes);
        void updateDogs();
        float randFloat(float l, float r);
    
    private:
       
        

};

#endif
