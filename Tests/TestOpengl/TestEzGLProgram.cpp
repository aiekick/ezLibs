#include "TestEzGLProgram.h"
#include "glContext.h"
#include <ezlibs/ezGL/ezGL.hpp>
#include <ezlibs/ezCTest.hpp>

bool TestEzGL_Program_Create() {
    CTEST_ASSERT(GLContext::initGLContext());

    auto program = ez::gl::Program::create("TestProgram");
    CTEST_ASSERT(program != nullptr);
    CTEST_ASSERT(program->getLabelName() != nullptr);

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Program_AddShader() {
    CTEST_ASSERT(GLContext::initGLContext());

    const char* vertCode = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos, 1.0);
}
)";

    auto program = ez::gl::Program::create("TestProgram");
    CTEST_ASSERT(program != nullptr);

    auto shader = ez::gl::Shader::createFromCode("TestVert", GL_VERTEX_SHADER, vertCode);
    CTEST_ASSERT(shader != nullptr);

    CTEST_ASSERT(program->addShader(shader));

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Program_Link() {
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

    auto program = ez::gl::Program::create("TestProgram");
    CTEST_ASSERT(program != nullptr);

    auto vertShader = ez::gl::Shader::createFromCode("TestVert", GL_VERTEX_SHADER, vertCode);
    auto fragShader = ez::gl::Shader::createFromCode("TestFrag", GL_FRAGMENT_SHADER, fragCode);

    CTEST_ASSERT(vertShader != nullptr);
    CTEST_ASSERT(fragShader != nullptr);

    CTEST_ASSERT(program->addShader(vertShader));
    CTEST_ASSERT(program->addShader(fragShader));
    CTEST_ASSERT(program->link());

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Program_Link_WithoutShaders() {
    CTEST_ASSERT(GLContext::initGLContext());

    auto program = ez::gl::Program::create("TestProgram");
    CTEST_ASSERT(program != nullptr);
    CTEST_ASSERT(!program->link());

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Program_AddUniformFloat() {
    CTEST_ASSERT(GLContext::initGLContext());

    const char* vertCode = R"(
#version 330 core
uniform float testUniform;
layout(location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos * testUniform, 1.0);
}
)";

    const char* fragCode = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
)";

    auto program = ez::gl::Program::create("TestProgram");
    CTEST_ASSERT(program != nullptr);

    auto vertShader = ez::gl::Shader::createFromCode("TestVert", GL_VERTEX_SHADER, vertCode);
    auto fragShader = ez::gl::Shader::createFromCode("TestFrag", GL_FRAGMENT_SHADER, fragCode);

    CTEST_ASSERT(program->addShader(vertShader));
    CTEST_ASSERT(program->addShader(fragShader));
    CTEST_ASSERT(program->link());

    float uniformValue = 1.0f;
    program->addUniformFloat(GL_VERTEX_SHADER, "testUniform", &uniformValue, 1, 1, false, nullptr);

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Program_AddUniformVec2() {
    CTEST_ASSERT(GLContext::initGLContext());

    const char* vertCode = R"(
#version 330 core
uniform vec2 testVec2;
layout(location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos.x * testVec2.x, aPos.y * testVec2.y, aPos.z, 1.0);
}
)";

    const char* fragCode = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
)";

    auto program = ez::gl::Program::create("TestProgram");
    auto vertShader = ez::gl::Shader::createFromCode("TestVert", GL_VERTEX_SHADER, vertCode);
    auto fragShader = ez::gl::Shader::createFromCode("TestFrag", GL_FRAGMENT_SHADER, fragCode);

    CTEST_ASSERT(program->addShader(vertShader));
    CTEST_ASSERT(program->addShader(fragShader));
    CTEST_ASSERT(program->link());

    float uniformValue[2] = {1.0f, 2.0f};
    program->addUniformFloat(GL_VERTEX_SHADER, "testVec2", uniformValue, 2, 1, false, nullptr);

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Program_AddUniformInt() {
    CTEST_ASSERT(GLContext::initGLContext());

    const char* vertCode = R"(
#version 330 core
uniform int testInt;
layout(location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos * float(testInt), 1.0);
}
)";

    const char* fragCode = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
)";

    auto program = ez::gl::Program::create("TestProgram");
    auto vertShader = ez::gl::Shader::createFromCode("TestVert", GL_VERTEX_SHADER, vertCode);
    auto fragShader = ez::gl::Shader::createFromCode("TestFrag", GL_FRAGMENT_SHADER, fragCode);

    CTEST_ASSERT(program->addShader(vertShader));
    CTEST_ASSERT(program->addShader(fragShader));
    CTEST_ASSERT(program->link());

    int32_t uniformValue = 42;
    program->addUniformInt(GL_VERTEX_SHADER, "testInt", &uniformValue, 1, 1, false, nullptr);

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Program_AddUniformMat4() {
    CTEST_ASSERT(GLContext::initGLContext());

    const char* vertCode = R"(
#version 330 core
uniform mat4 testMat;
layout(location = 0) in vec3 aPos;
void main() {
    gl_Position = testMat * vec4(aPos, 1.0);
}
)";

    const char* fragCode = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
)";

    auto program = ez::gl::Program::create("TestProgram");
    auto vertShader = ez::gl::Shader::createFromCode("TestVert", GL_VERTEX_SHADER, vertCode);
    auto fragShader = ez::gl::Shader::createFromCode("TestFrag", GL_FRAGMENT_SHADER, fragCode);

    CTEST_ASSERT(program->addShader(vertShader));
    CTEST_ASSERT(program->addShader(fragShader));
    CTEST_ASSERT(program->link());

    float matrixData[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    program->addUniformMatrix(GL_VERTEX_SHADER, "testMat", matrixData, 4, 1, false, nullptr);

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Program_AddUniformSampler2D() {
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
uniform sampler2D testSampler;
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
)";

    auto program = ez::gl::Program::create("TestProgram");
    auto vertShader = ez::gl::Shader::createFromCode("TestVert", GL_VERTEX_SHADER, vertCode);
    auto fragShader = ez::gl::Shader::createFromCode("TestFrag", GL_FRAGMENT_SHADER, fragCode);

    CTEST_ASSERT(program->addShader(vertShader));
    CTEST_ASSERT(program->addShader(fragShader));
    CTEST_ASSERT(program->link());

    uint32_t textureId = 0;
    program->addUniformSampler2D(GL_FRAGMENT_SHADER, "testSampler", &textureId, false);

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Program_Use() {
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

    auto program = ez::gl::Program::create("TestProgram");
    auto vertShader = ez::gl::Shader::createFromCode("TestVert", GL_VERTEX_SHADER, vertCode);
    auto fragShader = ez::gl::Shader::createFromCode("TestFrag", GL_FRAGMENT_SHADER, fragCode);

    CTEST_ASSERT(program->addShader(vertShader));
    CTEST_ASSERT(program->addShader(fragShader));
    CTEST_ASSERT(program->link());

    program->use();
    program->unuse();

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Program_SetUniformFloatDatas() {
    CTEST_ASSERT(GLContext::initGLContext());

    const char* vertCode = R"(
#version 330 core
uniform float testUniform;
layout(location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos * testUniform, 1.0);
}
)";

    const char* fragCode = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
)";

    auto program = ez::gl::Program::create("TestProgram");
    auto vertShader = ez::gl::Shader::createFromCode("TestVert", GL_VERTEX_SHADER, vertCode);
    auto fragShader = ez::gl::Shader::createFromCode("TestFrag", GL_FRAGMENT_SHADER, fragCode);

    CTEST_ASSERT(program->addShader(vertShader));
    CTEST_ASSERT(program->addShader(fragShader));
    CTEST_ASSERT(program->link());

    float uniformValue = 1.0f;
    program->addUniformFloat(GL_VERTEX_SHADER, "testUniform", &uniformValue, 1, 1, false, nullptr);

    float newValue = 2.0f;
    program->setUniformFloatDatas(GL_VERTEX_SHADER, "testUniform", &newValue);

    GLContext::unitGLContext();
    return true;
}

////////////////////////////////////////////////////////////////////////////
//// ENTRY POINT ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzGL_Program(const std::string& vTest) {
    IfTestExist(TestEzGL_Program_Create);
    IfTestExist(TestEzGL_Program_AddShader);
    IfTestExist(TestEzGL_Program_Link);
    IfTestExist(TestEzGL_Program_Link_WithoutShaders);
    IfTestExist(TestEzGL_Program_AddUniformFloat);
    IfTestExist(TestEzGL_Program_AddUniformVec2);
    IfTestExist(TestEzGL_Program_AddUniformInt);
    IfTestExist(TestEzGL_Program_AddUniformMat4);
    IfTestExist(TestEzGL_Program_AddUniformSampler2D);
    IfTestExist(TestEzGL_Program_Use);
    IfTestExist(TestEzGL_Program_SetUniformFloatDatas);
    return false;
}
