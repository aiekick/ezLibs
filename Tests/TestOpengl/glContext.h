#pragma once
#include <cstdint>

struct GLFWwindow;
class GLContext {
private:
    static GLFWwindow* mp_window;

public:
    static bool initGLContext(int32_t aMajor = 3, int32_t aMinor = 3);
    static void unitGLContext();
};