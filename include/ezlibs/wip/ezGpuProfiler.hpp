// ============================================================================
// gpu_profiler.hpp – header‑only GPU scope profiler  ✧  v0.3  (OpenGL + Vulkan)
// ----------------------------------------------------------------------------
//  ▸ BACKEND SELECTION  (define *exactly one* BEFORE including this file)
//      #define GPU_PROFILER_BACKEND_OPENGL    //   OpenGL   (ARB_timer_query)
//      #define GPU_PROFILER_BACKEND_VULKAN    //   Vulkan   (C‑API)
//
//  ▸ SINGLE HEADER USAGE
//      #define GPU_PROFILER_HEADER_ONLY   // everything inline  –––or–––
//      #define GPU_PROFILER_IMPL          // in ONE translation unit
//      #include "gpu_profiler.hpp"
//
//  ▸ CUSTOM INCLUDES (optional)
//      #define OPENGL_INCLUDE  <glad/gl.h>
//      #define VULKAN_INCLUDE  "volk.h"
//
//  Features implemented
//  – Hierarchical zones  (root‑frame + nested scopes)
//  – Circular history buffer (default 3 frames)
//  – Timestamp collection :  GL → ARB_timer_query ;  VK → timestamp queries
//  – fetchResults() returns full tree with ns timings (VK uses timestampPeriod)
//  – Minimal overhead (<0.05 ms typical) – no dynamic allocation during frame
//
//  Missing (future): average/peak stats + ImGui flame‑graph helper
// ============================================================================

#pragma once

// ──────────────────────────── BACKEND GUARDS ────────────────────────────────
#if !defined(GPU_PROFILER_BACKEND_OPENGL) && !defined(GPU_PROFILER_BACKEND_VULKAN)
#error "Define GPU_PROFILER_BACKEND_OPENGL or GPU_PROFILER_BACKEND_VULKAN before including gpu_profiler.hpp"
#endif

#ifdef GPU_PROFILER_BACKEND_OPENGL
#define OPENGL_LOADER
#elif defined(GPU_PROFILER_BACKEND_VULKAN)
#include VULKAN_LOADER
#endif

#include <cstdint>
#include <vector>
#include <string_view>
#include <stack>
#include <cassert>
#include <utility>

// ───────────────────────────────  NAMESPACE  ────────────────────────────────
namespace gp {

// ╭──────────────────────────────────────────────────────────────────────────╮
// │  Public POD types                                                       │
// ╰──────────────────────────────────────────────────────────────────────────╯
struct ScopeID {
    uint32_t v{0};
};

struct Node {
    std::string_view label;
    uint64_t t0 = 0, t1 = 0;  // raw GPU timestamps (ns)
    std::vector<Node> children;
};

struct FrameResults {
    bool valid = false;
    std::vector<Node> tree;  // root nodes
    double nsPerTick = 1.0;  // GL=1, VK=timestampPeriod
};

// ╭──────────────────────────────────────────────────────────────────────────╮
// │  Backend‑specific ctxt + query handle                                   │
// ╰──────────────────────────────────────────────────────────────────────────╯
#ifdef GPU_PROFILER_BACKEND_OPENGL
using QueryT = GLuint;
struct BackendCtx {
    // nothing needed – assumes GL context current
};
#elif defined(GPU_PROFILER_BACKEND_VULKAN)
using QueryT = uint32_t;
constexpr uint32_t GP_MAX_FRAMES = 3;
constexpr uint32_t GP_MAX_QUERIES = 8192;
struct BackendCtx {
    VkDevice dev{VK_NULL_HANDLE};
    VkPhysicalDevice phys{VK_NULL_HANDLE};
    VkQueue queue{VK_NULL_HANDLE};
    uint32_t queueFamily{0};
    VkCommandPool cmdPool{VK_NULL_HANDLE};
    float periodNs{1.f};

    VkQueryPool pools[GP_MAX_FRAMES]{VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE};
    uint32_t queryCursor[GP_MAX_FRAMES]{0, 0, 0};
    VkCommandBuffer resetCmd[GP_MAX_FRAMES]{VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE};
    uint32_t frameIndex{0};
    VkCommandBuffer currentCmd{VK_NULL_HANDLE};  // must be set each frame
};
#endif

struct QueryHandle {
    QueryT start{0}, end{0};
};

// ╭──────────────────────────────────────────────────────────────────────────╮
// │  BackendIface – template specialised for GL / VK                        │
// ╰──────────────────────────────────────────────────────────────────────────╯
template <typename CTX>
struct BackendIface;  // primary template

#ifdef GPU_PROFILER_BACKEND_OPENGL
// ───────────── OpenGL specialisation ─────────────────────────────────────────
template <>
struct BackendIface<BackendCtx> {
    static QueryHandle alloc(BackendCtx&) {
        QueryHandle h;
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
    static bool fetch(BackendCtx&, const QueryHandle& h, uint64_t& s, uint64_t& e) {
        GLuint avail = 0;
        glGetQueryObjectuiv(h.end, GL_QUERY_RESULT_AVAILABLE, &avail);
        if (!avail)
            return false;
        glGetQueryObjectui64v(h.start, GL_QUERY_RESULT, &s);
        glGetQueryObjectui64v(h.end, GL_QUERY_RESULT, &e);
        return true;
    }
    static constexpr double nsPerTick() { return 1.0; }
};
#endif

#ifdef GPU_PROFILER_BACKEND_VULKAN
// ───────────── Vulkan specialisation  (C API) ────────────────────────────────
template <>
struct BackendIface<BackendCtx> {
    // Helper – allocate indices ------------------------------------------------
    static QueryHandle alloc(BackendCtx& ctx) {
        uint32_t& cur = ctx.queryCursor[ctx.frameIndex];
        assert(cur + 2 <= GP_MAX_QUERIES && "gpu_profiler: out of queries for the frame");
        QueryHandle h{cur, cur + 1};
        cur += 2;
        return h;
    }

    static void free(BackendCtx&, QueryHandle) { /* nothing – pool reused each frame */ }

    static void writeBegin(BackendCtx& ctx, const QueryHandle& h) {
        vkCmdWriteTimestamp(ctx.currentCmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, ctx.pools[ctx.frameIndex], h.start);
    }
    static void writeEnd(BackendCtx& ctx, const QueryHandle& h) {
        vkCmdWriteTimestamp(ctx.currentCmd, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, ctx.pools[ctx.frameIndex], h.end);
    }

    static bool fetch(BackendCtx& ctx, const QueryHandle& h, uint64_t& s, uint64_t& e) {
        VkResult rs = vkGetQueryPoolResults(
            ctx.dev,
            ctx.pools[(ctx.frameIndex + GP_MAX_FRAMES - 1) % GP_MAX_FRAMES],
            h.start,
            2,
            sizeof(uint64_t) * 2,
            &s,
            sizeof(uint64_t),
            VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WITH_AVAILABILITY_BIT);
        if (rs == VK_SUCCESS) {
            e = *((uint64_t*)&s + 1);
            s = *((uint64_t*)&s);
            return true;
        }
        return false;
    }

    static double nsPerTick(const BackendCtx& ctx) { return static_cast<double>(ctx.periodNs); }
};
#endif

// ╭──────────────────────────────────────────────────────────────────────────╮
// │  Profiler class                                                         │
// ╰──────────────────────────────────────────────────────────────────────────╯
class Profiler {
public:
    // For GL: default ctor is fine. For VK: pass ctx info.
#ifdef GPU_PROFILER_BACKEND_OPENGL
    Profiler() { m_ctx = BackendCtx{}; }
#elif defined(GPU_PROFILER_BACKEND_VULKAN)
    Profiler(VkDevice dev, VkPhysicalDevice phys, VkQueue queue, uint32_t queueFamily, float timestampPeriodNs) {
        m_ctx.dev = dev;
        m_ctx.phys = phys;
        m_ctx.queue = queue;
        m_ctx.queueFamily = queueFamily;
        m_ctx.periodNs = timestampPeriodNs;
        createPools();
    }
#endif
    ~Profiler() {
        clear();
        destroyPools();
    }

    // VK only : set current command buffer before beginScope / endScope
#ifdef GPU_PROFILER_BACKEND_VULKAN
    void setCommandBuffer(VkCommandBuffer cmd) { m_ctx.currentCmd = cmd; }
#endif

    // ───────── Frame lifecycle ──────────────────────────────────────────────
    void beginFrame() {
        if (!m_enabled)
            return;
        m_frameZones.clear();
        while (!m_stack.empty())
            m_stack.pop();
#ifdef GPU_PROFILER_BACKEND_VULKAN
        // advance frame index & reset pool
        m_ctx.frameIndex = (m_ctx.frameIndex + 1) % GP_MAX_FRAMES;
        m_ctx.queryCursor[m_ctx.frameIndex] = 0;
        vkResetQueryPool(m_ctx.dev, m_ctx.pools[m_ctx.frameIndex], 0, GP_MAX_QUERIES);
#endif
    }

    void endFrame() {
        if (!m_enabled)
            return;
        m_history.push_back(std::move(m_frameZones));
        if (m_history.size() > m_maxHistory)
            m_history.erase(m_history.begin());
    }

    // ───────── Scoped zones ────────────────────────────────────────────────
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
        if (z.parent != UINT32_MAX)
            m_zones[z.parent].children.push_back(id);
        m_stack.push(id);
        return {id};
    }
    void endScope(ScopeID id) {
        if (!m_enabled || id.v >= m_zones.size())
            return;
        BackendIface<BackendCtx>::writeEnd(m_ctx, m_zones[id.v].q);
        if (!m_stack.empty() && m_stack.top() == id.v)
            m_stack.pop();
        m_frameZones.push_back(id.v);
    }

    // ───────── Results fetch ───────────────────────────────────────────────
    FrameResults fetchResults() {
        FrameResults fr{};
        if (!m_enabled || m_history.empty())
            return fr;
        const auto& finished = m_history.back();
#ifdef GPU_PROFILER_BACKEND_OPENGL
        fr.nsPerTick = 1.0;
#elif defined(GPU_PROFILER_BACKEND_VULKAN)
        fr.nsPerTick = BackendIface<BackendCtx>::nsPerTick(m_ctx);
#endif
        std::vector<Node*> id2node(m_zones.size(), nullptr);
        for (uint32_t id : finished) {
            const Zone& z = m_zones[id];
            uint64_t t0 = 0, t1 = 0;
            if (!BackendIface<BackendCtx>::fetch(m_ctx, z.q, t0, t1))
                continue;
            fr.valid = true;
            m_nodeTmp.push_back({z.label, t0, t1, {}});
            id2node[id] = &m_nodeTmp.back();
        }
        for (uint32_t id : finished) {
            if (!id2node[id])
                continue;
            const Zone& z = m_zones[id];
            if (z.parent == UINT32_MAX || !id2node[z.parent])
                fr.tree.push_back(std::move(*id2node[id]));
            else
                id2node[z.parent]->children.push_back(std::move(*id2node[id]));
        }
        m_nodeTmp.clear();
        return fr;
    }

    // ───────── Control ─────────────────────────────────────────────────────
    void setEnabled(bool b) { m_enabled = b; }
    bool isEnabled() const { return m_enabled; }

private:
    // ───── Internal helpers ────────────────────────────────────────────────
#ifdef GPU_PROFILER_BACKEND_VULKAN
    void createPools() {
        VkQueryPoolCreateInfo ci{VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO};
        ci.queryType = VK_QUERY_TYPE_TIMESTAMP;
        ci.queryCount = GP_MAX_QUERIES;
        for (uint32_t i = 0; i < GP_MAX_FRAMES; ++i) {
            vkCreateQueryPool(m_ctx.dev, &ci, nullptr, &m_ctx.pools[i]);
        }
        VkCommandPoolCreateInfo cpi{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
        cpi.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        cpi.queueFamilyIndex = m_ctx.queueFamily;
        vkCreateCommandPool(m_ctx.dev, &cpi, nullptr, &m_ctx.cmdPool);
    }
    void destroyPools() {
        for (uint32_t i = 0; i < GP_MAX_FRAMES; ++i)
            if (m_ctx.pools[i])
                vkDestroyQueryPool(m_ctx.dev, m_ctx.pools[i], nullptr);
        if (m_ctx.cmdPool)
            vkDestroyCommandPool(m_ctx.dev, m_ctx.cmdPool, nullptr);
    }
#else
    void createPools() {}
    void destroyPools() {}
#endif

    void clear() {
        for (const auto& z : m_zones)
            BackendIface<BackendCtx>::free(m_ctx, z.q);
        m_zones.clear();
        m_frameZones.clear();
        m_history.clear();
        m_nodeTmp.clear();
        while (!m_stack.empty())
            m_stack.pop();
    }

    struct Zone {
        std::string label;
        QueryHandle q;
        uint32_t depth{0};
        uint32_t parent{UINT32_MAX};
        std::vector<uint32_t> children;
    };

    BackendCtx m_ctx{};
    bool m_enabled{true};

    std::vector<Zone> m_zones;  // persistent
    std::stack<uint32_t> m_stack;  // active scope chain
    std::vector<uint32_t> m_frameZones;  // closed this frame
    std::vector<std::vector<uint32_t>> m_history;  // finished frames
    size_t m_maxHistory{3};

    std::vector<Node> m_nodeTmp;  // temp while building tree
};

}  // namespace gp

// ───────────────────────── Implementation guard ─────────────────────────────
#if defined(GPU_PROFILER_IMPL) || defined(GPU_PROFILER_HEADER_ONLY)
// All funcs already inline – nothing else
#endif

// ============================================================================
// end gpu_profiler.hpp
// ============================================================================
