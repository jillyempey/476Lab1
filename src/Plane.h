#pragma once

#ifndef PLANE_H_INCLUDED
#define PLANE_H_INCLUDED
#include <vector>
#include <memory>

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Drawable.h"



using namespace std;
using namespace glm;


class Plane : public Drawable {
    public:
        shared_ptr<Program> prog;
        vector<shared_ptr<Shape>> allShapes;
        //Plane();
        float gridWidth, gridLength;
        void draw(shared_ptr<MatrixStack> Model);
};

#endif