#include <iostream>
#include "Plane.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"


// Plane::Plane(){

// }
void Plane::draw(shared_ptr<MatrixStack> Model, shared_ptr<Program> prog){
    SetMaterial(14, prog);
    float wallHeight = 100.0;
    // base
    Model->pushMatrix();
    Model->translate(vec3(0, -1.75, 0));
    Model->scale(vec3(gridWidth, .5, gridLength));
    setModel(prog, Model);
    allShapes[0]->draw(prog);
    Model->popMatrix(); 

    SetMaterial(1, prog);

    // wall L
    Model->pushMatrix();
    Model->translate(vec3(-gridWidth/2, 0, 0));
    Model->scale(vec3(1, wallHeight, gridWidth));
    setModel(prog, Model);
    allShapes[0]->draw(prog);
    Model->popMatrix(); 

    // wall L
    Model->pushMatrix();
    Model->translate(vec3(gridWidth/2, 0, 0));
    Model->scale(vec3(1, wallHeight, gridWidth));
    setModel(prog, Model);
    allShapes[0]->draw(prog);
    Model->popMatrix(); 

    // wall W
    Model->pushMatrix();
    Model->translate(vec3(0, 0, gridWidth/2));
    Model->scale(vec3(gridWidth, wallHeight, 1));
    setModel(prog, Model);
    allShapes[0]->draw(prog);
    Model->popMatrix(); 

    // wall W
    Model->pushMatrix();
    Model->translate(vec3(0, 0, -gridWidth/2));
    Model->scale(vec3(gridWidth, wallHeight, 1));
    setModel(prog, Model);
    allShapes[0]->draw(prog);
    Model->popMatrix(); 
}