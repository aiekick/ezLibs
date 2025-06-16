// ============================================================================
// gpu_profiler.hpp – **header‑only** GPU scope profiler (skeleton v0.1)
// ----------------------------------------------------------------------------
//  ➤ Usage ‑ minimal
//      #define GPU_PROFILER_BACKEND_OPENGL         // or _VULKAN (WIP)
//      #define GPU_PROFILER_HEADER_ONLY            // or GPU_PROFILER_IMPL in *one* TU
//      #include "gpu_profiler.hpp"
//
//  ➤ Customise includes if needed *before* including this file
//      #define OPENGL_INCLUDE   <GL/gl3w.h>        // glad / glbinding etc.
//      #define VULKAN_INCLUDE   <vulkan/vulkan.h>  // volk etc.
// ----------------------------------------------------------------------------
//  NOTE:  This is **only the foundation**.  The functional core extracted from
//         iagp_gl / iagp_vk is still TODO.  The current code compiles for the
//         OpenGL backend and records basic timestamps, but the scope hierarchy,
//         ImGui view, and Vulkan backend are stubs for now.
// ============================================================================

#pragma once

// -----------------------------------------------------------------------------
// 🛠 Backend selection ---------------------------------------------------------
// -----------------------------------------------------------------------------
#if !defined(GPU_PROFILER_BACKEND_OPENGL) && !defined(GPU_PROFILER_BACKEND_VULKAN)
#  error "Define GPU_PROFILER_BACKEND_OPENGL or GPU_PROFILER_BACKEND_VULKAN before including gpu_profiler.hpp"
#endif

#ifdef GPU_PROFILER_BACKEND_OPENGL
#include OPENGL_LOADER
#elif defined(GPU_PROFILER_BACKEND_VULKAN)
#include VULKAN_INCLUDE
#endif

#include <cstdint>
#include <vector>
#include <string_view>
#include <stack>
#include <utility>

// -----------------------------------------------------------------------------
namespace gp {
// -----------------------------------------------------------------------------
// 🍃 Tiny backend‑agnostic types ----------------------------------------------
// -----------------------------------------------------------------------------
struct ScopeID { uint32_t v { 0 }; };
struct Node {
    std::string_view          label;
    uint64_t                  t0  { 0 };
    uint64_t                  t1  { 0 };
    std::vector<Node>         children;
};
struct FrameResults {
    bool                      valid          { false };
    std::vector<Node>         tree;                 // roots depth‑0
    double                    nsPerTick      { 1.0 };  // GL=1, VK = timestampPeriod
};

// Forward decl for user handles ------------------------------------------------
#ifdef GPU_PROFILER_BACKEND_OPENGL
    using QueryT = GLuint;
#elif defined(GPU_PROFILER_BACKEND_VULKAN)
    using QueryT = uint32_t;
#endif

struct QueryHandle { QueryT start { 0 }, end { 0 }; };

// -----------------------------------------------------------------------------
// 🧩 BackendCtx – carries API‑specific state -----------------------------------
// -----------------------------------------------------------------------------
struct BackendCtxGL { /* nothing yet – context assumed current */ };
struct BackendCtxVK { /* TODO – device / queue / cmd pool / timestampPeriod */ };

// -----------------------------------------------------------------------------
// BackendIface   (static helpers specialised per backend) ----------------------
// -----------------------------------------------------------------------------
template<typename BACKEND_CTX> struct BackendIface; // primary template – undefined

#ifdef GPU_PROFILER_BACKEND_OPENGL
// --- OpenGL implementation ----------------------------------------------------
template<> struct BackendIface<BackendCtxGL>
{
    static QueryHandle alloc  (BackendCtxGL&) {
        QueryHandle h {};
        glGenQueries(1, &h.start);
        glGenQueries(1, &h.end);
        return h;
    }
    static void free          (BackendCtxGL&, QueryHandle h) {
        glDeleteQueries(1, &h.start);
        glDeleteQueries(1, &h.end);
    }
    static void writeBegin    (BackendCtxGL&, const QueryHandle& h) {
        glQueryCounter(h.start, GL_TIMESTAMP);
    }
    static void writeEnd      (BackendCtxGL&, const QueryHandle& h) {
        glQueryCounter(h.end, GL_TIMESTAMP);
    }
    static bool fetch         (BackendCtxGL&, const QueryHandle& h,
                               uint64_t& outStart, uint64_t& outEnd) {
        GLuint available = 0;
        glGetQueryObjectuiv(h.end, GL_QUERY_RESULT_AVAILABLE, &available);
        if (available == GL_TRUE) {
            glGetQueryObjectui64v(h.start, GL_QUERY_RESULT, &outStart);
            glGetQueryObjectui64v(h.end,   GL_QUERY_RESULT, &outEnd);
            return true;
        }
        return false;
    }
    static constexpr double nsPerTick() { return 1.0; } // GL timestamps are already in ns
};
#endif // OPENGL

#ifdef GPU_PROFILER_BACKEND_VULKAN
// --- Vulkan implementation (stub) --------------------------------------------
template<> struct BackendIface<BackendCtxVK>
{
    // TODO: allocate queries from VkQueryPool (created elsewhere)
    static QueryHandle alloc  (BackendCtxVK&) { return {}; }
    static void free          (BackendCtxVK&, QueryHandle) {}
    static void writeBegin    (BackendCtxVK&, const QueryHandle&) {}
    static void writeEnd      (BackendCtxVK&, const QueryHandle&) {}
    static bool fetch         (BackendCtxVK&, const QueryHandle&,
                               uint64_t&, uint64_t&) { return false; }
    static constexpr double nsPerTick() { return 0.0; /* filled at runtime */ }
};
#endif // VULKAN

// -----------------------------------------------------------------------------
// 🎛  Profiler – public interface ----------------------------------------------
// -----------------------------------------------------------------------------
class Profiler {
public:
    #ifdef GPU_PROFILER_BACKEND_OPENGL
        using Ctx = BackendCtxGL;
    #else
        using Ctx = BackendCtxVK;
    #endif

    explicit Profiler(/* backend‑specific params – for GL none */)
    {
        m_ctx = Ctx{};
        m_enabled = true;
    }
    ~Profiler() {
        clearAllZones();
    }

    // Frame delimitation -------------------------------------------------------
    inline void beginFrame() {
        if (!m_enabled) return;
        m_currentFrameZones.clear();
        m_scopeStack = {};
    }

    inline void endFrame() {
        if (!m_enabled) return;
        // Collect zones finished *this* frame into circular buffer
        m_previousFrames.push_back(std::move(m_currentFrameZones));
        if (m_previousFrames.size() > m_history) m_previousFrames.erase(m_previousFrames.begin());
    }

    // Scopes ------------------------------------------------------------------
    inline ScopeID beginScope(const char* label) {
        if (!m_enabled) return {0};
        Zone z;
        z.label = label;
        z.q = BackendIface<Ctx>::alloc(m_ctx);
        BackendIface<Ctx>::writeBegin(m_ctx, z.q);
        const uint32_t id = static_cast<uint32_t>(m_zones.size());
        m_zones.push_back(std::move(z));
        m_scopeStack.push(id);
        return { id };
    }

    inline void endScope(ScopeID id) {
        if (!m_enabled) return;
        if (id.v >= m_zones.size()) return; // invalid
        auto& z = m_zones[id.v];
        BackendIface<Ctx>::writeEnd(m_ctx, z.q);
        z.completed = true;
        if (!m_scopeStack.empty()) m_scopeStack.pop();
        m_currentFrameZones.push_back(id.v);
    }

    // Results -----------------------------------------------------------------
    inline FrameResults fetchResults() {
        FrameResults fr;
        if (!m_enabled) return fr;

        // For simplicity we pull the *latest* finished frame (if any).
        if (m_previousFrames.empty()) return fr;
        const auto& finishedIds = m_previousFrames.back();
        fr.valid = true;
        fr.nsPerTick = BackendIface<Ctx>::nsPerTick();
        fr.tree.reserve(finishedIds.size());

        // Build a flat list; hierarchy TODO.
        for (auto id : finishedIds) {
            const Zone& z = m_zones[id];
            uint64_t t0=0, t1=0;
            if (BackendIface<Ctx>::fetch(m_ctx, z.q, t0, t1)) {
                Node n { z.label, t0, t1 }; // no children for now
                fr.tree.push_back(std::move(n));
            }
        }
        return fr;
    }

    // Enable / disable --------------------------------------------------------
    inline void setEnabled(bool b) { m_enabled = b; }
    inline bool isEnabled() const  { return m_enabled; }

private:
    struct Zone {
        std::string           label;
        QueryHandle           q {};
        bool                  completed { false };
    };

    void clearAllZones() {
        for (auto& z : m_zones) {
            BackendIface<Ctx>::free(m_ctx, z.q);
        }
        m_zones.clear();
        m_previousFrames.clear();
        m_currentFrameZones.clear();
        while (!m_scopeStack.empty()) m_scopeStack.pop();
    }

    // State -------------------------------------------------------------------
    Ctx                             m_ctx {};
    bool                            m_enabled { false };

    std::vector<Zone>               m_zones;             // ever allocated
    std::stack<uint32_t>            m_scopeStack;        // current hierarchy (flat for now)

    std::vector<uint32_t>           m_currentFrameZones; // ids finished this frame
    std::vector<std::vector<uint32_t>> m_previousFrames; // circular buffer
    size_t                          m_history { 3 };     // N frames in history
};

// -----------------------------------------------------------------------------
} // namespace gp

// ============================================================================
// End gpu_profiler.hpp (skeleton)
// ============================================================================
