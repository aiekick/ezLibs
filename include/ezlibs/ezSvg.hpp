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

namespace ez {
namespace img {

class Svg {
private:
    std::string width;
    std::string height;
    std::vector<std::string> elements;
    std::vector<std::string> gradients;  // Stockage des dégradés

    std::string generateStyle(const std::string& fillColor, const std::string& strokeColor, int strokeWidth) {
        std::ostringstream style;
        style << "fill:" << fillColor << ";"
              << "stroke:" << strokeColor << ";"
              << "stroke-width:" << strokeWidth << ";";
        return style.str();
    }

public:
    Svg(const std::string& width = "800", const std::string& height = "600") : width(width), height(height) {}

    void addRectangle(int x, int y, int w, int h, const std::string& fillColor = "none", const std::string& strokeColor = "black", int strokeWidth = 1) {
        std::ostringstream rect;
        rect << "<rect x=\"" << x << "\" y=\"" << y << "\" width=\"" << w << "\" height=\"" << h << "\" style=\"" << generateStyle(fillColor, strokeColor, strokeWidth)
             << "\" />";
        elements.push_back(rect.str());
    }

    void addText(int x, int y, const std::string& text, const std::string& color = "black", int fontSize = 16) {
        std::ostringstream txt;
        txt << "<text x=\"" << x << "\" y=\"" << y << "\" fill=\"" << color << "\" font-size=\"" << fontSize << "\">" << text << "</text>";
        elements.push_back(txt.str());
    }

    void addCircle(int cx, int cy, int r, const std::string& fillColor = "none", const std::string& strokeColor = "black", int strokeWidth = 1) {
        std::ostringstream circle;
        circle << "<circle cx=\"" << cx << "\" cy=\"" << cy << "\" r=\"" << r << "\" style=\"" << generateStyle(fillColor, strokeColor, strokeWidth) << "\" />";
        elements.push_back(circle.str());
    }

    void addLine(int x1, int y1, int x2, int y2, const std::string& strokeColor = "black", int strokeWidth = 1) {
        std::ostringstream line;
        line << "<line x1=\"" << x1 << "\" y1=\"" << y1 << "\" x2=\"" << x2 << "\" y2=\"" << y2 << "\" style=\"stroke:" << strokeColor << ";stroke-width:" << strokeWidth
             << ";\" />";
        elements.push_back(line.str());
    }

    // Ajouter un dégradé linéaire
    void addLinearGradient(const std::string& id, const std::vector<std::pair<std::string, float>>& stops) {
        std::ostringstream gradient;
        gradient << "<linearGradient id=\"" << id << "\">\n";
        for (const auto& stop : stops) {
            gradient << "  <stop offset=\"" << stop.second * 100 << "%\" style=\"stop-color:" << stop.first << ";\" />\n";
        }
        gradient << "</linearGradient>";
        gradients.push_back(gradient.str());
    }

    // Utiliser un dégradé dans les formes
    void addRectangleWithGradient(int x, int y, int w, int h, const std::string& gradientId, const std::string& strokeColor = "black", int strokeWidth = 1) {
        std::ostringstream rect;
        rect << "<rect x=\"" << x << "\" y=\"" << y << "\" width=\"" << w << "\" height=\"" << h << "\" style=\"fill:url(#" << gradientId << ");stroke:" << strokeColor
             << ";stroke-width:" << strokeWidth << ";\" />";
        elements.push_back(rect.str());
    }

    void exportToFile(const std::string& filename) {
        std::ofstream file(filename);
        if (!file) {
            throw std::runtime_error("Unable to open file: " + filename);
        }

        file << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << width << "\" height=\"" << height << "\">\n";

        // Ajouter les dégradés
        if (!gradients.empty()) {
            file << "  <defs>\n";
            for (const auto& gradient : gradients) {
                file << "    " << gradient << "\n";
            }
            file << "  </defs>\n";
        }

        // Ajouter les éléments
        for (const auto& elem : elements) {
            file << "  " << elem << "\n";
        }
        file << "</svg>";
        file.close();
    }
};

}  // namespace img
}  // namespace ez
