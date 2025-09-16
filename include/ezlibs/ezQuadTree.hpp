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

// ezQuadTree is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>
#include <queue>

// On suppose ici que vous avez déjà une classe/matrice/méthode pour gérer un vec2<T> :
// template <typename T>
// struct vec2 {
//     T x, y;
// };
//
// Exemple minimaliste d'un rect/bounding box. Vous pouvez l'adapter à votre structure existante.
// template <typename T>
// struct Rect {
//     T xMin, xMax, yMin, yMax;
//     ...
// };

namespace ez
{

    /**
     * @brief QuadTree générique permettant d'insérer, de supprimer et de déplacer des points.
     *        Il propose également une méthode pour récupérer les voisins les plus proches d'un point.
     *
     * @tparam T Type numérique sous-jacent (float, double, etc.)
     *
     * @warning Nécessite l'existence de vec2<T> (déjà fourni selon l'énoncé).
     */
    template <typename T>
    class QuadTree
    {
    public:
        /**
         * @brief Constructeur principal.
         * @param xMin   Borne minimale en X de la zone couverte par le QuadTree.
         * @param xMax   Borne maximale en X de la zone couverte par le QuadTree.
         * @param yMin   Borne minimale en Y de la zone couverte par le QuadTree.
         * @param yMax   Borne maximale en Y de la zone couverte par le QuadTree.
         * @param capacity Nombre maximum de points qu'un noeud peut contenir avant subdivision.
         */
        QuadTree(T xMin, T xMax, T yMin, T yMax, std::size_t capacity = 4)
            : m_capacity(capacity)
        {
            m_boundary.xMin = xMin;
            m_boundary.xMax = xMax;
            m_boundary.yMin = yMin;
            m_boundary.yMax = yMax;
        }

        /**
         * @brief Insère un nouveau point dans le QuadTree.
         * @param pt Le point à insérer.
         * @return true si l'insertion a réussi, false sinon (hors limite par ex.).
         */
        bool insert(const vec2<T>& pt)
        {
            return insertImpl(m_root, pt, m_boundary);
        }

        /**
         * @brief Supprime un point du QuadTree (si présent).
         * @param pt Le point à supprimer.
         * @return true si le point a été trouvé et supprimé, false sinon.
         */
        bool remove(const vec2<T>& pt)
        {
            return removeImpl(m_root, pt, m_boundary);
        }

        /**
         * @brief Déplace un point existant vers une nouvelle position.
         *        Cette fonction layerue une remove + insert si le point existe.
         * @param oldPt Ancienne position du point.
         * @param newPt Nouvelle position du point.
         * @return true si le déplacement a réussi (point trouvé et bien réinséré),
         *         false sinon (point absent ou nouvelle insertion impossible).
         */
        bool move(const vec2<T>& oldPt, const vec2<T>& newPt)
        {
            if (!remove(oldPt))
                return false;
            return insert(newPt);
        }

        /**
         * @brief Récupère les vMaxNeighbors points les plus proches de pt.
         *        Approche simple: on parcourt l'arbre et on stocke les distances au fur et à mesure.
         *        Une approche plus élaborée pourrait faire un rayon d'exploration croissant, etc.
         *
         * @param pt Point de référence.
         * @param vMaxNeighbors Nombre maximal de voisins à récupérer.
         * @return Un vecteur des points les plus proches, triés par distance croissante (peut-être moins que vMaxNeighbors si l'arbre ne contient pas assez de points).
         */
        std::vector<vec2<T>> getNNeighboors(const vec2<T>& pt, std::size_t vMaxNeighbors) const
        {
            // On pourrait optimiser la recherche par région,
            // mais voici une version simple qui parcourt l'ensemble des points.
            std::vector<vec2<T>> allPoints;
            collectAllPoints(m_root, allPoints);

            // On trie par distance
            auto distanceSq = [&](const vec2<T>& p1, const vec2<T>& p2)
            {
                T dx = p1.x - p2.x;
                T dy = p1.y - p2.y;
                return dx * dx + dy * dy;
            };

            std::sort(allPoints.begin(), allPoints.end(), [&](const vec2<T>& a, const vec2<T>& b)
            {
                return distanceSq(a, pt) < distanceSq(b, pt);
            });

            if (allPoints.size() > vMaxNeighbors)
                allPoints.resize(vMaxNeighbors);

            return allPoints;
        }

    private:
        /**
         * @brief Structure contenant la zone englobante (boundary) du QuadTree ou d'un sous-noeud.
         */
        struct Boundary
        {
            T xMin, xMax;
            T yMin, yMax;

            bool contains(const vec2<T>& p) const
            {
                return (p.x >= xMin && p.x <= xMax &&
                        p.y >= yMin && p.y <= yMax);
            }
        };

        /**
         * @brief Structure interne représentant un noeud du QuadTree.
         */
        struct Node
        {
            // Points stockés dans ce noeud (si pas subdivisé ou si capacité non dépassée).
            std::vector<vec2<T>> points;

            // Fils du noeud (quad subdivision). S'ils sont non-nuls, on est subdivisé.
            Node* children[4] = {nullptr, nullptr, nullptr, nullptr};

            ~Node()
            {
                // Nettoyage récursif
                for (int i = 0; i < 4; ++i)
                {
                    delete children[i];
                    children[i] = nullptr;
                }
            }
        };

    private:
        /**
         * @brief Insère un point dans l'arbre à partir d'un noeud donné et d'une boundary connue.
         */
        bool insertImpl(Node*& node, const vec2<T>& pt, const Boundary& boundary)
        {
            // Si le point n'est pas dans la boundary, on ne l'insère pas
            if (!boundary.contains(pt))
                return false;

            // Si le noeud n'existe pas encore, on le crée.
            if (!node)
            {
                node = new Node();
            }

            // Si on n'est pas subdivisé et qu'on peut stocker directement le point.
            if (node->children[0] == nullptr && node->points.size() < m_capacity)
            {
                node->points.push_back(pt);
                return true;
            }

            // Sinon, si on n'a pas encore subdivisé, on subdivise.
            if (node->children[0] == nullptr)
            {
                subdivide(node, boundary);
            }

            // Tente d'insérer dans l'un des 4 sous-noeuds.
            for (int i = 0; i < 4; ++i)
            {
                Boundary childBoundary = computeChildBoundary(boundary, i);
                if (insertImpl(node->children[i], pt, childBoundary))
                {
                    return true;
                }
            }

            // Si quelque chose ne va pas et qu'on ne peut pas l'insérer...
            return false;
        }

        /**
         * @brief Supprime un point à partir d'un noeud donné si présent.
         */
        bool removeImpl(Node*& node, const vec2<T>& pt, const Boundary& boundary)
        {
            if (!node)
                return false;
            if (!boundary.contains(pt))
                return false;

            // Vérifie si le point est stocké dans ce noeud directement
            auto& pts = node->points;
            auto it = std::find(pts.begin(), pts.end(), pt);
            if (it != pts.end())
            {
                pts.erase(it);
                // Optionnel: si plus de points et pas de subdivision, on peut cleaner le node.
                cleanIfEmpty(node);
                return true;
            }

            // Sinon on va plus loin dans les enfants
            bool removed = false;
            for (int i = 0; i < 4; ++i)
            {
                if (node->children[i])
                {
                    Boundary childBoundary = computeChildBoundary(boundary, i);
                    if (removeImpl(node->children[i], pt, childBoundary))
                    {
                        removed = true;
                        break;
                    }
                }
            }

            // On nettoie si besoin
            if (removed)
                cleanIfEmpty(node);

            return removed;
        }

        /**
         * @brief Subdivise un noeud en créant 4 sous-noeuds et en redistribuant les points.
         */
        void subdivide(Node* node, const Boundary& boundary)
        {
            // Crée les 4 children
            for (int i = 0; i < 4; ++i)
            {
                node->children[i] = new Node();
            }

            // Redistribue les points existants dans le noeud vers ses enfants
            std::vector<vec2<T>> oldPoints = node->points;
            node->points.clear();

            for (auto& pt : oldPoints)
            {
                for (int i = 0; i < 4; ++i)
                {
                    Boundary childB = computeChildBoundary(boundary, i);
                    if (childB.contains(pt))
                    {
                        node->children[i]->points.push_back(pt);
                        break;
                    }
                }
            }
        }

        /**
         * @brief Calcule la boundary d'un enfant i (0 à 3) dans la subdivision.
         */
        Boundary computeChildBoundary(const Boundary& boundary, int childIndex) const
        {
            T midX = (boundary.xMin + boundary.xMax) / 2;
            T midY = (boundary.yMin + boundary.yMax) / 2;

            Boundary childB = boundary;

            switch (childIndex)
            {
            // Quadrant 0 : bas-gauche
            case 0:
                childB.xMax = midX;
                childB.yMax = midY;
                break;
            // Quadrant 1 : bas-droit
            case 1:
                childB.xMin = midX;
                childB.yMax = midY;
                break;
            // Quadrant 2 : haut-gauche
            case 2:
                childB.xMax = midX;
                childB.yMin = midY;
                break;
            // Quadrant 3 : haut-droit
            case 3:
                childB.xMin = midX;
                childB.yMin = midY;
                break;
            default:
                break;
            }
            return childB;
        }

        /**
         * @brief Nettoie un noeud si celui-ci ne contient plus de points et que tous ses enfants sont vides.
         *        Dans ce cas, on le supprime entièrement (pour limiter l'usage mémoire).
         */
        void cleanIfEmpty(Node*& node)
        {
            if (!node)
                return;

            if (!node->points.empty())
                return;

            // Vérifie si tous les sous-arbres sont vides
            for (int i = 0; i < 4; ++i)
            {
                if (node->children[i])
                {
                    // Si un enfant existe et n'est pas vide, on ne clean pas
                    if (!node->children[i]->points.empty())
                        return;
                    // On vérifie aussi récursivement
                    bool childHasChildren = false;
                    for (int j = 0; j < 4; ++j)
                    {
                        if (node->children[i]->children[j] != nullptr)
                        {
                            childHasChildren = true;
                            break;
                        }
                    }
                    if (childHasChildren)
                        return;
                }
            }

            // A ce stade, le noeud n'a plus de points et tous les enfants sont vides.
            // On peut détruire ce noeud pour libérer la mémoire
            delete node;
            node = nullptr;
        }

        /**
         * @brief Récupère tous les points (récursivement) d'un sous-arbre.
         */
        void collectAllPoints(const Node* node, std::vector<vec2<T>>& outPoints) const
        {
            if (!node)
                return;
            // Ajout des points du noeud
            outPoints.insert(outPoints.end(), node->points.begin(), node->points.end());
            // Récursivement, va chercher dans les enfants
            for (int i = 0; i < 4; ++i)
            {
                collectAllPoints(node->children[i], outPoints);
            }
        }

    private:
        Node* m_root = nullptr;        ///< Racine du QuadTree
        Boundary m_boundary;           ///< Zone englobante de la racine
        std::size_t m_capacity = 4;    ///< Capacité de chaque noeud avant subdivision
    };

} // namespace ez

/*

*/