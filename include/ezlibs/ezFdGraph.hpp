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
private:
    struct Node {
        std::string tag;
        ez::fvec2 position;
        ez::fvec2 velocity;
    };

    typedef std::shared_ptr<Node> NodePtr;
    typedef std::weak_ptr<Node> NodeWeak;

    struct Link {
        NodeWeak from;
        NodeWeak to;
    };

    std::vector<NodePtr> m_nodes;
    std::vector<Link> m_links;
    float m_repulsionStrength = 100.0f;
    float m_attractionStrength = 0.1f;

public:
    Node& addNode() {
        auto ptr = std::make_shared<Node>();
        ptr->position = randomPosition();
        m_nodes.push_back(ptr);
        return *ptr.get();
    }

    void addLink(const NodeWeak& nA, const NodeWeak& nB) {  //
        m_links.push_back({nA, nB});
    }

    void update(float deltaTime) {
        // Apply repulsion
        for (auto& node_a_ptr : m_nodes) {
            for (auto& node_b_ptr : m_nodes) {
                if (node_a_ptr == node_b_ptr) {
                    continue;
                }

                ez::fvec2 diff = node_a_ptr->position - node_b_ptr->position;
                float distance = diff.length() + 1.0f;  // Avoid division by zero
                ez::fvec2 repulsionForce = diff.normalize() * (m_repulsionStrength / (distance * distance));
                node_a_ptr->velocity = node_a_ptr->velocity + repulsionForce * deltaTime;
            }
        }

        // Apply attraction
        for (const auto& link : m_links) {
            auto node_from_ptr = link.from.lock();
            auto node_to_ptr = link.to.lock();

            ez::fvec2 diff = node_to_ptr->position - node_to_ptr->position;
            ez::fvec2 attractionForce = diff * m_attractionStrength;
            node_to_ptr->velocity = node_to_ptr->velocity + attractionForce * deltaTime;
            node_to_ptr->velocity = node_to_ptr->velocity - attractionForce * deltaTime;
        }

        // Update positions
        for (auto& node_ptr : m_nodes) {
            node_ptr->position = node_ptr->position + node_ptr->velocity * deltaTime;
            node_ptr->velocity = node_ptr->velocity * 0.9f;  // Damping
        }
    }

    const std::vector<NodePtr>& getNodes() const { return m_nodes; }

    const std::vector<Link>& getLinks() const { return m_links; }

private:
    ez::fvec2 randomPosition() const { return ez::fvec2(static_cast<float>(rand() % 100), static_cast<float>(rand() % 100)); }
};

}  // namespace ez
