#include "glContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ezlibs/ezCTest.hpp>

GLFWwindow* GLContext::mp_window{nullptr};

bool GLContext::initGLContext(int32_t aMajor, int32_t aMinor) {
    CTEST_ASSERT(mp_window == nullptr);
    CTEST_ASSERT(glfwInit() != 0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, aMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, aMinor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    mp_window = glfwCreateWindow(10, 10, "test", nullptr, nullptr);
    if (mp_window == nullptr) {
        unitGLContext();
        return false;
    }
    glfwMakeContextCurrent(mp_window);
    return (gladLoadGL() != 0);
}

void GLContext::unitGLContext() {
    if (mp_window != nullptr) {
        glfwDestroyWindow(mp_window);
        mp_window = nullptr;
    }
    // glfwTerminate(); // very slow with llvmpipe, so we comment it
}