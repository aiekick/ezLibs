#include "TestEzGLUniforms.h"
#include <ezlibs/ezGL/ezGL.hpp>
#include "glContext.h"

////////////////////////////////////////////////////////////////////////////
//// GOOD SYNTAX ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzGL_Uniforms_Parsing_Good_Syntax_0() {
    return ez::gl::UniformParsingDatas("/*ds\\nddfs*/uniform/*ds\\nddfs*/float/*ds\\nddfs*/toto/*dsd\\ndfs*/;/*dsd\\ndfs*/").isValid();
}

bool TestEzGL_Uniforms_Parsing_Good_Syntax_1() {
    return ez::gl::UniformParsingDatas("uniform float toto;").isValid();
}

bool TestEzGL_Uniforms_Parsing_Good_Syntax_2() {
    return ez::gl::UniformParsingDatas("uniform float toto ;").isValid();
}

bool TestEzGL_Uniforms_Parsing_Good_Syntax_3() {
    return ez::gl::UniformParsingDatas("uniform float toto; // toto").isValid();
}

bool TestEzGL_Uniforms_Parsing_Good_Syntax_4() {
    return ez::gl::UniformParsingDatas("uniform float toto; // toto \\n tata;").isValid();
}

bool TestEzGL_Uniforms_Parsing_Good_Syntax_5() {
    return ez::gl::UniformParsingDatas("uniform float toto; /* toto \\n tata; */").isValid();
}

bool TestEzGL_Uniforms_Parsing_Good_Syntax_6() {
    return ez::gl::UniformParsingDatas("/* lkj\\ndfghxkl */ uniform float toto ; /* sdfsd\\nfsdfsdf */").isValid();
}

bool TestEzGL_Uniforms_Parsing_Good_Syntax_7() {
    return ez::gl::UniformParsingDatas("/* lkjdf \\nghxkl */ uniform /* d \\nsdfdf */ float /* qs \\ndsddf */ toto /* lkjdf \\nghxkl */ ; /* lkjdf \\nghxkl */ ")
        .isValid();
}

bool TestEzGL_Uniforms_Parsing_Good_Syntax_8() {
    return ez::gl::UniformParsingDatas(" uniform float toto;").isValid();
}

bool TestEzGL_Uniforms_Parsing_Good_Syntax_9() {
    return ez::gl::UniformParsingDatas(" uniform float(titi) toto;").isValid();
}

bool TestEzGL_Uniforms_Parsing_Good_Syntax_10() {
    auto upd = ez::gl::UniformParsingDatas(" uniform float(titi:one:two:three) toto; /* kjhdfgkshk */");
    if (!upd.isValid()) {
        return false;
    }
    if (upd.uniform_type != "float") {
        return false;
    }
    if (upd.widget_type != "titi") {
        return false;
    }
    if (upd.uniform_name != "toto") {
        return false;
    }
    if (upd.params.size() != 3U) {
        return false;
    }
    if (upd.params.size() > 0 && upd.params[0] != "one") {
        return false;
    }
    if (upd.params.size() > 1 && upd.params[1] != "two") {
        return false;
    }
    if (upd.params.size() > 2 && upd.params[2] != "three") {
        return false;
    }
    if (upd.uniform_comment_original != " kjhdfgkshk ") {
        return false;
    }

    return true;
}

bool TestEzGL_Uniforms_Parsing_Good_Syntax_11() {
    auto upd = ez::gl::UniformParsingDatas(" uniform float(titi) toto; /* kjhdfgkshk */");
    if (!upd.isValid()) {
        return false;
    }
    if (upd.uniform_type != "float") {
        return false;
    }
    if (upd.widget_type != "titi") {
        return false;
    }
    if (upd.uniform_name != "toto") {
        return false;
    }
    if (upd.params.size() != 0U) {
        return false;
    }
    if (upd.uniform_comment_original != " kjhdfgkshk ") {
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////
//// BAD SYNTAX ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzGL_Uniforms_Parsing_Bad_Syntax_0() {
    return !ez::gl::UniformParsingDatas("").isValid();
}

bool TestEzGL_Uniforms_Parsing_Bad_Syntax_1() {
    return !ez::gl::UniformParsingDatas("//uniform float toto;").isValid();
}

bool TestEzGL_Uniforms_Parsing_Bad_Syntax_2() {
    return !ez::gl::UniformParsingDatas("// uniform float toto;").isValid();
}

bool TestEzGL_Uniforms_Parsing_Bad_Syntax_3() {
    return !ez::gl::UniformParsingDatas("uniform /* toto;").isValid();
}

bool TestEzGL_Uniforms_Parsing_Bad_Syntax_4() {
    return !ez::gl::UniformParsingDatas("uniform float ;").isValid();
}

bool TestEzGL_Uniforms_Parsing_Bad_Syntax_5() {
    return !ez::gl::UniformParsingDatas("uniform float toto").isValid();
}

bool TestEzGL_Uniforms_Parsing_Bad_Syntax_6() {
    return !ez::gl::UniformParsingDatas(" float toto;").isValid();
}

bool TestEzGL_Uniforms_Parsing_Bad_Syntax_7() {
    return !ez::gl::UniformParsingDatas("unifrm float toto;").isValid();
}

////////////////////////////////////////////////////////////////////////////
//// TIME WIDGET ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzGL_Uniforms_Parsing_Time_Widget_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform float(time:0.0) uTime;");
    if (!upd.isValid()) {
        return false;
    }
    if (upd.uniform_type != "float") {
        return false;
    }
    if (upd.widget_type != "time") {
        return false;
    }
    if (upd.uniform_name != "uTime") {
        return false;
    }
    if (upd.params.size() != 1U) {
        return false;
    }
    if (upd.params.size() > 0 && upd.params[0] != "0.0") {
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////
//// BUFFER WIDGET /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzGL_Uniforms_Parsing_Buffer_Widget_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform vec2(buffer) uResolution;");
    if (!upd.isValid()) {
        return false;
    }
    if (upd.uniform_type != "vec2") {
        return false;
    }
    if (upd.widget_type != "buffer") {
        return false;
    }
    if (upd.uniform_name != "uResolution") {
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////
//// SLIADER WIDGET ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzGL_Uniforms_Parsing_Slider_Widget_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform float(0.0:1.0:0.5:0.1) uSlider;");
    if (!upd.isValid()) {
        return false;
    }
    if (upd.uniform_type != "float") {
        return false;
    }
    if (upd.widget_type != "slider") {
        return false;
    }
    if (upd.params.size() != 4U) {
        return false;
    }
    if (upd.params.size() > 0 && upd.params[0] != "0.0") {
        return false;
    }
    if (upd.params.size() > 1 && upd.params[1] != "1.0") {
        return false;
    }
    if (upd.params.size() > 2 && upd.params[2] != "0.5") {
        return false;
    }
    if (upd.params.size() > 3 && upd.params[3] != "0.1") {
        return false;
    }
    if (upd.uniform_name != "uSlider") {
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////
//// EXTENDED WIDGET TESTS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// Test that vec2 works with widgets
bool TestEzGL_Uniforms_Widget_Vec2_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform vec2(slider:0.0:1.0:0.5:0.1) uVec2;");
    return upd.isValid() && upd.uniform_type == "vec2" && upd.widget_type == "slider";
}

// Test that vec3 works with widgets
bool TestEzGL_Uniforms_Widget_Vec3_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform vec3(color:1.0:0.5:0.0) uColor;");
    return upd.isValid() && upd.uniform_type == "vec3" && upd.widget_type == "color";
}

// Test that vec4 works with widgets
bool TestEzGL_Uniforms_Widget_Vec4_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform vec4(color) uVec4;");
    return upd.isValid() && upd.uniform_type == "vec4" && upd.widget_type == "color";
}

// Test int with widget
bool TestEzGL_Uniforms_Widget_Int_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform int(slider:0:100:50:1) uInt;");
    return upd.isValid() && upd.uniform_type == "int" && upd.widget_type == "slider";
}

// Test mat3 with widget
bool TestEzGL_Uniforms_Widget_Mat3_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform mat3(matrix) uMat3;");
    return upd.isValid() && upd.uniform_type == "mat3" && upd.widget_type == "matrix";
}

// Test mat4 with widget
bool TestEzGL_Uniforms_Widget_Mat4_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform mat4(matrix) uMat4;");
    return upd.isValid() && upd.uniform_type == "mat4" && upd.widget_type == "matrix";
}

// Test sampler2D with widget
bool TestEzGL_Uniforms_Widget_Sampler2D_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform sampler2D(texture) uSampler;");
    return upd.isValid() && upd.uniform_type == "sampler2D" && upd.widget_type == "texture";
}

// Test sampler3D with widget
bool TestEzGL_Uniforms_Widget_Sampler3D_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform sampler3D(texture) uSampler3D;");
    return upd.isValid() && upd.uniform_type == "sampler3D" && upd.widget_type == "texture";
}

// Test samplerCube with widget
bool TestEzGL_Uniforms_Widget_SamplerCube_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform samplerCube(texture) uCubemap;");
    return upd.isValid() && upd.uniform_type == "samplerCube" && upd.widget_type == "texture";
}

bool TestEzGL_Uniforms_Widget_Multiple_Params_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform float(widget:param1:param2:param3:param4) uValue;");
    if (!upd.isValid()) {
        return false;
    }
    if (upd.widget_type != "widget") {
        return false;
    }
    if (upd.params.size() != 4U) {
        return false;
    }
    if (upd.params[0] != "param1" || upd.params[1] != "param2" ||
        upd.params[2] != "param3" || upd.params[3] != "param4") {
        return false;
    }
    return true;
}

bool TestEzGL_Uniforms_Widget_Numeric_Params_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform vec3(color:1.0:0.5:0.0) uColor;");
    if (!upd.isValid()) {
        return false;
    }
    if (upd.uniform_type != "vec3") {
        return false;
    }
    if (upd.widget_type != "color") {
        return false;
    }
    if (upd.params.size() != 3U) {
        return false;
    }
    return true;
}

bool TestEzGL_Uniforms_Comment_Extraction_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform float(slider:0.0:1.0:0.5:0.1) uValue; /* This is a comment */");
    if (!upd.isValid()) {
        return false;
    }
    if (upd.uniform_comment_original != " This is a comment ") {
        return false;
    }
    return true;
}

bool TestEzGL_Uniforms_GetFinalCode_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform float(slider:0.0:1.0:0.5:0.1) uValue;");
    if (!upd.isValid()) {
        return false;
    }
    std::string finalCode = upd.getFinalUniformCode();
    if (finalCode != "uniform float uValue;") {
        return false;
    }
    return true;
}

bool TestEzGL_Uniforms_GetFinalCode_1() {
    auto upd = ez::gl::UniformParsingDatas("uniform float(time:0.0) uValue; /* comment */");
    if (!upd.isValid()) {
        return false;
    }
    std::string finalCode = upd.getFinalUniformCode();
    if (finalCode != "uniform float uValue; //  comment ") {
        return false;
    }
    return true;
}

bool TestEzGL_Uniforms_Widget_Double_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform double(slider:0.0:1.0:0.5:0.01) uDouble;");
    return upd.isValid() && upd.uniform_type == "double";
}

bool TestEzGL_Uniforms_Widget_Ivec2_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform ivec2(slider) uIvec2;");
    return upd.isValid() && upd.uniform_type == "ivec2";
}

bool TestEzGL_Uniforms_Widget_Ivec3_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform ivec3(slider) uIvec3;");
    return upd.isValid() && upd.uniform_type == "ivec3";
}

bool TestEzGL_Uniforms_Widget_Ivec4_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform ivec4(slider) uIvec4;");
    return upd.isValid() && upd.uniform_type == "ivec4";
}

bool TestEzGL_Uniforms_Widget_Uvec2_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform uvec2(slider) uUvec2;");
    return upd.isValid() && upd.uniform_type == "uvec2";
}

bool TestEzGL_Uniforms_Widget_Uvec3_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform uvec3(slider) uUvec3;");
    return upd.isValid() && upd.uniform_type == "uvec3";
}

bool TestEzGL_Uniforms_Widget_Uvec4_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform uvec4(slider) uUvec4;");
    return upd.isValid() && upd.uniform_type == "uvec4";
}

bool TestEzGL_Uniforms_Widget_Bool_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform bool(checkbox) uBool;");
    return upd.isValid() && upd.uniform_type == "bool";
}

bool TestEzGL_Uniforms_Widget_Bvec2_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform bvec2(checkbox) uBvec2;");
    return upd.isValid() && upd.uniform_type == "bvec2";
}

bool TestEzGL_Uniforms_Widget_Bvec3_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform bvec3(checkbox) uBvec3;");
    return upd.isValid() && upd.uniform_type == "bvec3";
}

bool TestEzGL_Uniforms_Widget_Bvec4_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform bvec4(checkbox) uBvec4;");
    return upd.isValid() && upd.uniform_type == "bvec4";
}

bool TestEzGL_Uniforms_Widget_Mat2_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform mat2(matrix) uMat2;");
    return upd.isValid() && upd.uniform_type == "mat2";
}

bool TestEzGL_Uniforms_Widget_Sampler2DArray_0() {
    auto upd = ez::gl::UniformParsingDatas("uniform sampler2DArray(texture) uSampler2DArray;");
    return upd.isValid() && upd.uniform_type == "sampler2DArray";
}

////////////////////////////////////////////////////////////////////////////
//// ENTRY POINT ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzGL_Uniforms(const std::string& vTest) {
    IfTestExist(TestEzGL_Uniforms_Parsing_Good_Syntax_0);
    else IfTestExist(TestEzGL_Uniforms_Parsing_Good_Syntax_1);
    else IfTestExist(TestEzGL_Uniforms_Parsing_Good_Syntax_2);
    else IfTestExist(TestEzGL_Uniforms_Parsing_Good_Syntax_3);
    else IfTestExist(TestEzGL_Uniforms_Parsing_Good_Syntax_4);
    else IfTestExist(TestEzGL_Uniforms_Parsing_Good_Syntax_5);
    else IfTestExist(TestEzGL_Uniforms_Parsing_Good_Syntax_6);
    else IfTestExist(TestEzGL_Uniforms_Parsing_Good_Syntax_7);
    else IfTestExist(TestEzGL_Uniforms_Parsing_Good_Syntax_8);
    else IfTestExist(TestEzGL_Uniforms_Parsing_Good_Syntax_9);
    else IfTestExist(TestEzGL_Uniforms_Parsing_Good_Syntax_10);
    else IfTestExist(TestEzGL_Uniforms_Parsing_Good_Syntax_11);
    else IfTestExist(TestEzGL_Uniforms_Parsing_Bad_Syntax_0);
    else IfTestExist(TestEzGL_Uniforms_Parsing_Bad_Syntax_1);
    else IfTestExist(TestEzGL_Uniforms_Parsing_Bad_Syntax_2);
    else IfTestExist(TestEzGL_Uniforms_Parsing_Bad_Syntax_3);
    else IfTestExist(TestEzGL_Uniforms_Parsing_Bad_Syntax_4);
    else IfTestExist(TestEzGL_Uniforms_Parsing_Bad_Syntax_5);
    else IfTestExist(TestEzGL_Uniforms_Parsing_Bad_Syntax_6);
    else IfTestExist(TestEzGL_Uniforms_Parsing_Bad_Syntax_7);
    else IfTestExist(TestEzGL_Uniforms_Parsing_Time_Widget_0);
    else IfTestExist(TestEzGL_Uniforms_Parsing_Buffer_Widget_0);
    else IfTestExist(TestEzGL_Uniforms_Parsing_Slider_Widget_0);

    // Extended widget tests from TestEzGLCore.cpp
    else IfTestExist(TestEzGL_Uniforms_Widget_Vec2_0);
    else IfTestExist(TestEzGL_Uniforms_Widget_Vec3_0);
    else IfTestExist(TestEzGL_Uniforms_Widget_Vec4_0);
    else IfTestExist(TestEzGL_Uniforms_Widget_Int_0);
    else IfTestExist(TestEzGL_Uniforms_Widget_Mat3_0);
    else IfTestExist(TestEzGL_Uniforms_Widget_Mat4_0);
    else IfTestExist(TestEzGL_Uniforms_Widget_Sampler2D_0);
    else IfTestExist(TestEzGL_Uniforms_Widget_Sampler3D_0);
    else IfTestExist(TestEzGL_Uniforms_Widget_SamplerCube_0);
    else IfTestExist(TestEzGL_Uniforms_Widget_Multiple_Params_0);
    else IfTestExist(TestEzGL_Uniforms_Widget_Numeric_Params_0);
    else IfTestExist(TestEzGL_Uniforms_Comment_Extraction_0);
    else IfTestExist(TestEzGL_Uniforms_GetFinalCode_0);
    else IfTestExist(TestEzGL_Uniforms_GetFinalCode_1);
    else IfTestExist(TestEzGL_Uniforms_Widget_Double_0);
    else IfTestExist(TestEzGL_Uniforms_Widget_Ivec2_0);
    else IfTestExist(TestEzGL_Uniforms_Widget_Ivec3_0);
    else IfTestExist(TestEzGL_Uniforms_Widget_Ivec4_0);
    else IfTestExist(TestEzGL_Uniforms_Widget_Uvec2_0);
    else IfTestExist(TestEzGL_Uniforms_Widget_Uvec3_0);
    else IfTestExist(TestEzGL_Uniforms_Widget_Uvec4_0);
    else IfTestExist(TestEzGL_Uniforms_Widget_Bool_0);
    else IfTestExist(TestEzGL_Uniforms_Widget_Bvec2_0);
    else IfTestExist(TestEzGL_Uniforms_Widget_Bvec3_0);
    else IfTestExist(TestEzGL_Uniforms_Widget_Bvec4_0);
    else IfTestExist(TestEzGL_Uniforms_Widget_Mat2_0);
    else IfTestExist(TestEzGL_Uniforms_Widget_Sampler2DArray_0);

    return false;
}
