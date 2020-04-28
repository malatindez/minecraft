#ifndef PLAYER_H
#define PLAYER_H
#include "Camera.h"
#include "World.h"
#include <GLFW/glfw3.h>
class Player {
    GLFWwindow* window = nullptr;
    World* world = nullptr;
    Input *input;
    KeySequence *forward, *backward, *left, *right, *up, *down, *breakSeq, *placeSeq;
    double* deltaTime; // pointer to location where is deltatime updating
public:
    Camera cam;
    Player(GLFWwindow* window, World *world, double* deltaTimeLocation, Input* input) {
        this->input = input;
        this->deltaTime = deltaTimeLocation;
        this->window = window;
        this->world = world;
        input->addCheckingKey(GLFW_KEY_W);
        input->addCheckingKey(GLFW_KEY_A);
        input->addCheckingKey(GLFW_KEY_S);
        input->addCheckingKey(GLFW_KEY_D);
        input->addCheckingKey(GLFW_KEY_SPACE);
        input->addCheckingKey(GLFW_KEY_LEFT_CONTROL);
        input->addCheckingKey(GLFW_MOUSE_BUTTON_LEFT);
        input->addCheckingKey(GLFW_MOUSE_BUTTON_RIGHT);
        forward = new KeySequence(GLFW_KEY_W);
        backward = new  KeySequence(GLFW_KEY_S);
        left = new KeySequence(GLFW_KEY_A);
        right = new KeySequence(GLFW_KEY_D);
        up = new KeySequence(GLFW_KEY_SPACE);
        down = new KeySequence(GLFW_KEY_LEFT_CONTROL);
        breakSeq = new KeySequence((uint16_t)GLFW_MOUSE_BUTTON_LEFT);
        placeSeq = new KeySequence(GLFW_MOUSE_BUTTON_RIGHT);
    }
    ~Player() {
        delete forward;
        delete backward;
        delete left;
        delete right;
        delete up;
        delete down;
        delete breakSeq;
        delete placeSeq;
    }
private:
    double lastX = 0, lastY = 0;
    bool firstMouse = true;
    void Breaking() {
        //breaking = true;
    }
    void Placing() {
        
    }
public:
    int32_t hoveredx = 2147483647, hoveredy = 2147483647, hoveredz = 2147483647;
    uint16_t breakingStage;
    void update() {
        if (input->checkSequence((*forward))) {
            cam.ProcessKeyboard(Camera::Movement::FORWARD, (*deltaTime));
        }
        if (input->checkSequence((*backward))) {
            cam.ProcessKeyboard(Camera::Movement::BACKWARD, (*deltaTime));
        }
        if (input->checkSequence((*left))) {
            cam.ProcessKeyboard(Camera::Movement::LEFT, (*deltaTime));
        }
        if (input->checkSequence((*right))) {
            cam.ProcessKeyboard(Camera::Movement::RIGHT, (*deltaTime));
        }
        if (input->checkSequence((*up))) {
            cam.ProcessKeyboard(Camera::Movement::UP, (*deltaTime));
        }
        if (input->checkSequence((*down))) {
            cam.ProcessKeyboard(Camera::Movement::DOWN, (*deltaTime));
        }
        if (input->checkSequence((*breakSeq))) {
            Breaking();
        }
        if (input->checkSequence((*placeSeq))) {
            Placing();
        }
        std::pair<double, double> pos = input->getMousePosition();
        
        if (firstMouse) {
            lastX = pos.first;
            lastY = pos.second;
            firstMouse = false;
        }

        double xoffset = pos.first - lastX;
        double yoffset = lastY - pos.second; // reversed since y-coordinates go from bottom to top

        lastX = pos.first;
        lastY = pos.second;

        cam.ProcessMouseMovement(xoffset, yoffset);

        for (double i = 0; i < 5; i += 0.01) {
            glm::vec3 g = cam.Position + glm::vec3(cam.Front.x * i, cam.Front.y * i, cam.Front.z * i);
            int32_t x = (int32_t)std::round(g.x);
            int32_t y = (int32_t)std::round(g.y);
            int32_t z = (int32_t)std::round(g.z);
        }
    }
};

#endif