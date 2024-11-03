#include "TestEzGLUniforms.h"
#include <ezlibs/ezGL/ezGL.hpp>

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

    return false;
}
