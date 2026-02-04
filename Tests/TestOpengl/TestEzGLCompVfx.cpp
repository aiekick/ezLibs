#include "TestEzGLCompVfx.h"
#include "glContext.h"
#include <ezlibs/ezTools.hpp>
#include <ezlibs/ezGL/ezGL.hpp>
#include <ezlibs/ezCTest.hpp>

class Effect {
private:
public:
    bool init(const ez::fvec4& vDisplayRect) { return false; }
    bool reInit() { return false; }
    void unit() {}
    bool resize(const ez::fvec4& vDisplayRect) { return false; }
    void render() {}
    void clearBuffers(const ez::fvec4& vColor) {}
    void blitOnScreen(const ez::fvec4& vDisplayRect) {}
};

////////////////////////////////////////////////////////////////////////////
//// GOOD SYNTAX ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzGL_CompVfx_init() {
    Effect eff;
    return true;
}

////////////////////////////////////////////////////////////////////////////
//// ENTRY POINT ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzGL_CompVfx(const std::string& vTest) {
    IfTestExist(TestEzGL_CompVfx_init);
    return false;
}
