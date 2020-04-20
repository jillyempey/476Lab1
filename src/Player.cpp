#include <iostream>
#include "Player.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Texture.h"

Player::Player(){
    phi = 0;//-.3;
    theta = -1.58;
    eye = glm::vec3(0, 1.5, 0);
    lookAtPoint = glm::vec3(0, 1.5, 1);
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
    lookAtPoint.y = 1.5;
    gaze = lookAtPoint - eye;
    return;
}
void Player::scrollCallback(double deltaX, double deltaY){
    phi = 0;// += .05*deltaY;
    theta -= .05*deltaX;
    
    if (phi > 1.39626) {
        phi = 1.39626;
    } else if (phi < -1.39626) {
        phi = -1.39626;
    }
    
    lookAtPoint.x = cos(phi)*cos(theta) + eye.x;
    lookAtPoint.y = sin(phi) + eye.y;
    lookAtPoint.z = cos(phi) * cos(1.57079632679 - theta) + eye.z;
}
void Player::moveBackwards(){
    eye += speed * w;
    lookAtPoint += speed * w;
    lookAtPoint.y = 1.5;
    gaze = lookAtPoint - eye;
    return;
}

void Player::updateLocalOrientation(){
    w = -(glm::normalize(gaze));
    u = glm::normalize(glm::cross(upVector, w));
    v = glm::normalize(glm::cross(w, u));
}

void Player::moveLeft() {
    eye -= speed * u;
    lookAtPoint -= speed * u;
    lookAtPoint.y = 1.5;
    gaze = lookAtPoint - eye;
    return;
}

void Player::moveRight() {
    eye += speed * u;
    lookAtPoint += speed * u;
    lookAtPoint.y = 1.5;
    gaze = lookAtPoint - eye;
    return;
}




