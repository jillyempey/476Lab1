#include <iostream>
#include "Dog.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Texture.h"


void Dog::move() {
    position += speed * orientation;
    
}
