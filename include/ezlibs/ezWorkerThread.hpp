#pragma once

/*
MIT License

Copyright (c) 2014-2025 Stephane Cuillerdier (aka aiekick)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// ezWorkerThread is part of the ezLibs project : https://github.com/aiekick/ezLibs.git
#include <mutex>
#include <atomic>
#include <thread>
#include <string>
#include <vector>
#include <chrono>
#include <functional>
#include <chrono>
#include <memory>
#include <typeindex>

namespace ez {
namespace thread {


class Worker {
public:
    typedef std::atomic<float> AtomicFloat;
    typedef std::atomic<bool> AtomicBool;
    class WorkerIO {
    public:
        WorkerIO(Worker& vSelf, AtomicFloat& vProgress, AtomicBool& vWorking, AtomicFloat& vGenerationTime)
            : m_self(vSelf), m_progress(vProgress), m_working(vWorking), m_generationTime(vGenerationTime) {
            m_baseTime = std::chrono::high_resolution_clock::now();
        }

        // Non copiable (tient des références)
        WorkerIO(const WorkerIO&) = delete;
        WorkerIO& operator=(const WorkerIO&) = delete;

        void setProgress(float v) {
            fixTime();
            m_progress.store(v, std::memory_order_relaxed);
            if (v == 1.0f) { // for let the user see the 100% on the progressbar, 100 ms is 6 frames at 60 fps
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        void addProgress(float dv) {
            fixTime();
            m_atomicAdd(m_progress, dv);
        }

        void run() { m_working.store(true, std::memory_order_relaxed); }
        void stop() { m_working.store(false, std::memory_order_relaxed); }
        bool isRunning() const { return m_working.load(std::memory_order_relaxed); }

        void setElapsed(float s) { m_generationTime.store(s, std::memory_order_relaxed); }
        void fixTime() {
            m_generationTime.store(  //
                std::chrono::duration<float>(now() - m_baseTime).count(),
                std::memory_order_relaxed);
        }
        
        template <typename T>
        T& get() {
            return m_self.m_get<T>();
        }

        template <typename T>
        const T& get() const {
            return m_self.m_get<T>();
        }
        
        void setCurrentPhase(const std::string& vPhase) {  //
            fixTime();
            m_self.m_setCurrentPhase(vPhase);
        }
        
        void setCurrentStepHeader(const std::string& vHeader) {  //
            fixTime();
            m_self.m_setCurrentStepHeader(vHeader);
        }

        void setCurrentStepMessage(const std::string& vMessage) {  //
            fixTime();
            m_self.m_setCurrentStepMessage(vMessage);
        }

        std::mutex& getMutexRef() { return m_self.m_mutex; }

    private:
        static std::chrono::steady_clock::time_point now() { return std::chrono::high_resolution_clock::now(); }
        // fetch_add available only for integral in cpp11 (available for float only in cpp20)
        static void m_atomicAdd(AtomicFloat& target, float value) {  // CAS for cpp11
            float old = target.load(std::memory_order_relaxed);
            float desired;
            do {
                desired = old + value;
            } while (!target.compare_exchange_weak(  //
                old,
                desired,
                std::memory_order_relaxed,
                std::memory_order_relaxed));
        }

    private:
        Worker& m_self;
        AtomicFloat& m_progress;        // [0..1]
        AtomicBool& m_working;          // true tant que le worker doit continuer
        AtomicFloat& m_generationTime;  // secondes
        std::chrono::steady_clock::time_point m_baseTime;
    };

    class BaseIO {
    public:
        BaseIO(Worker& vSelf) : m_self(vSelf) {}

        // Non copiable (tient des références)
        BaseIO(const BaseIO&) = delete;
        BaseIO& operator=(const BaseIO&) = delete;

        template <typename T>
        T& get() {
            return m_self.m_get<T>();
        }

        template <typename T>
        const T& get() const {
            return m_self.m_get<T>();
        }
    private:
        Worker& m_self;
    };
    
    using BaseFunctor = std::function<void(BaseIO&)>;
    using WorkerFunctor = std::function<void(WorkerIO&)>;

protected:
    AtomicFloat aProgress;  // [0.0:1.0]
    AtomicBool aWorking;
    AtomicFloat aGenerationTime;  // secondes

private:
    mutable std::mutex m_mutex; // we can have the mutex in const methods
    std::thread m_workerThread;
    BaseFunctor m_finishFunc;
    BaseFunctor m_cancelFunc;
    WorkerFunctor m_workerFunc;
    std::string m_taskTitle;

    // Données possédées par le thread (type effacé)
    std::shared_ptr<void> m_payload;  // propriétaire
    std::type_index m_payloadType{typeid(void)};

    // thread shared data (need a mutex lock)
    std::string m_currentStepHeader;
    std::string m_currentStepMessage;
    std::string m_currentPhase;

public:
    Worker() : aProgress(0.0f), aWorking(false), aGenerationTime(0.0f) {}

    // Démarrage avec transfert de propriété des datas au thread
    template <typename T>
    void start(
        const std::string& vTaskTitle,
        T vData,                       // passé par valeur : sera mové/copîé dans m_payload
        WorkerFunctor vWorkerFunctor,  // (this, progress, working, generationTime)
        BaseFunctor vFinishFunctor = nullptr,
        BaseFunctor vCancelFunctor = nullptr) {
        if (!stop()) {
            m_taskTitle = vTaskTitle;
            m_finishFunc = std::move(vFinishFunctor);
            m_cancelFunc = std::move(vCancelFunctor);
            m_workerFunc = std::move(vWorkerFunctor);

            // Possession des datas par le thread
            m_payload = std::shared_ptr<void>(new T(std::move(vData)), [](void* p) { delete static_cast<T*>(p); });
            m_payloadType = std::type_index(typeid(T));

            aWorking = true;
            aProgress = 0.0f;
            aGenerationTime = 0.0f;

            m_workerThread = std::thread([this]() {
                const auto t0 = std::chrono::high_resolution_clock::now();

                if (m_workerFunc) {
                    WorkerIO io{*this, aProgress, aWorking, aGenerationTime};
                    m_workerFunc(io);
                }

                const auto t1 = std::chrono::high_resolution_clock::now();
                aGenerationTime = std::chrono::duration<float>(t1 - t0).count();
                aWorking = false;
            });
        }
    }

    bool stop() {
        const bool res = isRunning();
        if (res) {
            aWorking = false;  // must be done before join
            destroy();
            if (m_finishFunc) {
                BaseIO io{*this};
                m_finishFunc(io);
            }
        }
        return res;
    }

    bool cancel() {
        const bool res = isRunning();
        if (res) {
            aWorking = false;  // must be done before join
            destroy();
            if (m_cancelFunc) {
                BaseIO io{*this};
                m_cancelFunc(io);
            }
        }
        return res;
    }

    bool isRunning() const { return m_workerThread.joinable(); }

    void destroy() {
        if (m_workerThread.joinable()) {
            if (std::this_thread::get_id() == m_workerThread.get_id()) {
                // Ne jamais join() sur soi-même : détacher pour éviter le deadlock.
                m_workerThread.detach();
            } else {
                m_workerThread.join();
            }
        }
    }

    void finishIfNeeded() {
        if (isRunning() && (!aWorking)) {
            destroy();
            if (m_finishFunc) {
                BaseIO io{*this};
                m_finishFunc(io);
            }
        }
    }

    std::mutex& getMutexRef() { return m_mutex; }

#ifdef IMGUI_API
    void drawDialog(const ImVec2& vPos, const float vItemWidth, const float vItemAlign) {
        if (aWorking) {
            ImGui::OpenPopup(m_taskTitle.c_str());
            ImGui::SetNextWindowPos(vPos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            if (ImGui::BeginPopupModal(                      //
                    m_taskTitle.c_str(),                     //
                    (bool*)nullptr,                          //
                    ImGuiWindowFlags_NoTitleBar |            //
                        ImGuiWindowFlags_NoResize |          //
                        ImGuiWindowFlags_AlwaysAutoResize |  //
                        ImGuiWindowFlags_NoDocking)) {
                ImGui::Header(m_taskTitle.c_str());
                ImGui::Separator();
                m_drawDialogPhase(vItemWidth, vItemAlign);
                m_drawDialogProgressBar(vItemWidth, vItemAlign);
                ImGui::Separator();
                if (ImGui::ContrastedButton("Stop")) {
                    cancel();
                }
                ImGui::EndPopup();
            }
        }
    }
    void drawStatusBar() {
        if (aWorking) {
            ImGui::Text("%s", m_taskTitle.c_str());
            m_drawStatusBarPhase();
            m_drawStatusBarProgressBar();
            if (ImGui::SmallContrastedButton("Stop")) {
                cancel();
            }        
        }
    }
#endif

private:
    template <typename T>
    T& m_get() {
        if (!m_payload || m_payloadType != std::type_index(typeid(T))) {
            throw std::bad_cast();
        }
        return *static_cast<T*>(m_payload.get());
    }

    template <typename T>
    const T& m_get() const {
        if (!m_payload || m_payloadType != std::type_index(typeid(T))) {
            throw std::bad_cast();
        }
        return *static_cast<const T*>(m_payload.get());
    }
#ifdef IMGUI_API
    void m_setCurrentStepHeader(const std::string& vHeader) {
        std::lock_guard<std::mutex> _{m_mutex};
        m_currentStepHeader = vHeader;
    }
    void m_setCurrentStepMessage(const std::string& vMessage) {
        std::lock_guard<std::mutex> _{m_mutex};
        m_currentStepMessage = vMessage;
    }
    void m_setCurrentPhase(const std::string& vPhase) {
        std::lock_guard<std::mutex> _{m_mutex};
        m_currentPhase = vPhase;
    }
    void m_drawDialogPhase(const float vItemWidth, const float vItemAlign) const {
        std::lock_guard<std::mutex> _{m_mutex};
        if (!m_currentPhase.empty()) {
            ImGui::DisplayAlignedWidget(vItemWidth, "Phase", vItemAlign, [this]() {  //
                ImGui::Text("%s", m_currentPhase.c_str());
            });
        }
        if (!m_currentStepMessage.empty()) {
            ImGui::DisplayAlignedWidget(vItemWidth, m_currentStepHeader.c_str(), vItemAlign, [this]() {  //
                ImGui::TextWrapped("%s", m_currentStepMessage.c_str());
            });
        }
    }
    void m_drawStatusBarPhase() {
        std::lock_guard<std::mutex> _{m_mutex};
        if (!m_currentPhase.empty()) {
            ImGui::Separator();
            ImGui::Text("Phase %s", m_currentPhase.c_str());
        }
        if (!m_currentStepMessage.empty()) {
            ImGui::Separator();
            ImGui::TextWrapped("%s %s", m_currentStepHeader.c_str(), m_currentStepMessage.c_str());
        }
    }

    // no mutex needed, since use atomic vars
    void m_drawDialogProgressBar(const float vItemWidth, const float vItemAlign) const {
        ImGui::DisplayAlignedWidget(vItemWidth, "Progres", vItemAlign, [this]() {  //
            float progress = aProgress;                                            // attendu entre 0.0 et 1.0
            float elapsed_time = aGenerationTime;
            const auto txt = ez::str::toStr("%.3f s", elapsed_time);
            ImGui::ProgressBar(progress, ImVec2(300.0f, 0.0f), txt.c_str());
        });
    }
    void m_drawStatusBarProgressBar() {
        float progress = aProgress;  // attendu entre 0.0 et 1.0
        float elapsed_time = aGenerationTime;
        const auto txt = ez::str::toStr("%.3f s", elapsed_time);
        ImGui::Separator();
        ImGui::ProgressBar(progress, ImVec2(300.0f, 0.0f), txt.c_str());
    }
#endif
};

}  // namespace thread
}  // namespace ez
