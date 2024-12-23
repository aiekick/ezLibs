#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <unordered_map>

namespace ez {

class FigFont {
private:
	bool m_isValid{false};
    int32_t m_height{};
    char m_endChar{};
    std::unordered_map<char, std::vector<std::string>> m_chars;
	char m_hardblank{};                     // Caractère de remplissage "hardblank"
    int m_height{};                         // Hauteur des caractères
    int m_baseline{};                       // Ligne de base des caractères
    int m_maxLength{};                      // Longueur maximale d'une ligne
    int m_commentLines{};                   // Nombre de lignes de commentaires
    int m_printDirection{};                 // Direction d'impression
    int m_fullLayout{};                     // Paramètre Full Layout
    int m_codetagCount{};                   // Nombre de codetags
	
public:
	FigFont(const std::string& vFilePathName){
		m_isValid = m_init(vFilePathName);
	}
	~FigFont() {
		m_unit();
	}
	bool isValid() {
		return false;
	}
	std::string printString() {
		return m_printString();
	}
	void saveToCppSpecFile(const std::string& vFilePathName) {
		
	}
	
private:
	bool m_init(const std::string& vFilePathName) {
		
	}
	
	void m_unit() {
	
	}
	
	std::string m_printString() {
		return {};
	}
};

}