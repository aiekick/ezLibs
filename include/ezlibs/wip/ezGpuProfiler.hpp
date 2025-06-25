// ============================================================================
// gpu_profiler.hpp – header‑only GPU scope profiler (v1.1‑beta)
// ----------------------------------------------------------------------------
//  NEW IN 1.1
//  • ProfDesc lets you tune pool size & frames‑in‑flight
//  • beginScope / endScope early‑out when disabled ⇒ zéro GPU calls
//  • GL multi‑context already merged
//
//  QUICK SETUP (C++11)
//  -------------------
//      #define GPU_PROFILER_HEADER_ONLY
//      #define GPU_PROFILER_BACKEND_OPENGL       // or _VULKAN
//      #define OPENGL_LOADER <GL/glew.h>         // or glad
//      #include "gpu_profiler.hpp"
// ============================================================================

#pragma once

// ─────────────────── Backend selection & loaders ────────────────────────────
#if !defined(GPU_PROFILER_BACKEND_OPENGL) && !defined(GPU_PROFILER_BACKEND_VULKAN)
#error "Define GPU_PROFILER_BACKEND_OPENGL or GPU_PROFILER_BACKEND_VULKAN before including"
#endif
#ifdef GPU_PROFILER_BACKEND_OPENGL
#ifndef OPENGL_LOADER
#error "Define OPENGL_LOADER (<GL/glew.h> / glad / …)"
#endif
#include OPENGL_LOADER
#endif
#ifdef GPU_PROFILER_BACKEND_VULKAN
#ifndef VULKAN_LOADER
#error "Define VULKAN_LOADER (\"volk.h\" or <vulkan/vulkan.h>)"
#endif
#include VULKAN_LOADER
#endif

#include <cstdint>
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <cassert>

#ifdef IMGUI_INCLUDE
#include IMGUI_INCLUDE
#endif

namespace gp {

// ───────────────────────────── Config struct ────────────────────────────────
struct ProfDesc {
    uint32_t queriesPerFrame = 8192;
    uint32_t framesInFlight = 3;  // Vulkan only
};

// ─────────────────────────── Small POD types ────────────────────────────────
struct ScopeID {
    uint32_t v{0};
};
struct Node {
    std::string label;
    uint64_t t0 = 0, t1 = 0;
    std::vector<Node> children;
};
struct FrameResults {
    bool valid = false;
    std::vector<Node> tree;
    double nsPerTick = 1.0;
};

struct Stat {
    uint64_t min{UINT64_MAX}, max{0}, sum{0};
    uint32_t cnt{0};
    void add(uint64_t d) {
        min = std::min(min, d);
        max = std::max(max, d);
        sum += d;
        ++cnt;
    }
    double avg() const { return cnt ? double(sum) / cnt : 0.0; }
};
using StatMap = std::unordered_map<std::string, Stat>;

inline void updateStats(StatMap& m, const FrameResults& fr) {
    if (!fr.valid)
        return;
    std::function<void(const Node&)> rec = [&](const Node& n) {
        m[n.label].add(uint64_t((n.t1 - n.t0) * fr.nsPerTick));
        for (const auto& c : n.children)
            rec(c);
    };
    for (const auto& r : fr.tree)
        rec(r);
}

// ───────────────────────── Backend contexts ─────────────────────────────────
#ifdef GPU_PROFILER_BACKEND_OPENGL
using QueryT = GLuint;
struct BackendCtx { /* GL : empty */
};
#elif defined(GPU_PROFILER_BACKEND_VULKAN)
using QueryT = uint32_t;
struct BackendCtx {
    uint32_t frames = 0, poolSize = 0;
    VkDevice dev{};
    VkQueue queue{};
    uint32_t qFam{};
    float periodNs{1.f};
    std::vector<VkQueryPool> pools;
    std::vector<uint32_t> cursor;
    uint32_t frame = 0;
    VkCommandPool cmdPool{};
    VkCommandBuffer cmd{};
};
#endif
struct QueryHandle {
    QueryT start{}, end{};
};

// ───────────────────────── Backend interface template ───────────────────────
template <class CTX>
struct Iface;  // primary

#ifdef GPU_PROFILER_BACKEND_OPENGL
// ---- OpenGL ----
template <>
struct Iface<BackendCtx> {
    static void* curCtx() {
#if defined(_WIN32) return (void*)wglGetCurrentContext();
#elif defined(__linux__) return (void*)glXGetCurrentContext();
#else return nullptr;
#endif
    }
    struct GLPool {
        std::vector<QueryT> free;
    };
    static std::unordered_map<void*, GLPool>& pools() {
        static std::unordered_map<void*, GLPool> s;
        return s;
    }
    static QueryHandle alloc(BackendCtx&) {
        void* c = curCtx();
        auto& p = pools()[c];
        QueryHandle h;
        if (p.free.size() >= 2) {
            h.start = p.free.back();
            p.free.pop_back();
            h.end = p.free.back();
            p.free.pop_back();
        } else {
            glGenQueries(1, &h.start);
            glGenQueries(1, &h.end);
        }
        return h;
    }
    static void free(BackendCtx&, QueryHandle h) {
        pools()[curCtx()].free.push_back(h.start);
        pools()[curCtx()].free.push_back(h.end);
    }
    static void begin(BackendCtx&, const QueryHandle& h) { glQueryCounter(h.start, GL_TIMESTAMP); }
    static void end(BackendCtx&, const QueryHandle& h) { glQueryCounter(h.end, GL_TIMESTAMP); }
    static bool fetch(BackendCtx&, const QueryHandle& h, uint64_t& s, uint64_t& e) {
        GLuint a = 0;
        glGetQueryObjectuiv(h.end, GL_QUERY_RESULT_AVAILABLE, &a);
        if (!a)
            return false;
        glGetQueryObjectui64v(h.start, GL_QUERY_RESULT, &s);
        glGetQueryObjectui64v(h.end, GL_QUERY_RESULT, &e);
        return true;
    }
    static constexpr double nsPerTick(const BackendCtx&) { return 1.0; }
};
#endif

#ifdef GPU_PROFILER_BACKEND_VULKAN
// ---- Vulkan ----
template <>
struct Iface<BackendCtx> {
    static QueryHandle alloc(BackendCtx& c) {
        uint32_t& cur = c.cursor[c.frame];
        assert(cur + 2 <= c.poolSize);
        QueryHandle h{cur, cur + 1};
        cur += 2;
        return h;
    }
    static void free(BackendCtx&, QueryHandle) {}
    static void begin(BackendCtx& c, const QueryHandle& h) { vkCmdWriteTimestamp(c.cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, c.pools[c.frame], h.start); }
    static void end(BackendCtx& c, const QueryHandle& h) { vkCmdWriteTimestamp(c.cmd, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, c.pools[c.frame], h.end); }
    static bool fetch(BackendCtx& c, const QueryHandle& h, uint64_t& s, uint64_t& e) {
        uint64_t r[2];
        VkResult rs = vkGetQueryPoolResults(
            c.dev,
            c.pools[(c.frame + c.frames - 1) % c.frames],
            h.start,
            2,
            sizeof(r),
            r,
            sizeof(uint64_t),
            VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WITH_AVAILABILITY_BIT);
        if (rs != VK_SUCCESS)
            return false;
        s = r[0];
        e = r[1];
        return true;
    }
    static double nsPerTick(const BackendCtx& c) { return c.periodNs; }
};
#endif

// ─────────────────────────────── Profiler ───────────────────────────────────
class Profiler {
public:
    // GL ctor
#ifdef GPU_PROFILER_BACKEND_OPENGL
    explicit Profiler(const ProfDesc& d = ProfDesc{}) : m_cfg(d) {}
#endif
#ifdef GPU_PROFILER_BACKEND_VULKAN
    Profiler(const ProfDesc& d, VkDevice dev, VkQueue q, uint32_t fam, float periodNs) { initVk(d, dev, q, fam, periodNs); }
    void setCommandBuffer(VkCommandBuffer cmd) { m_ctx.cmd = cmd; }
#endif
    ~Profiler() { release(); }

    // frame
    void beginFrame() {
        if (!m_enabled)
            return;
        m_curr.clear();
        while (!m_stack.empty())
            m_stack.pop();
#ifdef GPU_PROFILER_BACKEND_VULKAN
        m_ctx.frame = (m_ctx.frame + 1) % m_ctx.frames;
        m_ctx.cursor[m_ctx.frame] = 0;
        vkResetQueryPool(m_ctx.dev, m_ctx.pools[m_ctx.frame], 0, m_ctx.poolSize);
#endif
    }
    void endFrame() {
        if (!m_enabled)
            return;
        m_hist.push_back(std::move(m_curr));
        if (m_hist.size() > m_histMax)
            m_hist.erase(m_hist.begin());
    }

    // scopes
    ScopeID beginScope(const char* lbl) {
        if (!m_enabled)
            return {};
        Zone z;
        z.label = lbl;
        z.parent = m_stack.empty() ? UINT32_MAX : m_stack.top();
        if (m_enabled) {
            z.q = Iface<BackendCtx>::alloc(m_ctx);
            Iface<BackendCtx>::begin(m_ctx, z.q);
        }
        uint32_t id = m_zones.size();
        m_zones.push_back(std::move(z));
        if (z.parent != UINT32_MAX)
            m_zones[z.parent].children.push_back(id);
        m_stack.push(id);
        return {id};
    }
    void endScope(ScopeID id) {
        if (!m_enabled)
            return;
        if (id.v >= m_zones.size())
            return;
        Iface<BackendCtx>::end(m_ctx, m_zones[id.v].q);
        if (!m_stack.empty() && m_stack.top() == id.v)
            m_stack.pop();
        m_curr.push_back(id.v);
    }

    // results
    FrameResults fetch() {
        FrameResults fr;
        if (!m_enabled || m_hist.empty())
            return fr;
        const auto& done = m_hist.back();
        fr.nsPerTick = Iface<BackendCtx>::nsPerTick(m_ctx);
        std::vector<Node*> map(m_zones.size(), nullptr);
        for (uint32_t id : done) {
            const Zone& z = m_zones[id];
            uint64_t s = 0, e = 0;
            if (!Iface<BackendCtx>::fetch(m_ctx, z.q, s, e))
                continue;
            fr.valid = true;
            m_tmp.push_back({z.label, s, e, {}});
            map[id] = &m_tmp.back();
        }
        for (uint32_t id : done) {
            if (!map[id])
                continue;
            const Zone& z = m_zones[id];
            if (z.parent == UINT32_MAX || !map[z.parent])
                fr.tree.emplace_back(std::move(*map[id]));
            else
                map[z.parent]->children.emplace_back(std::move(*map[id]));
        }
        m_tmp.clear();
        return fr;
    }

    // misc
    void setEnabled(bool b) { m_enabled = b; }
    bool isEnabled() const { return m_enabled; }

    // ---------- Chrome trace export ----------------------------------------
    static void dumpTrace(const FrameResults& fr, const char* filePath) {
        FILE* f = fopen(filePath, "w");
        if (!f)
            return;
        fprintf(f, "[\n");
        std::function<void(const Node&, int)> rec = [&](const Node& n, int tid) {
            uint64_t dur = uint64_t((n.t1 - n.t0) * fr.nsPerTick);
            fprintf(
                f,
                "{\"name\":\"%s\",\"ph\":\"X\",\"ts\":%llu,\"dur\":%llu,\"pid\":0,\"tid\":%d},\n",
                n.label.c_str(),
                (unsigned long long)(n.t0 * fr.nsPerTick),
                (unsigned long long)dur,
                tid);
            int cid = 0;
            for (const auto& c : n.children)
                rec(c, tid * 10 + cid++);
        };
        int id = 0;
        for (const auto& root : fr.tree)
            rec(root, id++);
        fseek(f, -2, SEEK_CUR);
        fprintf(f, "\n]\n");
        fclose(f);
    }

private:
    struct Zone {
        std::string label;
        QueryHandle q{};
        uint32_t parent{UINT32_MAX};
        std::vector<uint32_t> children;
    };

#ifdef GPU_PROFILER_BACKEND_VULKAN
    void initVk(const ProfDesc& d, VkDevice dev, VkQueue q, uint32_t fam, float period) {
        m_cfg = d;
        m_ctx.frames = d.framesInFlight;
        m_ctx.poolSize = d.queriesPerFrame;
        m_ctx.dev = dev;
        m_ctx.queue = q;
        m_ctx.qFam = fam;
        m_ctx.periodNs = period;
        m_ctx.cursor.resize(m_ctx.frames, 0);
        VkQueryPoolCreateInfo ci{VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO};
        ci.queryType = VK_QUERY_TYPE_TIMESTAMP;
        ci.queryCount = m_ctx.poolSize;
        m_ctx.pools.resize(m_ctx.frames);
        for (uint32_t i = 0; i < m_ctx.frames; ++i)
            vkCreateQueryPool(dev, &ci, nullptr, &m_ctx.pools[i]);
        VkCommandPoolCreateInfo cpi{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
        cpi.queueFamilyIndex = fam;
        cpi.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        vkCreateCommandPool(dev, &cpi, nullptr, &m_ctx.cmdPool);
    }
    void destroyVk() {
        for (auto p : m_ctx.pools)
            if (p)
                vkDestroyQueryPool(m_ctx.dev, p, nullptr);
        if (m_ctx.cmdPool)
            vkDestroyCommandPool(m_ctx.dev, m_ctx.cmdPool, nullptr);
    }
#else
    void destroyVk() {}
#endif
    void release() {
        for (auto& z : m_zones)
            Iface<BackendCtx>::free(m_ctx, z.q);
        m_zones.clear();
        m_hist.clear();
        m_tmp.clear();
        m_curr.clear();
        while (!m_stack.empty())
            m_stack.pop();
        destroyVk();
    }

    // data -------------------------------------------------------------------
    ProfDesc m_cfg{};
    BackendCtx m_ctx{};
    bool m_enabled{true};

    std::vector<Zone> m_zones;
    std::stack<uint32_t> m_stack;
    std::vector<uint32_t> m_curr;
    std::vector<std::vector<uint32_t>> m_hist;
    size_t m_histMax{3};
    std::vector<Node> m_tmp;
};

// ──────────────────────────── RAII helper & macros ──────────────────────────
struct ScopeGuard {
    Profiler* p;
    ScopeID id;
    ScopeGuard(Profiler* prof, const char* l) : p(prof) { id = p ? p->beginScope(l) : ScopeID{}; }
    ~ScopeGuard() {
        if (p)
            p->endScope(id);
    }
};
#ifndef GP_PROFILER_NO_MACROS
#define GP_PROF_INIT(Name, Desc, ...) static gp::Profiler Name(Desc, __VA_ARGS__)
#define GP_PROF_FRAME_BEGIN(P) P.beginFrame()
#define GP_PROF_FRAME_END(P) P.endFrame()
#define GP_PROF_SCOPE(P, LABEL) gp::ScopeGuard _gp_scope_##__LINE__(&(P), (LABEL))
#endif  // GP_PROFILER_NO_MACROS

}  // namespace gp

// ============================================================================
//  EOF
// ============================================================================
