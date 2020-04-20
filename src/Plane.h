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
        vector<shared_ptr<Shape>> allShapes;
        float gridWidth, gridLength;
        void draw(shared_ptr<MatrixStack> Model, shared_ptr<Program> prog);
};

#endif