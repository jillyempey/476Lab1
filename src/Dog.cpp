#include <iostream>
#include "Dog.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Texture.h"


void Dog::move(double frametime) {
    position += (float(frametime) * speed) * glm::normalize(orientation);
}
float get_rotation(vec3 a, vec3 b){
        float dotProd = dot(a, b);
        float magnitudeA = length(a);
        float magnitudeB = length(b);
        return acos(dotProd / (magnitudeA * magnitudeB));
    }
void Dog::draw(shared_ptr<MatrixStack> Model, shared_ptr<Program> prog) {

        float dogx = position.x;
        float dogz = position.z;
        float sTheta = sin(glfwGetTime());
        float theta = isCollected ? 0 : sTheta;
        SetMaterial(10, prog);
        Model->pushMatrix();

        Model->translate(vec3(dogx, 0, dogz));
        
        float rotation = get_rotation(vec3(0, 0, 1), orientation) ;

        if(orientation.x < 0){
            rotation *= -1;
        }

        Model->rotate(rotation, vec3(0, 1, 0));
        Model->scale(vec3(.5, .5, .5));
        Model->translate(-dogMiddle);
        setModel(prog, Model);
        // head
        allShapes[0]->draw(prog);
        //Model->pushMatrix();
        Model->pushMatrix();
        Model->translate(vec3(0, -.9, -1.5));
        Model->rotate(-.7*theta, vec3(1, 0, 0));
        Model->translate(vec3(0, 1.1, 1.5));
        Model->pushMatrix();
        setModel(prog, Model);
        // back right leg bottom
        allShapes[2]->draw(prog);
        Model->popMatrix();
        setModel(prog, Model);
        //back right leg top
        allShapes[1]->draw(prog);
        Model->popMatrix();
        Model->pushMatrix();
        Model->translate(vec3(0, -.9, 1.5));
        Model->rotate(.7*theta, vec3(1, 0, 0));
        Model->translate(vec3(0, 1.1, -1.5));
        Model->pushMatrix();
        setModel(prog, Model);
        // front left leg bottom
        allShapes[4]->draw(prog);
        Model->popMatrix();
        setModel(prog, Model);
        // front left leg top
        allShapes[5]->draw(prog);
        Model->popMatrix();
        Model->pushMatrix();
        Model->translate(vec3(0, -.9, -1.5));
        Model->rotate(.7*theta, vec3(1, 0, 0));
        Model->translate(vec3(0, 1.1, +1.5));
        Model->pushMatrix();
        setModel(prog, Model);
        // back left leg bottom
        allShapes[6]->draw(prog);
        Model->popMatrix();
        setModel(prog, Model);
        // back left leg top
        allShapes[7]->draw(prog);
        Model->popMatrix();
        Model->pushMatrix();
        Model->translate(vec3(0, -.9, 1.5));
        Model->rotate(-.7*theta, vec3(1, 0, 0));
        Model->translate(vec3(0, 1.1, -1.5));
        Model->pushMatrix();
        setModel(prog, Model);
        // front right leg bottom
        allShapes[8]->draw(prog);
        Model->popMatrix();
        setModel(prog, Model);
        // front right leg top
        allShapes[9]->draw(prog);
        Model->popMatrix();
        setModel(prog, Model);
        // bottom
        allShapes[3]->draw(prog);
        Model->popMatrix();
    }