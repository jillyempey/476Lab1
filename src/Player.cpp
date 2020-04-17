#include <iostream>
#include "Player.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Texture.h"

Player::Player(){
    phi = -.3;
    theta = -1.58;
    eye = glm::vec3(0, 15, 0);
    lookAtPoint = glm::vec3(0, 0, 1);
    upVector = glm::vec3(0, 1, 0);
    
    speed = 2;

    gaze = lookAtPoint - eye;
    w = -(glm::normalize(gaze));
    u = glm::normalize(glm::cross(upVector, w));
    v = glm::normalize(glm::cross(w, u));
}
void Player::moveForward() {
    eye -= speed * w;
    lookAtPoint -= speed * w;
    gaze = lookAtPoint - eye;
    return;
}

void Player::moveBackwards(){
    eye += speed * w;
    lookAtPoint += speed * w;
    gaze = lookAtPoint - eye;
    return;
}

void Player::moveLeft() {
    eye -= speed * u;
    lookAtPoint -= speed * u;
    gaze = lookAtPoint - eye;
    return;
}

void Player::moveRight() {
    eye += speed * u;
    lookAtPoint += speed * u;
    gaze = lookAtPoint - eye;
    return;
}



