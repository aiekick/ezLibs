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

// ezCsv is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <algorithm>

namespace ez {

/**
 * @class Csv
 * @brief Classe permettant de lire, manipuler et écrire des fichiers Csv.
 * 
 * Inspirée des bibliothèques Python pour la manipulation de données tabulaires.
 * - Lecture d’un Csv : readFromFile(...)
 * - Écriture d’un Csv : writeToFile(...)
 * - Gestion du header (facultatif) : hasHeader_, header_, etc.
 * - Accès aux données : data_[ligne][colonne]
 * - Méthodes utilitaires : rowCount(), colCount(), head(), tail(), etc.
 */
class Csv {
private:
    bool hasHeader_{false};                            ///< Indique si le Csv possède un header ou non
    std::vector<std::string> header_;                  ///< Stockage des noms de colonnes (si hasHeader_ = true)
    std::vector<std::vector<std::string>> data_;       ///< Stockage des lignes et des colonnes (hors header)

public:
    /**
     * @brief Constructeur par défaut
     */
    Csv() = default;

    /**
     * @brief Construit la structure Csv en lisant un fichier.
     * @param filename Nom du fichier à lire.
     * @param delimiter Caractère de délimitation (`,` par défaut).
     * @param hasHeader Indique si la première ligne est un header (false par défaut).
     */
    Csv(const std::string& filename, char delimiter = ',', bool hasHeader = false)
        : hasHeader_(hasHeader)
    {
        readFromFile(filename, delimiter, hasHeader_);
    }

    /**
     * @brief Lit un fichier Csv et stocke son contenu dans le membre data_.
     * @param filename Nom du fichier Csv à lire.
     * @param delimiter Caractère de délimitation (`,` par défaut).
     * @param hasHeader Indique si la première ligne est un header (false par défaut).
     */
    void readFromFile(const std::string& filename, char delimiter = ',', bool hasHeader = false) {
        hasHeader_ = hasHeader;
        data_.clear();
        header_.clear();

        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Impossible d'ouvrir le fichier: " + filename);
        }

        std::string line;
        bool isFirstLine = true;

        while (std::getline(file, line)) {
            std::vector<std::string> row;
            std::stringstream lineStream(line);
            std::string cell;

            while (std::getline(lineStream, cell, delimiter)) {
                row.push_back(cell);
            }

            // Si c'est la première ligne et qu'on considère qu'il y a un header,
            // on le stocke séparément puis on passe à la suite.
            if (isFirstLine && hasHeader_) {
                header_ = row;
                isFirstLine = false;
                continue;
            }

            data_.push_back(row);
            isFirstLine = false;
        }

        file.close();
    }

    /**
     * @brief Écrit le contenu de data_ (et éventuellement header_) dans un fichier Csv.
     * @param filename Nom du fichier de sortie.
     * @param delimiter Caractère de délimitation (`,` par défaut).
     */
    void writeToFile(const std::string& filename, char delimiter = ',') const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Impossible d'ouvrir le fichier en écriture: " + filename);
        }

        // Écriture du header si nécessaire
        if (hasHeader_ && !header_.empty()) {
            for (size_t i = 0; i < header_.size(); ++i) {
                file << header_[i];
                if (i < header_.size() - 1) {
                    file << delimiter;
                }
            }
            file << "\n";
        }

        // Écriture des lignes de données
        for (size_t i = 0; i < data_.size(); ++i) {
            for (size_t j = 0; j < data_[i].size(); ++j) {
                file << data_[i][j];
                if (j < data_[i].size() - 1) {
                    file << delimiter;
                }
            }
            file << "\n";
        }

        file.close();
    }

    // --- Méthodes d'accès et d'information ---

    /**
     * @brief Retourne le nombre de lignes (hors header).
     * @return Nombre de lignes de données.
     */
    size_t rowCount() const {
        return data_.size();
    }

    /**
     * @brief Retourne le nombre de colonnes.
     * 
     * Si le Csv possède un header, on se base sur sa taille. Sinon, on se base
     * sur la taille de la première ligne de données (s'il y en a).
     * @return Nombre de colonnes.
     */
    size_t colCount() const {
        if (hasHeader_ && !header_.empty()) {
            return header_.size();
        } else {
            return data_.empty() ? 0 : data_[0].size();
        }
    }

    /**
     * @brief Vérifie si un header est présent.
     * @return True si un header est présent, sinon false.
     */
    bool hasHeader() const {
        return hasHeader_;
    }

    // --- Méthodes liées au header ---

    /**
     * @brief Retourne une référence constante vers le header.
     * @return Le vecteur des noms de colonnes.
     */
    const std::vector<std::string>& getHeader() const {
        if (!hasHeader_) {
            throw std::runtime_error("Le Csv n'a pas de header.");
        }
        return header_;
    }

    /**
     * @brief Modifie le header en entier.
     * @param newHeader Nouveau vecteur représentant les noms de colonnes.
     * 
     * Assure que la taille du nouveau header correspond à la taille des colonnes actuelles.
     */
    void setHeader(const std::vector<std::string>& newHeader) {
        if (!data_.empty() && newHeader.size() != data_[0].size()) {
            throw std::runtime_error("La taille du nouveau header ne correspond pas au nombre de colonnes des données.");
        }
        hasHeader_ = true;
        header_ = newHeader;
    }

    /**
     * @brief Renvoie l’index d’une colonne à partir de son nom dans le header.
     * @param columnName Nom de la colonne.
     * @return L’index de la colonne dans le header (size_t).
     * @throws std::runtime_error si la colonne n’est pas trouvée ou si le Csv n’a pas de header.
     */
    size_t getColumnIndex(const std::string& columnName) const {
        if (!hasHeader_) {
            throw std::runtime_error("Le Csv n'a pas de header.");
        }
        auto it = std::find(header_.begin(), header_.end(), columnName);
        if (it == header_.end()) {
            throw std::runtime_error("Colonne non trouvée: " + columnName);
        }
        return std::distance(header_.begin(), it);
    }

    /**
     * @brief Renomme une colonne dans le header.
     * @param oldName Nom actuel de la colonne.
     * @param newName Nouveau nom de la colonne.
     */
    void renameColumn(const std::string& oldName, const std::string& newName) {
        size_t idx = getColumnIndex(oldName);
        header_[idx] = newName;
    }

    // --- Méthodes d’accès à la data ---

    /**
     * @brief Accès à la ligne i (en lecture/écriture).
     * @param row Index de la ligne (hors header).
     * @return Référence vers le vecteur de chaînes de la ligne.
     */
    std::vector<std::string>& operator[](size_t row) {
        return data_[row];
    }

    /**
     * @brief Accès à la ligne i (en lecture seule).
     * @param row Index de la ligne (hors header).
     * @return Const-référence vers le vecteur de chaînes de la ligne.
     */
    const std::vector<std::string>& operator[](size_t row) const {
        return data_[row];
    }

    /**
     * @brief Accède à une cellule en particulier (en lecture seule), via (row, col).
     * @param row Index de la ligne.
     * @param col Index de la colonne.
     * @return Const-référence vers la cellule (std::string).
     */
    const std::string& at(size_t row, size_t col) const {
        if (row >= rowCount() || col >= colCount()) {
            throw std::out_of_range("Index hors limite dans Csv::at()");
        }
        return data_[row][col];
    }

    /**
     * @brief Accède à une cellule en particulier (en lecture/écriture), via (row, col).
     * @param row Index de la ligne.
     * @param col Index de la colonne.
     * @return Référence vers la cellule (std::string).
     */
    std::string& at(size_t row, size_t col) {
        if (row >= rowCount() || col >= colCount()) {
            throw std::out_of_range("Index hors limite dans Csv::at()");
        }
        return data_[row][col];
    }

    /**
     * @brief Accès rapide à la cellule via nom de colonne et index de ligne.
     * @param row Index de la ligne (hors header).
     * @param columnName Nom de la colonne dans le header.
     * @return Référence vers la cellule (std::string).
     * @throws std::runtime_error si le Csv n’a pas de header.
     */
    std::string& at(size_t row, const std::string& columnName) {
        size_t col = getColumnIndex(columnName);
        return at(row, col);
    }

    // --- Méthodes de modifications ---

    /**
     * @brief Ajoute une nouvelle ligne à la fin du Csv.
     * @param row Vecteur de chaînes représentant la nouvelle ligne.
     * 
     * Vérifie que le nombre d’éléments de row correspond bien au nombre de colonnes actuel
     * (header ou première ligne si pas de header).
     */
    void appendRow(const std::vector<std::string>& row) {
        if (!data_.empty()) {
            // On se base sur la taille de la première ligne
            if (row.size() != data_[0].size()) {
                throw std::runtime_error("La nouvelle ligne n'a pas le même nombre de colonnes.");
            }
        } else if (hasHeader_ && !header_.empty()) {
            // Si on a un header, on compare avec sa taille
            if (row.size() != header_.size()) {
                throw std::runtime_error("La nouvelle ligne n'a pas le même nombre de colonnes que le header.");
            }
        }
        data_.push_back(row);
    }

    /**
     * @brief Ajoute une nouvelle colonne à la fin du Csv.
     * 
     * La nouvelle colonne doit avoir une taille égale au nombre de lignes actuel.
     * Si un header est présent, on peut également lui donner un nom (optionnel).
     * 
     * @param column Vecteur de chaînes représentant la nouvelle colonne.
     * @param columnName (optionnel) Nom de la nouvelle colonne si le Csv a un header.
     */
    void appendColumn(const std::vector<std::string>& column, const std::string& columnName = "") {
        if (column.size() != rowCount()) {
            throw std::runtime_error("La colonne à ajouter doit avoir la même taille que le nombre de lignes.");
        }

        // Si on a un header et un nom de colonne, on l’ajoute au header
        if (hasHeader_ && !header_.empty()) {
            header_.push_back(columnName.empty() ? "New Column" : columnName);
        }

        for (size_t i = 0; i < rowCount(); ++i) {
            data_[i].push_back(column[i]);
        }
    }

    /**
     * @brief Insère une ligne à une position donnée (décalant les lignes suivantes).
     * @param index Position où insérer la ligne.
     * @param row Vecteur de chaînes représentant la nouvelle ligne.
     */
    void insertRow(size_t index, const std::vector<std::string>& row) {
        if (index > rowCount()) {
            throw std::out_of_range("Index de ligne hors limite pour l'insertion.");
        }
        if (!data_.empty() && row.size() != data_[0].size()) {
            throw std::runtime_error("La nouvelle ligne n'a pas le bon nombre de colonnes.");
        } else if (hasHeader_ && !header_.empty() && row.size() != header_.size()) {
            throw std::runtime_error("La nouvelle ligne n'a pas le bon nombre de colonnes (par rapport au header).");
        }
        data_.insert(data_.begin() + index, row);
    }

    /**
     * @brief Insère une colonne à une position donnée (décalant les colonnes suivantes).
     * @param index Position où insérer la colonne.
     * @param column Vecteur de chaînes représentant la nouvelle colonne.
     * @param columnName Nom de la colonne (si header).
     */
    void insertColumn(size_t index, const std::vector<std::string>& column, const std::string& columnName = "") {
        if (index > colCount()) {
            throw std::out_of_range("Index de colonne hors limite pour l'insertion.");
        }
        if (column.size() != rowCount()) {
            throw std::runtime_error("La colonne à insérer doit avoir la même taille que le nombre de lignes.");
        }

        // Mise à jour du header si nécessaire
        if (hasHeader_) {
            if (index > header_.size()) {
                throw std::out_of_range("Index de colonne hors limite dans le header.");
            }
            header_.insert(header_.begin() + index, columnName.empty() ? "New Column" : columnName);
        }

        // Insertion dans la data
        for (size_t i = 0; i < rowCount(); ++i) {
            data_[i].insert(data_[i].begin() + index, column[i]);
        }
    }

    /**
     * @brief Supprime une ligne du Csv.
     * @param index Index de la ligne à supprimer.
     */
    void removeRow(size_t index) {
        if (index >= rowCount()) {
            throw std::out_of_range("Index de ligne hors limite pour la suppression.");
        }
        data_.erase(data_.begin() + index);
    }

    /**
     * @brief Supprime une colonne du Csv.
     * @param index Index de la colonne à supprimer.
     */
    void removeColumn(size_t index) {
        if (index >= colCount()) {
            throw std::out_of_range("Index de colonne hors limite pour la suppression.");
        }

        // Mise à jour du header si nécessaire
        if (hasHeader_) {
            header_.erase(header_.begin() + index);
        }

        // Suppression dans chaque ligne
        for (auto& row : data_) {
            row.erase(row.begin() + index);
        }
    }

    // --- Méthodes d’affichage ---

    /**
     * @brief Affiche les n premières lignes du Csv (similaire à head() en Python/pandas).
     * @param n Nombre de lignes à afficher (5 par défaut).
     */
    void head(size_t n = 5) const {
        if (hasHeader_ && !header_.empty()) {
            for (size_t j = 0; j < header_.size(); ++j) {
                std::cout << header_[j];
                if (j < header_.size() - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << "\n";
        }

        for (size_t i = 0; i < std::min(n, rowCount()); ++i) {
            for (size_t j = 0; j < data_[i].size(); ++j) {
                std::cout << data_[i][j];
                if (j < data_[i].size() - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << "\n";
        }
    }

    /**
     * @brief Affiche les n dernières lignes du Csv (similaire à tail() en Python/pandas).
     * @param n Nombre de lignes à afficher (5 par défaut).
     */
    void tail(size_t n = 5) const {
        if (rowCount() == 0) {
            // Même si le Csv est vide, on peut éventuellement afficher le header
            if (hasHeader_ && !header_.empty()) {
                for (size_t j = 0; j < header_.size(); ++j) {
                    std::cout << header_[j];
                    if (j < header_.size() - 1) {
                        std::cout << ", ";
                    }
                }
                std::cout << "\n";
            }
            return;
        }

        size_t start = (n >= rowCount()) ? 0 : (rowCount() - n);

        // Affichage du header si nécessaire
        if (hasHeader_ && !header_.empty()) {
            for (size_t j = 0; j < header_.size(); ++j) {
                std::cout << header_[j];
                if (j < header_.size() - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << "\n";
        }

        for (size_t i = start; i < rowCount(); ++i) {
            for (size_t j = 0; j < data_[i].size(); ++j) {
                std::cout << data_[i][j];
                if (j < data_[i].size() - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << "\n";
        }
    }
};

} // namespace ez

/*
Exemple d'utilisation :

#include "ez_csv.hpp"
#include <iostream>

int main() {
    // Lecture d'un Csv avec header
    ez::Csv csv("data_with_header.csv", ',', true);

    // Affiche les premières lignes
    csv.head();

    // Récupère l'index d'une colonne
    size_t colIndex = csv.getColumnIndex("Name");
    std::cout << "Index de la colonne 'Name': " << colIndex << "\n";

    // Modifie une cellule
    csv.at(0, "Name") = "NouvelleValeur";

    // Ajoute une nouvelle colonne
    std::vector<std::string> newCol(csv.rowCount(), "X");
    csv.appendColumn(newCol, "New_Column");

    // Écrit vers un fichier
    csv.writeToFile("output.csv");

    return 0;
}
*/
