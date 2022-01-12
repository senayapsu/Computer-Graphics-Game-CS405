#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "glm/gtc/constants.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/common.hpp"
#include "glm/gtc/type_ptr.hpp"

class Camera
{
public:
    static glm::vec3 position;
    static glm::vec3 rotation; // (x degrees, y degrees, z degrees)
    static glm::vec3 front, up;

    static void moveFront(float);
    static void moveUp(float);
    static void moveRight(float);

    static glm::mat4 getViewMatrix();
    static glm::mat4 getProjMatrix();

    static glm::mat4 getMVPMatrix(glm::mat4 model, glm::mat4 projection) ;
    static glm::vec3 getPosition();
};

glm::vec3 Camera::position = glm::vec3(0, 0, 3);

glm::vec3 Camera::rotation = glm::vec3(0, 0, 0);

glm::vec3 Camera::front = glm::vec3(0, 0, -1);

glm::vec3 Camera::up = glm::vec3(0, 1, 0);

void Camera::moveFront(float num) {
    Camera::position += front * num;
}

void Camera::moveUp(float num) {
    Camera::position += up * num;
}

void Camera::moveRight(float num) {
    Camera::position += glm::cross(front, up) * num;
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(position, position + Camera::front, glm::vec3(0,1,0));
}

glm::mat4 Camera::getProjMatrix()
{
    return glm::perspective(glm::radians(45.f), 1.f, 0.1f, 10000.f);
}

glm::mat4 Camera::getMVPMatrix(glm::mat4 model, glm::mat4 projection) 
{
    return projection * Camera::getViewMatrix() * model;
}

glm::vec3 Camera::getPosition() {
    return Camera::position;
}

#endif