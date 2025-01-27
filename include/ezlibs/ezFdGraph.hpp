#pragma once

/*
MIT License

Copyright (c) 2014-2024 Stephane Cuillerdier (aka aiekick)

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

// ezFdGraph is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <cmath>

#include "ezVec2.hpp"
#include "ezCnt.hpp"

namespace ez {

class FdGraph {
public:
    typedef void* UserDatas;
    struct NodeDatas {
        ez::fvec2 pos;
        ez::fvec2 force;
        float mass{1.0f};
        uint32_t connCount{0};
        UserDatas userDatas = nullptr;
        NodeDatas() = default;
        NodeDatas(const ez::fvec2& vPos, const ez::fvec2& vForce, float vMass) : pos(vPos), force(vForce), mass(vMass) {}
    };
    class Node {
    private:
        std::shared_ptr<NodeDatas> mp_NodeDatas;

    public:
        template <typename T = NodeDatas>
        explicit Node(const T& vDatas) : mp_NodeDatas(std::make_shared<T>(vDatas)) {
            static_assert(std::is_base_of<NodeDatas, T>::value, "T must derive of ez::FdGraph::NodeDatas");
        }

        template <typename T = NodeDatas>
        const T& getDatas() const {
            // remove the need to use a slow dynamic_cast
            static_assert(std::is_base_of<NodeDatas, T>::value, "T must derive of ez::FdGraph::NodeDatas");
            return static_cast<const T&>(*mp_NodeDatas);
        }

        template <typename T = NodeDatas>
        T& getDatasRef() {
            // remove the need to use a slow dynamic_cast
            static_assert(std::is_base_of<NodeDatas, T>::value, "T must derive of ez::FdGraph::NodeDatas");
            return static_cast<T&>(*mp_NodeDatas);
        }

        virtual void update() {  //
            if (getDatas().mass > 0.0f) {
                getDatasRef().pos += getDatas().force / getDatas().mass;
            }
        }
    };

    typedef std::shared_ptr<Node> NodePtr;
    typedef std::weak_ptr<Node> NodeWeak;

    struct LinkDatas {};
    class Link {
    private:
        std::shared_ptr<LinkDatas> mp_LinkDatas;
        NodeWeak m_from;
        NodeWeak m_to;

    public:
        template <typename T = LinkDatas>
        explicit Link(const NodeWeak& vFrom, const NodeWeak& vTo, const T& vDatas = {}) : m_from(vFrom), m_to(vTo), mp_LinkDatas(std::make_shared<T>(vDatas)) {
            static_assert(std::is_base_of<LinkDatas, T>::value, "T must derive of ez::FdGraph::LinkDatas");
        }

        template <typename T = LinkDatas>
        const T& getDatas() const {
            // remove the need to use a slow dynamic_cast
            static_assert(std::is_base_of<LinkDatas, T>::value, "T must derive of ez::FdGraph::LinkDatas");
            return static_cast<const T&>(*mp_LinkDatas);
        }

        template <typename T = LinkDatas>
        T& getDatasRef() {
            // remove the need to use a slow dynamic_cast
            static_assert(std::is_base_of<LinkDatas, T>::value, "T must derive of ez::FdGraph::LinkDatas");
            return static_cast<T&>(*mp_LinkDatas);
        }

        const NodeWeak& getFromNode() const { return m_from; }
        const NodeWeak& getToNode() const { return m_to; }
    };

public:
    static ez::fvec2 randomPosition() {  //
        return ez::fvec2(
            static_cast<float>(rand() % 100),  //
            static_cast<float>(rand() % 100));
    }

private:
    std::vector<NodePtr> m_nodes;
    std::vector<Link> m_links;

    struct Config {
        float centralGravityFactor = 1.1f;
        float forceFactor = 1000.0f;
        float deltaTimeFactor = 2.0f;
    } m_config;

public:
    template <typename T = Node, typename U = NodeDatas>
    std::weak_ptr<T> addNode(const U& vDatas) {
        static_assert(std::is_base_of<Node, T>::value, "T must derive of ez::FdGraph::Node");
        static_assert(std::is_base_of<NodeDatas, U>::value, "U must derive of ez::FdGraph::NodeDatas");
        auto ptr = std::make_shared<T>(vDatas);
        m_nodes.push_back(ptr);
        return ptr;
    }

    void addLink(const NodeWeak& nA, const NodeWeak& nB) {  //
        if (!nA.expired() && !nB.expired()) {
            m_links.push_back(Link(nA, nB));
            nA.lock()->getDatasRef().connCount += 1;
            nB.lock()->getDatasRef().connCount += 1;
        }
    }

    void clear() {
        m_links.clear();
        m_nodes.clear();
    }

    const Config& getConfig() const { return m_config; }
    Config& getConfigRef() { return m_config; }

    void updateForces(float vDeltaTime) {
        const float deltaTime = vDeltaTime * m_config.deltaTimeFactor;

        // gravity
        for (auto& node_ptr : m_nodes) {
            node_ptr->getDatasRef().force = node_ptr->getDatas().pos * -m_config.centralGravityFactor * deltaTime;
        }

        // repulsion between nodes
        for (auto& node_a_ptr : m_nodes) {
            for (auto& node_b_ptr : m_nodes) {
                if (node_a_ptr != node_b_ptr) {
                    auto dir = node_b_ptr->getDatas().pos - node_a_ptr->getDatas().pos;
                    if (dir.emptyAND()) {
                        dir = 0.01f;
                    }
                    auto force = dir * m_config.forceFactor / ez::dot(dir, dir);
                    node_a_ptr->getDatasRef().force -= force * deltaTime;
                    node_b_ptr->getDatasRef().force += force * deltaTime;
                }
            }
        }

        // attraction between connected nodes
        for (auto& link : m_links) {
            auto node_a_ptr = link.getFromNode().lock();
            auto node_b_ptr = link.getToNode().lock();
            if (node_a_ptr != node_b_ptr) {
                auto div = node_a_ptr->getDatas().pos - node_b_ptr->getDatas().pos;
                node_a_ptr->getDatasRef().force -= div * deltaTime;
                node_b_ptr->getDatasRef().force += div * deltaTime;
            }
        }

        // update forces
        for (const auto& node_ptr : m_nodes) {
            node_ptr->update();
        }
    }

    const std::vector<NodePtr>& getNodes() const { return m_nodes; }
    std::vector<NodePtr>& getNodesRef() { return m_nodes; }

    const std::vector<Link>& getLinks() const { return m_links; }
    std::vector<Link>& getLinksRef() { return m_links; }
};

}  // namespace ez
