#include "TestEzGLBufferBlock.h"
#include "glContext.h"
#include <ezlibs/ezGL/ezGL.hpp>
#include <ezlibs/ezCTest.hpp>

bool TestEzGL_BufferBlock_Create() {
    CTEST_ASSERT(GLContext::initGLContext());
    auto buffer = ez::gl::BufferBlock::create("TestBuffer", GL_UNIFORM_BUFFER);
    CTEST_ASSERT(buffer != nullptr);
    CTEST_ASSERT(buffer->getName() == "TestBuffer");
    CTEST_ASSERT(buffer->id() != 0);
    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_BufferBlock_Upload() {
    CTEST_ASSERT(GLContext::initGLContext());
    auto buffer = ez::gl::BufferBlock::create("TestBuffer", GL_UNIFORM_BUFFER);
    CTEST_ASSERT(buffer != nullptr);

    struct TestData {
        float values[4];
    };
    TestData data = {{1.0f, 2.0f, 3.0f, 4.0f}};

    buffer->upload(GL_STATIC_DRAW, &data, sizeof(TestData));
    CTEST_ASSERT(buffer->id() != 0);
    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_BufferBlock_Bind() {
    CTEST_ASSERT(GLContext::initGLContext());
    auto buffer = ez::gl::BufferBlock::create("TestBuffer", GL_UNIFORM_BUFFER);
    CTEST_ASSERT(buffer != nullptr);

    buffer->bind(0);
    CTEST_ASSERT(buffer->id() != 0);
    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_BufferBlockAuto_Create() {
    CTEST_ASSERT(GLContext::initGLContext());

    struct TestData {
        float x, y, z;
    };

    auto bufferAuto = ez::gl::BufferBlockAuto<GL_SHADER_STORAGE_BUFFER, TestData>::create("TestSSBO");
    CTEST_ASSERT(bufferAuto != nullptr);
    CTEST_ASSERT(bufferAuto->getId() != 0);
    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_BufferBlockAuto_PushData() {
    CTEST_ASSERT(GLContext::initGLContext());

    struct TestData {
        float x, y, z;
    };

    auto bufferAuto = ez::gl::BufferBlockAuto<GL_SHADER_STORAGE_BUFFER, TestData>::create("TestSSBO");
    CTEST_ASSERT(bufferAuto != nullptr);

    bufferAuto->getDatasRef().push_back({1.0f, 2.0f, 3.0f});
    bufferAuto->getDatasRef().push_back({4.0f, 5.0f, 6.0f});

    CTEST_ASSERT(bufferAuto->getDatas().size() == 2);
    bufferAuto->upload(GL_STATIC_DRAW);
    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_BufferBlock_SSBO_Create() {
    CTEST_ASSERT(GLContext::initGLContext());

    struct TestData {
        float values[4];
    };

    auto ssbo = ez::gl::SSBO<TestData>::create("TestSSBO");
    CTEST_ASSERT(ssbo != nullptr);
    CTEST_ASSERT(ssbo->getId() != 0);
    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_BufferBlock_UBOAuto_Create() {
    CTEST_ASSERT(GLContext::initGLContext());

    ez::gl::UBOAuto ubo("TestUBO");
    CTEST_ASSERT(ubo.id() != 0);
    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_BufferBlock_UBOAuto_RegisterVar_Float() {
    CTEST_ASSERT(GLContext::initGLContext());

    ez::gl::UBOAuto ubo("TestUBO");
    ubo.registerVar("myFloat", 42.0f);

    float value = 0.0f;
    CTEST_ASSERT(ubo.getVar("myFloat", value));
    CTEST_ASSERT(value == 42.0f);
    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_BufferBlock_UBOAuto_RegisterVar_Int() {
    CTEST_ASSERT(GLContext::initGLContext());

    ez::gl::UBOAuto ubo("TestUBO");
    ubo.registerVar("myInt", 123);

    int value = 0;
    CTEST_ASSERT(ubo.getVar("myInt", value));
    CTEST_ASSERT(value == 123);
    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_BufferBlock_UBOAuto_SetVar() {
    CTEST_ASSERT(GLContext::initGLContext());

    ez::gl::UBOAuto ubo("TestUBO");
    ubo.registerVar("myFloat", 42.0f);

    CTEST_ASSERT(ubo.setVar("myFloat", 100.0f));

    float value = 0.0f;
    CTEST_ASSERT(ubo.getVar("myFloat", value));
    CTEST_ASSERT(value == 100.0f);
    CTEST_ASSERT(ubo.isDirty());
    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_BufferBlock_UBOAuto_SetAddVar() {
    CTEST_ASSERT(GLContext::initGLContext());

    ez::gl::UBOAuto ubo("TestUBO");
    ubo.registerVar("myInt", 10);

    CTEST_ASSERT(ubo.setAddVar("myInt", 5));

    int value = 0;
    CTEST_ASSERT(ubo.getVar("myInt", value));
    CTEST_ASSERT(value == 15);
    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_BufferBlock_UBOAuto_UploadIfDirty() {
    CTEST_ASSERT(GLContext::initGLContext());

    ez::gl::UBOAuto ubo("TestUBO");
    ubo.registerVar("myFloat", 42.0f);
    ubo.setVar("myFloat", 100.0f);

    CTEST_ASSERT(ubo.isDirty());
    ubo.uploadIfDirty();
    CTEST_ASSERT(!ubo.isDirty());
    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_BufferBlock_UBOAuto_RegisterArray() {
    CTEST_ASSERT(GLContext::initGLContext());

    ez::gl::UBOAuto ubo("TestUBO");
    float values[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    ubo.registerVar("myArray", values, sizeof(values));
    ubo.uploadIfDirty();

    float readValues[4] = {0};
    CTEST_ASSERT(ubo.getVar("myArray", readValues));
    CTEST_ASSERT(readValues[0] == 1.0f);
    CTEST_ASSERT(readValues[3] == 4.0f);
    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_BufferBlock_UBOAuto_Bind() {
    CTEST_ASSERT(GLContext::initGLContext());

    ez::gl::UBOAuto ubo("TestUBO");
    ubo.registerVar("myFloat", 42.0f);
    ubo.uploadIfDirty();

    ubo.bind(0);
    CTEST_ASSERT(ubo.id() != 0);
    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_BufferBlock_UBOAuto_Clear() {
    CTEST_ASSERT(GLContext::initGLContext());

    ez::gl::UBOAuto ubo("TestUBO");
    ubo.registerVar("myFloat", 42.0f);
    ubo.uploadIfDirty();

    float value = 0.0f;
    CTEST_ASSERT(ubo.getVar("myFloat", value));
    CTEST_ASSERT(value == 42.0f);

    ubo.clear();

    CTEST_ASSERT(!ubo.getVar("myFloat", value));
    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_BufferBlock_UBOAuto_Recreate() {
    CTEST_ASSERT(GLContext::initGLContext());

    ez::gl::UBOAuto ubo("TestUBO");
    ubo.registerVar("myFloat", 42.0f);
    ubo.uploadIfDirty();

    float value = 0.0f;
    CTEST_ASSERT(ubo.getVar("myFloat", value));
    CTEST_ASSERT(ez::isEqual(value, 42.0f));

    CTEST_ASSERT(ubo.recreate());

    CTEST_ASSERT(ubo.getVar("myFloat", value));
    CTEST_ASSERT(ez::isEqual(value, 42.0f));

    GLContext::unitGLContext();
    return true;
}

////////////////////////////////////////////////////////////////////////////
//// ENTRY POINT ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzGL_BufferBlock(const std::string& vTest) {
    IfTestExist(TestEzGL_BufferBlock_Create);
    IfTestExist(TestEzGL_BufferBlock_Upload);
    IfTestExist(TestEzGL_BufferBlock_Bind);
    IfTestExist(TestEzGL_BufferBlockAuto_Create);
    IfTestExist(TestEzGL_BufferBlockAuto_PushData);
    IfTestExist(TestEzGL_BufferBlock_SSBO_Create);
    IfTestExist(TestEzGL_BufferBlock_UBOAuto_Create);
    IfTestExist(TestEzGL_BufferBlock_UBOAuto_RegisterVar_Float);
    IfTestExist(TestEzGL_BufferBlock_UBOAuto_RegisterVar_Int);
    IfTestExist(TestEzGL_BufferBlock_UBOAuto_SetVar);
    IfTestExist(TestEzGL_BufferBlock_UBOAuto_SetAddVar);
    IfTestExist(TestEzGL_BufferBlock_UBOAuto_UploadIfDirty);
    IfTestExist(TestEzGL_BufferBlock_UBOAuto_RegisterArray);
    IfTestExist(TestEzGL_BufferBlock_UBOAuto_Bind);
    IfTestExist(TestEzGL_BufferBlock_UBOAuto_Clear);
    IfTestExist(TestEzGL_BufferBlock_UBOAuto_Recreate);
    return false;
}
