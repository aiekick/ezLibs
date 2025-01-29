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

// ezSvg is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

/*
#include "ezsvg.h"
#include <iostream>

int main() {
    ez::SVG svg("1024", "768");

    // Ajouter un dégradé linéaire
    svg.addLinearGradient("gradient1", {
        {"rgba(255,0,0,1)", 0.0},
        {"rgba(0,255,0,1)", 0.5},
        {"rgba(0,0,255,1)", 1.0}
    });

    // Ajouter un rectangle avec un dégradé
    svg.addRectangleWithGradient(50, 50, 300, 150, "gradient1");

    // Ajouter un cercle avec une couleur RGBA
    svg.addCircle(500, 200, 50, "rgba(0,0,255,0.5)", "rgba(255,0,0,0.8)", 3);

    // Exporter le fichier SVG
    try {
        svg.exportToFile("output_with_gradient.svg");
        std::cout << "SVG exported successfully to 'output_with_gradient.svg'" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

*/

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "ezVec2.hpp"
#include "ezXml.hpp"

namespace ez {
namespace img {

class Svg {
private:
    ez::uvec2 m_size{800};
    std::vector<ez::xml::Node> m_elements;
    std::vector<ez::xml::Node> m_gradients;

public:
    Svg(const ez::uvec2& vSize = 800U) : m_size(vSize) {}

    void addRectangle(  //
        const ez::uvec2& vMin,
        const ez::uvec2& vMax,
        const std::string& fillColor = "none",
        const std::string& strokeColor = "black",
        int strokeWidth = 1) {
        ez::xml::Node node("rect");  //
        node.addAttribute("x", vMin.x)  //
            .addAttribute("y", vMin.y)  //
            .addAttribute("width", vMax.x)  //
            .addAttribute("height", vMax.y)
            .addAttribute("style", m_generateStyle(fillColor, strokeColor, strokeWidth));
        m_elements.push_back(node);
    }

    void addText(  //
        const ez::uvec2& vPos,
        const std::string& text,
        const std::string& color = "black",
        int32_t fontSize = 16) {
        ez::xml::Node node("text");
        node.addAttribute("x", vPos.x)  //
            .addAttribute("y", vPos.y)  //
            .addAttribute("fill", color)  //
            .addAttribute("font-size", fontSize)  //
            .setContent(text);
        m_elements.push_back(node);
    }

    void addCircle(  //
        const ez::uvec2& vPos,
        int32_t vRadius,
        const std::string& fillColor = "none",
        const std::string& strokeColor = "black",
        int32_t strokeWidth = 1) {
        ez::xml::Node node("circle");
        node.addAttribute("cx", vPos.x)  //
            .addAttribute("cy", vPos.y)  //
            .addAttribute("r", vRadius)  //
            .addAttribute("style", m_generateStyle(fillColor, strokeColor, strokeWidth));
        m_elements.push_back(node);
    }

    void addLine(  //
        const ez::uvec2& vPos1,  //
        const ez::uvec2& vPos2,  //
        const std::string& strokeColor = "black",
        int32_t strokeWidth = 1) {
        ez::xml::Node node("line");
        node.addAttribute("x1", vPos1.x)  //
            .addAttribute("y1", vPos1.y)  //
            .addAttribute("x2", vPos2.x)  //
            .addAttribute("y2", vPos2.y)  //
            .addAttribute("style", m_generateStyle(strokeColor, strokeWidth));
        m_elements.push_back(node);
    }

    void addLinearGradient(  //
        const std::string& id,
        const std::vector<std::pair<std::string, float>>& stops) {
        ez::xml::Node node("linearGradient");
        node.addAttribute("id", id);
        for (const auto& stop : stops) {
            node.addChild("stop")  //
             // .addAttribute("offset") << stop.second * 100.0f << "%";
                .addAttribute("offset", ez::str::toStr("%f%", stop.second * 100.0f))  //
                .addAttribute("style", ez::str::toStr("stop-color:%s;", stop.first.c_str()));
        }
        m_gradients.push_back(node);
    }

    // Utiliser un dégradé dans les formes
    void addRectangleWithGradient(//
        const ez::uvec2& vMin,
        const ez::uvec2& vMax,
        const std::string& gradientId,
        const std::string& strokeColor = "black",
        int32_t strokeWidth = 1) {
        ez::xml::Node node("rect");  //
        node.addAttribute("x", vMin.x)  //
            .addAttribute("y", vMin.y)  //
            .addAttribute("width", vMax.x)  //
            .addAttribute("height", vMax.y)
            .addAttribute("style", m_generateStyle("url(#" + gradientId + ")", strokeColor, strokeWidth));  //
        m_elements.push_back(node);
    }

    void exportToFile(const std::string& filename) {
        std::ofstream file(filename);
        if (file.is_open()) {
            ez::xml::Node node("svg");
            node.addAttribute("xmlns", "http://www.w3.org/2000/svg")  //
                .addAttribute("width", m_size.x)  //
                .addAttribute("height", m_size.y);

            // Ajouter les dégradés
            if (!m_gradients.empty()) {
                auto& defs_node = node.addChild("defs");
                for (const auto& gradient_node : m_gradients) {
                    defs_node.addChild(gradient_node);
                }
            }

            // Ajouter les éléments
            for (const auto& elem_node : m_elements) {
                node.addChild(elem_node);
            }

            file << node.dump();

            file.close();
        }
    }

private:
    std::string m_generateStyle(const std::string& strokeColor, int strokeWidth) {
        std::ostringstream style;
        style << "stroke:" << strokeColor << ";"
              << "stroke-width:" << strokeWidth << ";";
        return style.str();
    }

    std::string m_generateStyle(const std::string& fillColor, const std::string& strokeColor, int strokeWidth) {
        std::ostringstream style;
        style << "fill:" << fillColor << ";"
              << "stroke:" << strokeColor << ";"
              << "stroke-width:" << strokeWidth << ";";
        return style.str();
    }
};

}  // namespace img
}  // namespace ez
