#pragma once

#ifndef DRAWABLE_H_INCLUDED
#define DRAWABLE_H_INCLUDED

#include <vector>
#include <memory>

#include <glad/glad.h>


#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shape.h"
#include "MatrixStack.h"
using namespace std;

using namespace glm;

class Drawable;

class Drawable {
    public:
        void draw(shared_ptr<MatrixStack> Model);
        void SetMaterial(int i, shared_ptr<Program> prog);
        void setModel(shared_ptr<Program> prog, shared_ptr<MatrixStack>M);
        vector<shared_ptr<Shape>> allShapes;
};

#endif