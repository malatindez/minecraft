#ifndef INPUT_H
#define INPUT_H
#include <glfw3.h>
#include <functional>
#include <thread>
#include <mutex>

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
        KeySequence(const KeySequence& v) : KeySequence(v.size, v.sequence) { }
        KeySequence(const KeySequence* v) : KeySequence(v->size, v->sequence) { }
        ~KeySequence() {
            if (sequence != nullptr) {
                delete[] sequence;
            }
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
    
    template<class T>
    class Input {
    public:
        T* ref;
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
        // func(a, key)
        // a == 0 - pressed for first time
        // a == 1 - currently pressed
        // a == 2 - released
        // key - key sequence that was activated
        // if type == 1 - Input will send only 0
        // if type == 2 - Input will send only 1
        // if type == 4 - input will send only 2
        // 1 | 2 | 4 to send all data
        struct struct4vec {
            KeySequence seq;
            void (T::* func)(int, KeySequence) = nullptr;
            bool pressed = false;
            uint8_t type = 0;
            struct4vec() {}
        };
        std::vector<struct4vec> callbacks;
        std::vector<void(T::*)(double, double)> mouseCallbacks;
        std::vector<void(T::*)(double, double)> scrollCallbacks;
        GLFWwindow* window = nullptr;
        double xpos = 0, ypos = 0;
        double xoffset = 0, yoffset = 0;
        std::mutex mtx;
        std::thread* t = nullptr;
    public:
        Input(GLFWwindow* win) {
            window = win;
            glfwGetCursorPos(window, &xpos, &ypos);
            if (not scroll_callback_set) {
                glfwSetScrollCallback(window, scroll_callback);
                scroll_callback_set = true;
            }
        }
        void addKeyCallback(void (T::* func)(int, KeySequence), KeySequence seq, uint8_t type) {
            struct4vec a;
            a.seq = seq;
            a.func = func;
            callbacks.push_back(a);
        }
        void addMouseCallback(void (T::* func)(double, double)) {
            mouseCallbacks.push_back(func);
        }
        void addScrollCallback(void (T::* func)(double, double)) {
            scrollCallbacks.push_back(func);
        }
    private:
        void processCallback(struct4vec& callback) {
            bool flag = processKeySeq(window, callback.seq);
            if (callback.pressed) {
                if (callback.type & 4 and not flag) {
                    (ref->*callback.func)(2, callback.seq);
                }
                if (callback.type & 2 and flag) {
                    (ref->*callback.func)(1, callback.seq);
                }
                callback.pressed = false;
            }
            else if (callback.type & 1 and flag) {
                (ref->*callback.func)(0, callback.seq);
                callback.pressed = true;
            }
        }
    public:
        void process() {
            mtx.lock();
            for (size_t i = 0; i < callbacks.size(); i++) {
                processCallback(callbacks[i]);
            }
            double previousx = xpos, previousy = ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            if (previousx != xpos or previousy != ypos) {
                for (size_t i = 0; i < mouseCallbacks.size(); i++) {
                    (ref->*mouseCallbacks[i])(xpos, ypos);
                }
            }
            if (xoffset != scrollOffsetx or yoffset != scrollOffsety) {
                for (size_t i = 0; i < scrollCallbacks.size(); i++) {
                    (ref->*scrollCallbacks[i])(scrollOffsetx, scrollOffsety);
                }
                xoffset = scrollOffsetx;
                xoffset = scrollOffsety;
            }
            mtx.unlock();
        }
    };

#endif