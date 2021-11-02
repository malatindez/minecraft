#ifndef INPUT_H
#define INPUT_H
#include <GLFW/glfw3.h>
#include <functional>
#include <thread>
#include <mutex>
#include <set>
#include <algorithm>
double scrollOffsetx = 0, scrollOffsety = 0;
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    scrollOffsetx = xoffset;
    scrollOffsety = yoffset;
}
bool scroll_callback_set = false;

struct KeySequence {
    uint16_t* sequence = nullptr;
    size_t size = 0;
    KeySequence() {}
    KeySequence(size_t size, uint16_t* seq) {
        if (size > 5 || size == 0) {
            return;
        }
        sequence = new uint16_t[size];
        memcpy(sequence, seq, sizeof(uint16_t) * size);
        this->size = size;
    }
    KeySequence(uint16_t first) {
        size = 1;
        sequence = new uint16_t[1];
        sequence[0] = first;
    }
    KeySequence(uint16_t first, uint16_t second) {
        size = 2;
        sequence = new uint16_t[2];
        sequence[0] = first; sequence[1] = second;
    }
    KeySequence(uint16_t first, uint16_t second, uint16_t third) {
        size = 3;
        sequence = new uint16_t[3];
        sequence[0] = first; sequence[1] = second; sequence[2] = third;
    }
    KeySequence(uint16_t first, uint16_t second, uint16_t third, uint16_t fourth) {
        size = 4;
        sequence = new uint16_t[4];
        sequence[0] = first; sequence[1] = second; sequence[2] = third; sequence[3] = fourth;
    }
    KeySequence(uint16_t first, uint16_t second, uint16_t third, uint16_t fourth, uint16_t fifth) {
        size = 4;
        sequence = new uint16_t[4];
        sequence[0] = first; sequence[1] = second; sequence[2] = third;
        sequence[3] = fourth; sequence[4] = fifth;
    }
    KeySequence(const KeySequence& v) : KeySequence(v.size, v.sequence) { }
    KeySequence(const KeySequence* v) : KeySequence(v->size, v->sequence) { }
    ~KeySequence() {
        if (size == 1) {
            delete sequence;
        }
        else if (size < 5 && sequence != nullptr) {
            delete[] sequence;
        }
        size = -1;
        sequence = nullptr;
    }
    bool operator==(const KeySequence& ks) {
        if (ks.size != size) {
            return false;
        }
        for (size_t i = 0; i < size; i++) {
            if (ks.sequence[i] != sequence[i]) {
                return false;
            }
        }
        return true;
    }
};

class Input {
public:
    static bool processKeySeq(GLFWwindow* win, KeySequence seq) {
        bool flag = true;
        for (size_t i = 0; i < seq.size; i++) {
            if (glfwGetKey(win, seq.sequence[i]) != GLFW_PRESS) {
                flag = false; break;
            }
        }
        return flag;
    }

private:
    GLFWwindow* window = nullptr;
    double xpos = 0, ypos = 0;
    double xoffset = 0, yoffset = 0;
    bool* keys = new bool[65536];
    std::set<uint16_t> checking;
public:
    Input(GLFWwindow* win) {
        memset(keys, 0, sizeof(bool) * 65536);
        window = win;
        glfwGetCursorPos(window, &xpos, &ypos);
        if (!scroll_callback_set) {
            glfwSetScrollCallback(window, scroll_callback);
            scroll_callback_set = true;
        }
    }
    ~Input() {
        delete[] keys;
    }
    void addCheckingKey(uint16_t key) {
        checking.insert(key);
    }
    bool checkSequence(KeySequence seq) {
        for (size_t i = 0; i < seq.size; i++) {
            if (!keys[seq.sequence[i]]) {
                return false;
            }
        }
        return true;
    }
    std::pair<double, double> getScrollOffset() {
        return std::pair<double, double>(xoffset, yoffset);
    }
    std::pair<double, double> getMousePosition() {
        return std::pair<double, double>(xpos, ypos);
    }
public:
    void update() {
        for (std::set<uint16_t>::iterator i = checking.begin(); i != checking.end(); i++) {
            keys[(*i)] = (glfwGetKey(window, (*i)) == GLFW_PRESS);
        }
        glfwGetCursorPos(window, &xpos, &ypos);
        xoffset = scrollOffsetx;
        xoffset = scrollOffsety;
    }
};

#endif