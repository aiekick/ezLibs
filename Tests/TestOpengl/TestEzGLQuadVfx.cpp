#include "TestEzGLQuadVfx.h"
#include "glContext.h"
#include <ezlibs/ezTools.hpp>
#include <ezlibs/ezGL/ezGL.hpp>
#include <ezlibs/ezCTest.hpp>

class Effect {
public:
    struct EffectUniforms {
        float time = 0.0f;
        int32_t frame = 0;
        ez::fvec4 mouse;  // xy:pos, zw:pressed pos
        ez::fvec3 resolution;
        uint32_t backBufferId{};
    } uniforms;

private:
    ez::gl::QuadMeshPtr m_QuadMeshPtr = nullptr;
    ez::gl::ShaderPtr m_QuadVertPtr = nullptr;
    ez::gl::QuadVfxPtr m_QuadVfxPtr = nullptr;

public:
    bool init(const ez::fvec4& vDisplayRect) {
        CTEST_ASSERT(GLContext::initGLContext());
        m_QuadMeshPtr = ez::gl::QuadMesh::create();
        if (m_QuadMeshPtr != nullptr) {
            auto vert_shader_fpn = INPUTS_DIR "/quadVfx.vert";
            auto frag_shader_fpn = INPUTS_DIR "/quadVfx.frag";
            m_QuadVertPtr = ez::gl::Shader::createFromFile("Quad", GL_VERTEX_SHADER, vert_shader_fpn);
            if (m_QuadVertPtr != nullptr) {
                m_QuadVfxPtr =
                    ez::gl::QuadVfx::createFromFile("Vfx", m_QuadVertPtr, m_QuadMeshPtr, frag_shader_fpn, vDisplayRect.size().x, vDisplayRect.size().y, 2U, true, true);
                if (m_QuadVfxPtr != nullptr) {
                    m_QuadVfxPtr->addUniformInt(GL_FRAGMENT_SHADER, "iFrame", &uniforms.frame, 1U);
                    uniforms.resolution.x = vDisplayRect.size().x;
                    uniforms.resolution.y = vDisplayRect.size().y;
                    uniforms.resolution.z = 1.0f;
                    m_QuadVfxPtr->addUniformFloat(GL_FRAGMENT_SHADER, "iMouse", &uniforms.mouse.x, 4U);
                    m_QuadVfxPtr->addUniformFloat(GL_FRAGMENT_SHADER, "iResolution", &uniforms.resolution.x, 3U);
                    m_QuadVfxPtr->addUniformSampler2D(GL_FRAGMENT_SHADER, "backBuffer", &uniforms.backBufferId, false);
                    m_QuadVfxPtr->setUniformPreUploadFunctor([this](ez::gl::FBOPipeLinePtr vFBOPipeLinePtr, ez::gl::Program::Uniform& vUniform) {
                        if (vFBOPipeLinePtr != nullptr) {
                            if (vUniform.name == "backBuffer") {
                                uniforms.backBufferId = vFBOPipeLinePtr->getBackTextureId(0);
                            }
                        }
                    });
                    m_QuadVfxPtr->finalizeBeforeRendering();
                    m_QuadVfxPtr->setRenderingIterations(1U);
                    return true;
                }
            }
        }
        return false;
    }

    bool reInit() { return false; }
    void unit() {
        m_QuadVfxPtr.reset();
        m_QuadVertPtr.reset();
        m_QuadMeshPtr.reset();
        GLContext::unitGLContext();
    }
    bool resize(const ez::fvec4& vDisplayRect) {
        const auto& size = vDisplayRect.size();
        if (m_QuadVfxPtr->resize(static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y))) {
            uniforms.resolution.x = size.x;
            uniforms.resolution.y = size.x;
            uniforms.resolution.z = 1.0f;
            return true;
        }
        return false;
    }
    void render() {
        m_QuadVfxPtr->render();
        ++uniforms.frame;
    }
    void clearBuffers(const ez::fvec4& vColor) { m_QuadVfxPtr->clearBuffers({vColor.x, vColor.y, vColor.z, vColor.w}); }
    void blitOnScreen(const ez::fvec4& vDisplayRect) {
        auto fbo_ptr = m_QuadVfxPtr->getFrontFBO().lock();
        if (fbo_ptr != nullptr) {
            fbo_ptr->blitOnScreen(
                static_cast<GLint>(vDisplayRect.pos().x),   //
                static_cast<GLint>(vDisplayRect.pos().y),   //
                static_cast<GLint>(vDisplayRect.size().x),  //
                static_cast<GLint>(vDisplayRect.size().y),  //
                0,
                GL_COLOR_BUFFER_BIT,
                GL_NEAREST);
        }
    }
    std::vector<ez::u8vec4> getPixels(const ez::fvec4& vRect) {
        return m_QuadVfxPtr->getBackFBO().lock()->getPixels<ez::u8vec4, 1>(  //
            vRect.pos().x,
            vRect.pos().y,
            vRect.size().x,
            vRect.size().y);
    }
};

////////////////////////////////////////////////////////////////////////////
//// GOOD SYNTAX ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzGL_QuadVfx_init() {
    Effect eff;
    CTEST_ASSERT(eff.init(ez::fvec4(0, 0, 4, 4)));

    // first render : yellow output (1,1,0,1)
    eff.render(); 
    auto pixs = eff.getPixels(ez::fvec4(0, 0, 4, 4));
    CTEST_ASSERT(pixs[0] == ez::u8vec4(255, 255, 0, 255));

    // second render : blue output (0,0,1,1)
    eff.render();
    pixs = eff.getPixels(ez::fvec4(0, 0, 4, 4));
    CTEST_ASSERT(pixs[0] == ez::u8vec4(0, 0, 255, 255));

    // resize
    CTEST_ASSERT(eff.resize(ez::fvec4(0, 0, 2, 2)));

    // third render : no white since, resize cause black output
    eff.render();
    pixs = eff.getPixels(ez::fvec4(0, 0, 2, 2));
    CTEST_ASSERT(pixs[0] == ez::u8vec4(255, 255, 255, 255));

    // resize again
    CTEST_ASSERT(eff.resize(ez::fvec4(0, 0, 1, 1)));
    // and set red output
    eff.clearBuffers(ez::fvec4(1, 0, 0, 1));
    pixs = eff.getPixels(ez::fvec4(0, 0, 1, 1));
    CTEST_ASSERT(pixs[0] == ez::u8vec4(255, 0, 0, 255));

    // fourth render : pink output now (0,1,1,1)
    eff.render();
    pixs = eff.getPixels(ez::fvec4(0, 0, 1, 1));
    CTEST_ASSERT(pixs[0] == ez::u8vec4(0, 255, 255, 255));

    // blit
    eff.blitOnScreen(ez::fvec4(0, 0, 4, 4));

    eff.unit();
    return true;
}

////////////////////////////////////////////////////////////////////////////
//// ENTRY POINT ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzGL_QuadVfx(const std::string& vTest) {
    IfTestExist(TestEzGL_QuadVfx_init);
    return false;
}
