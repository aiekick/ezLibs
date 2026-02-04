#include "TestEzGLShader.h"
#include "glContext.h"
#include <ezlibs/ezGL/ezGL.hpp>
#include <ezlibs/ezCTest.hpp>

bool TestEzGL_Shader_CreateFromCode_Vertex() {
    CTEST_ASSERT(GLContext::initGLContext());

    const char* vertCode = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos, 1.0);
}
)";

    auto shader = ez::gl::Shader::createFromCode("TestVert", GL_VERTEX_SHADER, vertCode);
    CTEST_ASSERT(shader != nullptr);
    CTEST_ASSERT(shader->getShaderId() != 0);
    CTEST_ASSERT(shader->getName() == "TestVert");

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Shader_CreateFromCode_Fragment() {
    CTEST_ASSERT(GLContext::initGLContext());

    const char* fragCode = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
)";

    auto shader = ez::gl::Shader::createFromCode("TestFrag", GL_FRAGMENT_SHADER, fragCode);
    CTEST_ASSERT(shader != nullptr);
    CTEST_ASSERT(shader->getShaderId() != 0);
    CTEST_ASSERT(shader->getName() == "TestFrag");

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Shader_CreateFromCode_Compute() {
    CTEST_ASSERT(GLContext::initGLContext());

    const char* compCode = R"(
#version 430
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
}
)";

    auto shader = ez::gl::Shader::createFromCode("TestComp", GL_COMPUTE_SHADER, compCode);
    CTEST_ASSERT(shader != nullptr);
    CTEST_ASSERT(shader->getShaderId() != 0);
    CTEST_ASSERT(shader->getName() == "TestComp");

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Shader_CreateFromCode_Invalid() {
    CTEST_ASSERT(GLContext::initGLContext());

    const char* invalidCode = R"(
#version 330 core
invalid syntax here!!!
)";

    auto shader = ez::gl::Shader::createFromCode("TestInvalid", GL_VERTEX_SHADER, invalidCode);
    CTEST_ASSERT(shader == nullptr);

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Shader_CreateFromFile() {
    CTEST_ASSERT(GLContext::initGLContext());

    auto shader = ez::gl::Shader::createFromFile("TestFromFile", GL_VERTEX_SHADER, INPUTS_DIR "/quadVfx.vert");
    CTEST_ASSERT(shader != nullptr);
    CTEST_ASSERT(shader->getShaderId() != 0);

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Shader_CreateFromFile_NonExistent() {
    CTEST_ASSERT(GLContext::initGLContext());

    auto shader = ez::gl::Shader::createFromFile("TestNonExistent", GL_VERTEX_SHADER, "/nonexistent/file.vert");
    CTEST_ASSERT(shader == nullptr);

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Shader_Unit() {
    CTEST_ASSERT(GLContext::initGLContext());

    const char* vertCode = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos, 1.0);
}
)";

    auto shader = ez::gl::Shader::createFromCode("TestUnit", GL_VERTEX_SHADER, vertCode);
    CTEST_ASSERT(shader != nullptr);
    GLuint shaderId = shader->getShaderId();
    CTEST_ASSERT(shaderId != 0);

    shader->unit();
    CTEST_ASSERT(shader->getShaderId() == 0);

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Shader_MultipleShaders() {
    CTEST_ASSERT(GLContext::initGLContext());

    const char* vertCode = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos, 1.0);
}
)";

    const char* fragCode = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
)";

    auto vertShader = ez::gl::Shader::createFromCode("VertShader", GL_VERTEX_SHADER, vertCode);
    auto fragShader = ez::gl::Shader::createFromCode("FragShader", GL_FRAGMENT_SHADER, fragCode);

    CTEST_ASSERT(vertShader != nullptr);
    CTEST_ASSERT(fragShader != nullptr);
    CTEST_ASSERT(vertShader->getShaderId() != fragShader->getShaderId());

    GLContext::unitGLContext();
    return true;
}

////////////////////////////////////////////////////////////////////////////
//// ENTRY POINT ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzGL_Shader(const std::string& vTest) {
    IfTestExist(TestEzGL_Shader_CreateFromCode_Vertex);
    IfTestExist(TestEzGL_Shader_CreateFromCode_Fragment);
    IfTestExist(TestEzGL_Shader_CreateFromCode_Compute);
    IfTestExist(TestEzGL_Shader_CreateFromCode_Invalid);
    IfTestExist(TestEzGL_Shader_CreateFromFile);
    IfTestExist(TestEzGL_Shader_CreateFromFile_NonExistent);
    IfTestExist(TestEzGL_Shader_Unit);
    IfTestExist(TestEzGL_Shader_MultipleShaders);
    return false;
}
