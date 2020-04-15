#include <iostream>
#include "Player.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Texture.h"

Player::Player(){
    phi = -.3;
    theta = -1.58;
    xTrans = 4;
    //float dogTheta = 0;
    eye = glm::vec3(0, 15, 0);
    lookAtPoint = glm::vec3(0, 0, 1);
    upVector = glm::vec3(0, 1, 0);

    gaze = lookAtPoint - eye;
    w = -(glm::normalize(gaze));
    u = glm::normalize(glm::cross(upVector, w));
    v = glm::normalize(glm::cross(w, u));
}
void Player::move() {
    return;
}


