// ============================================================================
// gpu_profiler.hpp – header‑only GPU scope profiler  ✧  v0.2  (OpenGL complete)
// ----------------------------------------------------------------------------
//  ▸ BACKEND SELECTION
//      #define GPU_PROFILER_BACKEND_OPENGL   //   OpenGL   (fully implemented)
//      #define GPU_PROFILER_BACKEND_VULKAN   //   Vulkan   (WIP – skeleton only)
//
//  ▸ SINGLE HEADER USAGE
//      #define GPU_PROFILER_HEADER_ONLY   // everything inline ‑ *or*
//      #define GPU_PROFILER_IMPL          // in **one** translation unit
//      #include "gpu_profiler.hpp"
//
//  ▸ CUSTOM INCLUDES (optional)
//      #define OPENGL_INCLUDE  <glad/gl.h>
//      #define VULKAN_INCLUDE  "volk.h"
//
//  This version merges the functional core of the original iagp_gl implementation.
//  – Hierarchical zones  (root‑frame + nested scopes)
//  – Per‑frame circular history buffer (default 3 frames)
//  – Timestamp collection via ARB_timer_query
//  – fetchResults() returns a full tree of Node objects with nanosecond timing
//
//  Missing pieces (next milestones):
//  – Average/peak statistics
//  – ImGui flame‑graph visualisation
//  – Vulkan backend (query pools + C‑API bridge)
// ============================================================================

#pragma once

// ───────────────────────────────── BACKEND SELECTION ─────────────────────────
#if !defined(GPU_PROFILER_BACKEND_OPENGL) && !defined(GPU_PROFILER_BACKEND_VULKAN)
#error "Define GPU_PROFILER_BACKEND_OPENGL or GPU_PROFILER_BACKEND_VULKAN before including gpu_profiler.hpp"
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
#include <unordered_map>
#include <utility>
#include <cassert>

// ─────────────────────────────── PUBLIC INTERFACE ────────────────────────────
namespace gp {

struct ScopeID {
    uint32_t v{0};
};

struct Node {
    std::string_view label;
    uint64_t t0{0};
    uint64_t t1{0};
    std::vector<Node> children;
};

struct FrameResults {
    bool valid{false};
    std::vector<Node> tree;  // root nodes (depth‑0)
    double nsPerTick{1.0};  // GL = 1 ; VK = timestampPeriod
};

#ifdef GPU_PROFILER_BACKEND_OPENGL
using QueryT = GLuint;
struct BackendCtx { /* no extra state needed for GL */
};
#elif defined(GPU_PROFILER_BACKEND_VULKAN)
using QueryT = uint32_t;
struct BackendCtx { /* TODO – device/queue/pool… */
};
#endif

struct QueryHandle {
    QueryT start{0}, end{0};
};

// ───────────────────────────── BackendIface template ─────────────────────────
template <typename CTX>
struct BackendIface;  // primary template – specialised below

#ifdef GPU_PROFILER_BACKEND_OPENGL
// ––– OpenGL implementation –––––––––––––––––––––––––––––––––––––––––––––––––––
template <>
struct BackendIface<BackendCtx> {
    static QueryHandle alloc(BackendCtx&) {
        QueryHandle h{};
        glGenQueries(1, &h.start);
        glGenQueries(1, &h.end);
        return h;
    }

    static void free(BackendCtx&, QueryHandle h) {
        glDeleteQueries(1, &h.start);
        glDeleteQueries(1, &h.end);
    }

    static void writeBegin(BackendCtx&, const QueryHandle& h) { glQueryCounter(h.start, GL_TIMESTAMP); }

    static void writeEnd(BackendCtx&, const QueryHandle& h) { glQueryCounter(h.end, GL_TIMESTAMP); }

    static bool fetch(BackendCtx&, const QueryHandle& h, uint64_t& outStart, uint64_t& outEnd) {
        GLuint available = 0;
        glGetQueryObjectuiv(h.end, GL_QUERY_RESULT_AVAILABLE, &available);
        if (available == GL_TRUE) {
            glGetQueryObjectui64v(h.start, GL_QUERY_RESULT, &outStart);
            glGetQueryObjectui64v(h.end, GL_QUERY_RESULT, &outEnd);
            return true;
        }
        return false;
    }

    static constexpr double nsPerTick() { return 1.0; }  // GL timestamps already ns
};
#endif

#ifdef GPU_PROFILER_BACKEND_VULKAN
// ––– Vulkan skeleton (not yet implemented) –––––––––––––––––––––––––––––––––––
template <>
struct BackendIface<BackendCtx> {
    static QueryHandle alloc(BackendCtx&) { return {}; }
    static void free(BackendCtx&, QueryHandle) {}
    static void writeBegin(BackendCtx&, const QueryHandle&) {}
    static void writeEnd(BackendCtx&, const QueryHandle&) {}
    static bool fetch(BackendCtx&, const QueryHandle&, uint64_t&, uint64_t&) { return false; }
    static constexpr double nsPerTick() { return 0.0; }
};
#endif

// ────────────────────────────────  Profiler  ─────────────────────────────────
class Profiler {
public:
    explicit Profiler() { m_ctx = BackendCtx{}; }
    ~Profiler() { clear(); }

    // ── Frame delimitation ───────────────────────────────────────────────────
    void beginFrame() {
        if (!m_enabled)
            return;
        m_frameZones.clear();
        while (!m_stack.empty())
            m_stack.pop();
    }

    void endFrame() {
        if (!m_enabled)
            return;
        m_history.push_back(std::move(m_frameZones));
        if (m_history.size() > m_maxHistory)
            m_history.erase(m_history.begin());
    }

    // ── Scoped zones ─────────────────────────────────────────────────────────
    ScopeID beginScope(const char* label) {
        if (!m_enabled)
            return {0};
        Zone z;
        z.label = label;
        z.depth = static_cast<uint32_t>(m_stack.size());
        z.parent = m_stack.empty() ? UINT32_MAX : m_stack.top();
        z.q = BackendIface<BackendCtx>::alloc(m_ctx);
        BackendIface<BackendCtx>::writeBegin(m_ctx, z.q);

        uint32_t id = static_cast<uint32_t>(m_zones.size());
        m_zones.push_back(std::move(z));
        if (z.parent != UINT32_MAX) {
            m_zones[z.parent].children.push_back(id);
        }
        m_stack.push(id);
        return {id};
    }

    void endScope(ScopeID id) {
        if (!m_enabled || id.v >= m_zones.size())
            return;
        BackendIface<BackendCtx>::writeEnd(m_ctx, m_zones[id.v].q);
        m_zones[id.v].closed = true;
        if (!m_stack.empty() && m_stack.top() == id.v)
            m_stack.pop();
        m_frameZones.push_back(id.v);
    }

    // ── Results fetch (latest finished frame) ───────────────────────────────
    FrameResults fetchResults() {
        FrameResults fr{};
        if (!m_enabled || m_history.empty())
            return fr;

        const auto& finishedIDs = m_history.back();
        fr.nsPerTick = BackendIface<BackendCtx>::nsPerTick();

        // Map ID → Node* while building to attach children efficiently
        std::vector<Node*> idToNode(m_zones.size(), nullptr);
        fr.tree.reserve(finishedIDs.size());

        // First pass: create Node objects where timestamps are available
        for (uint32_t id : finishedIDs) {
            const Zone& z = m_zones[id];
            uint64_t t0 = 0, t1 = 0;
            if (!BackendIface<BackendCtx>::fetch(m_ctx, z.q, t0, t1))
                continue;  // not ready, skip

            Node n{z.label, t0, t1};
            fr.valid = true;
            m_nodesBuffer.push_back(std::move(n));  // persistent buffer to keep pointers valid
            idToNode[id] = &m_nodesBuffer.back();
        }

        // Second pass: attach children -> parents, build roots list
        for (uint32_t id : finishedIDs) {
            if (!idToNode[id])
                continue;  // timestamp not ready
            const Zone& z = m_zones[id];
            Node* cur = idToNode[id];
            if (z.parent == UINT32_MAX || !idToNode[z.parent]) {
                fr.tree.push_back(std::move(*cur));
            } else {
                idToNode[z.parent]->children.push_back(std::move(*cur));
            }
        }

        m_nodesBuffer.clear();  // nodes moved into fr.tree, safe to clear
        return fr;
    }

    // ── Control ─────────────────────────────────────────────────────────────
    void setEnabled(bool b) { m_enabled = b; }
    bool isEnabled() const { return m_enabled; }

private:
    struct Zone {
        std::string label;
        QueryHandle q;
        uint32_t depth{0};
        uint32_t parent{UINT32_MAX};
        std::vector<uint32_t> children;  // ids
        bool closed{false};
    };

    void clear() {
        for (auto& z : m_zones)
            BackendIface<BackendCtx>::free(m_ctx, z.q);
        m_zones.clear();
        m_frameZones.clear();
        m_history.clear();
        m_nodesBuffer.clear();
        while (!m_stack.empty())
            m_stack.pop();
    }

    // ── Members ──────────────────────────────────────────────────────────────
    BackendCtx m_ctx{};
    bool m_enabled{true};

    std::vector<Zone> m_zones;  // all zones ever allocated
    std::stack<uint32_t> m_stack;  // current scope stack

    std::vector<uint32_t> m_frameZones;  // zones closed this frame
    std::vector<std::vector<uint32_t>> m_history;  // finished frames (circular)
    size_t m_maxHistory{3};

    std::vector<Node> m_nodesBuffer;  // temp buffer while building tree
};

}  // namespace gp

// ─────────────────────────── IMPLEMENTATION GUARD ───────────────────────────
#if defined(GPU_PROFILER_IMPL) || defined(GPU_PROFILER_HEADER_ONLY)
// all functions already defined inline – nothing to do
#endif

// ============================================================================
// end gpu_profiler.hpp
// ============================================================================
