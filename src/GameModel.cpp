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
    maxNumDogs = 4;
    dogsCollected = 0;
    
    gridWidth = 50;
    gridLength = 50;
    
    gridWidthMin = -gridWidth/2;
    gridWidthMax = gridWidth/2;
    gridLengthMin = -gridLength/2;
    gridLengthMax = gridLength/2;
    
}

void GameModel::checkForCollisions(int dogIndex, float radius, vec3 position) {
    
    // check collisions against all the dogs
    for (int i = dogIndex + 1; i < dogs.size(); i++) {
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
        
        if (dogs[dogIndex].position.x >= gridWidthMax - radius || dogs[dogIndex].position.x <= gridWidthMin + radius) {
            dogs[dogIndex].orientation.x *= -1;
        }
        if (dogs[dogIndex].position.z >= gridLengthMax - radius || dogs[dogIndex].position.z <= gridLengthMin + radius) {
            dogs[dogIndex].orientation.z *= -1;
        }
    }
}

void GameModel::collectDog(int dogIndex){
    dogs[dogIndex].speed = 0;
    dogsCollected += 1;
    cout << "Dogs Collected: " << dogsCollected << " / " << dogs.size() << " total dogs" << endl;
}

void GameModel::generateDogs(int numDogs){
    for(int i = 0; i < numDogs; i++){
        Dog newDog;
        newDog.theta = 0;
        newDog.offset = randFloat(0, 10);
        newDog.position = vec3(randFloat(gridWidthMin + 2, gridWidthMax - 2), 0, randFloat(gridLengthMin + 2, gridLengthMax - 2));
        newDog.speed = .3;
        newDog.id = i;
        newDog.pathRadius = randFloat(5, 10);
        newDog.modelRadius = 2;
        newDog.isCollected = false;
        newDog.orientation = glm::normalize(vec3(randFloat(-1, 1), 0, randFloat(-1, 1)));
        dogs.push_back(newDog);
    }
    cout << "Dogs Collected: " << dogsCollected << " / " << dogs.size() << " total dogs" << endl;
}

void GameModel::updateDogs(){
    for(int i = 0; i < dogs.size(); i++){
        if(!dogs[i].isCollected)
        {
            checkForCollisions(i, dogs[i].modelRadius, dogs[i].position);
            dogs[i].move();
        }
    }
}

float GameModel::randFloat(float l, float h)
{
    float r = rand() / (float) RAND_MAX;
    return (1.0f - r) * l + r * h;
}
