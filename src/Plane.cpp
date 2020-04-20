#include <iostream>
#include "Plane.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"


// Plane::Plane(){

// }
void Plane::draw(shared_ptr<MatrixStack> Model){
    SetMaterial(4, prog);
    Model->pushMatrix();
    Model->translate(vec3(0, -1.75, 0));
    Model->scale(vec3(gridWidth, .5, gridLength));
    setModel(prog, Model);
    allShapes[0]->draw(prog);
    Model->popMatrix(); 
}