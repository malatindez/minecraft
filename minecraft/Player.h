#ifndef PLAYER_H
#define PLAYER_H
#include "Input.h"
#include "Camera.h"
#include "World.h"
#include <glfw3.h>
class Player {
    GLFWwindow* window = nullptr;
    World* world = nullptr;
    Input<Player> input;
    KeySequence forward, backward, left, right, up, down;
    double* deltaTime; // pointer to location where is deltatime updating
public:
    Camera cam;
    Player(GLFWwindow* window, World *world, double* deltaTimeLocation) : input(window) {
        input.ref = this;
        this->deltaTime = deltaTimeLocation;
        this->window = window;
        this->world = world;
        forward = KeySequence(GLFW_KEY_W);
        backward = KeySequence(GLFW_KEY_S);
        left = KeySequence(GLFW_KEY_A);
        right = KeySequence(GLFW_KEY_D);
        up = KeySequence(GLFW_KEY_SPACE);
        down = KeySequence(GLFW_KEY_LEFT_CONTROL);
        input.addKeyCallback(&Player::processMovement, forward, 7);
        input.addKeyCallback(&Player::processMovement, backward, 7);
        input.addKeyCallback(&Player::processMovement, left, 7);
        input.addKeyCallback(&Player::processMovement, right, 7);
        input.addKeyCallback(&Player::processMovement, up, 7);
        input.addKeyCallback(&Player::processMovement, down, 7);
    }
    void processMovement(int state, KeySequence seq) {
        if (seq == forward) {
            cam.ProcessKeyboard(Camera::Movement::FORWARD, (*deltaTime));
        }
        else if (seq == backward) {
            cam.ProcessKeyboard(Camera::Movement::BACKWARD, (*deltaTime));
        }
        else if (seq == left) {
            cam.ProcessKeyboard(Camera::Movement::LEFT, (*deltaTime));
        }
        else if (seq == right) {
            cam.ProcessKeyboard(Camera::Movement::RIGHT, (*deltaTime));
        }
        else if (seq == up) {
            cam.ProcessKeyboard(Camera::Movement::UP, (*deltaTime));
        }
        else if (seq == down) {
            cam.ProcessKeyboard(Camera::Movement::DOWN, (*deltaTime));
        }
    }
private:
    double lastX = 0, lastY = 0;
    bool firstMouse = true;
public:
    void processMouse(double xpos, double ypos) {
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        double xoffset = xpos - lastX;
        double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        cam.ProcessMouseMovement(xoffset, yoffset);
    }
};

#endif