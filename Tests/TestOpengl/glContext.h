#pragma once

struct GLFWwindow;
class GLContext {
private:
    static GLFWwindow* mp_window;

public:
    static bool initGLContext();
    static void unitGLContext();
};