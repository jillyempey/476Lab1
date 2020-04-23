#include <iostream>
#include "GameModel.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Texture.h"
#include "Player.h"

GameModel::GameModel(){
    vector<Dog> dogs;
    dogSpawnIntervalLow = 7.0;
    maxNumDogs = 15;
    dogsCollected = 0;
    dogRad = 2.0;
    gridWidth = 70;
    gridLength = 70;
    
    gridWidthMin = -gridWidth/2 + 1;
    gridWidthMax = gridWidth/2 - 1;
    gridLengthMin = -gridLength/2 + 1;
    gridLengthMax = gridLength/2 -1;
    plane.gridLength = gridLength;
    plane.gridWidth = gridWidth;
    plane.allShapes = allShapesPlane;
    //plane.prog = bf_prog;
}

void GameModel::checkForCollisions(int dogIndex, float radius, vec3 position) {
    
    // check collisions against all the dogs
    for (int i = dogIndex + 1; i < int(dogs.size()); i++) {
        if (distance(position, dogs[i].position) <= radius + dogs[i].modelRadius) {
            if (dogIndex == -1 && !dogs[i].isCollected) {
                collectDog(i);
                dogs[i].isCollected = true;
                
            } else {
                dogs[dogIndex].orientation = dogs[dogIndex].orientation * vec3(-1, 1, -1);
            }
            return;
        }
    }
    

    
    // check collisions against all the walls
    
    if (dogIndex != -1) {
        
        if (dogs[dogIndex].position.x > gridWidthMax) {

            dogs[dogIndex].orientation.x *= -1;
            dogs[dogIndex].position.x = gridWidthMax;
            dogs[dogIndex].move(.0005);

        }
        if (dogs[dogIndex].position.x < gridWidthMin){

            dogs[dogIndex].orientation.x *= -1;
            dogs[dogIndex].position.x = gridWidthMin;
            dogs[dogIndex].move(.0005);

        }
        if (dogs[dogIndex].position.z > gridLengthMax) {

            dogs[dogIndex].orientation.z *= -1;
            dogs[dogIndex].position.z = gridLengthMax;
            dogs[dogIndex].move(.0005);

            // dogs[dogIndex].move(.2);
            // dogs[dogIndex].move(.2);
            // //dogs[dogIndex].speed = 0;
            // cout << dogIndex << " p: " << dogs[dogIndex].position.x  << " " << dogs[dogIndex].position.z << " o: " << dogs[dogIndex].orientation.x  << " " << dogs[dogIndex].orientation.z << endl;
        }
        if (dogs[dogIndex].position.z < gridLengthMin){

            dogs[dogIndex].orientation.z *= -1;
            dogs[dogIndex].position.z = gridLengthMin;
            dogs[dogIndex].move(.0005);

        }
    }
}

void GameModel::collectDog(int dogIndex){
    dogs[dogIndex].speed = 0;
    dogsCollected += 1;
    cout << "Dogs Collected: " << dogsCollected << " / " << dogs.size() << " total dogs" << endl;
    if(dogsCollected == maxNumDogs){
        cout << "Congats, you collected all the dogs!" << endl;
    }
}

void GameModel::generateDogs(int numDogs, vector<shared_ptr<Shape>> allShapes){
    for(int i = 0; i < numDogs; i++){
        Dog newDog;
        newDog.theta = 0;
        newDog.offset = randFloat(0, 10);
        newDog.position = vec3(randFloat(gridWidthMin + 2, gridWidthMax - 2), 0, randFloat(gridLengthMin + 2, gridLengthMax - 2));
        newDog.speed = 500;
        newDog.id = i;
        newDog.pathRadius = randFloat(5, 10);
        newDog.modelRadius = 2;//dogRad * .5;
        newDog.isCollected = false;
        //newDog.prog = prog;
        newDog.material = 5; //floor(rand() % 10);
        newDog.dogMiddle = dogMiddle;
        newDog.allShapes = allShapes;
        newDog.orientation = glm::normalize(vec3(randFloat(-1, 1), 0, randFloat(-1, 1)));
        dogs.push_back(newDog);
    }
    cout << "Dogs Collected: " << dogsCollected << " / " << dogs.size() << " total dogs" << endl;
}

void GameModel::updateDogs(double frametime){
    for(int i = 0; i < int(dogs.size()); i++){
        if(!dogs[i].isCollected)
        {
            checkForCollisions(i, dogs[i].modelRadius, dogs[i].position);
            dogs[i].move(frametime);
        }
    }
}

float GameModel::randFloat(float l, float h)
{
    float r = rand() / (float) RAND_MAX;
    return (1.0f - r) * l + r * h;
}
