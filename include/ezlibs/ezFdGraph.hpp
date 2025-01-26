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
    class Node {
    private:
        uint32_t connCount{0};

    public:
        std::string tag;
        ez::fvec2 pos;
        ez::fvec2 force;
        float mass{1.0f};
        float radius{2.0f};

        void update() {  //
            if (mass > 0.0f) {
                pos += force / mass;
            }
        }
        void addConn(const uint32_t vCount = 1U) {  //
            connCount += vCount;
            mass = 5.0f * connCount;
            radius = 2.0f * connCount;
        }
    };

    typedef std::shared_ptr<Node> NodePtr;
    typedef std::weak_ptr<Node> NodeWeak;

    struct Link {
        NodeWeak from;
        NodeWeak to;
    };

private:

    std::vector<NodePtr> m_nodes;
    std::vector<Link> m_links;

    float m_centralGravityFactor = 1.1f;
    float m_forceFactor = 1000.0f;

public:
    NodeWeak addNode() {
        auto ptr = std::make_shared<Node>();
        ptr->pos = randomPosition();
        m_nodes.push_back(ptr);
        return ptr;
    }

    void addLink(const NodeWeak& nA, const NodeWeak& nB) {  //
        if (!nA.expired() && !nB.expired()) {
            m_links.push_back({nA, nB});
            nA.lock()->addConn();
            nB.lock()->addConn();
        }
    }

    void setForceFactor(float vFactor = 1000.0f) { m_forceFactor = vFactor; }
    void setCentralGravityFactor(float vFactor = 1.1f) { m_centralGravityFactor = vFactor; }

    void updateForces(float deltaTime) {
        // gravity (for let the graph around the center
        for (const auto& node_ptr : m_nodes) {
            node_ptr->force = node_ptr->pos * -m_centralGravityFactor * deltaTime;
        }

        // apply repulsive force between nodes
        for (const auto& node_a_ptr : m_nodes) {
            for (const auto& node_b_ptr : m_nodes) {
                if (node_a_ptr != node_b_ptr) {
                    auto dir = node_b_ptr->pos - node_a_ptr->pos;
                    auto force = dir * m_forceFactor / ez::dot(dir, dir);
                    node_a_ptr->force -= force * deltaTime;
                    node_b_ptr->force += force * deltaTime;
                }
            }
        }

        // apply forces applied by connections
        for (const auto& link : m_links) {
            auto node_a_ptr = link.from.lock();
            auto node_b_ptr = link.to.lock();
            if (node_a_ptr != node_b_ptr) {
                auto div = node_a_ptr->pos - node_b_ptr->pos;
                node_a_ptr->force -= div * deltaTime;
                node_b_ptr->force += div * deltaTime;
            }
        }

        // update forces
        for (const auto& node_ptr : m_nodes) {
            node_ptr->update();
        }
    }

    const std::vector<NodePtr>& getNodes() const { return m_nodes; }

    const std::vector<Link>& getLinks() const { return m_links; }

private:
    ez::fvec2 randomPosition() const { return ez::fvec2(static_cast<float>(rand() % 100), static_cast<float>(rand() % 100)); }
};

}  // namespace ez
