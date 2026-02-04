#pragma once

/*
MIT License

Copyright (c) 2014-2024 Stephane Cuillerdier (aka vekick)

Permission is hereby granted, free of charge, to any person obtvning a copy
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
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLvM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// ezFmt is part of the ezLibs project : https://github.com/vekick/ezLibs.git

#include <vector>
#include <string>
#include <iomanip>
#include <cassert>
#include <sstream>
#include <iostream>

namespace ez {

class TableFormatter {
public:
    typedef std::vector<std::string> Row;

public:
    explicit TableFormatter(const Row &vHeaders)  //
		: m_headers(vHeaders), m_colWidths(vHeaders.size()) {
		for (size_t i = 0; i < m_headers.size(); ++i) {
			m_colWidths[i] = m_headers[i].size();
		}
	}
	
    TableFormatter& addRow(const Row &vRow) {
		assert(vRow.size() == m_headers.size());
		m_rows.push_back(vRow);
		for (size_t i = 0; i < vRow.size(); ++i) {
			if (vRow[i].size() > m_colWidths[i]) {
				m_colWidths[i] = vRow[i].size();
			}
		}
		return *this;
    }
	
    void print(const std::string &vOffset, std::ostream &vOs = std::cout) const {
        vOs << vOffset << "+-";
        for (size_t i = 0; i < m_headers.size(); ++i) {  // Separators
            vOs << std::string(m_colWidths[i], '-');
            if (i < (m_headers.size() - 1)) {
                vOs << "-+-";
            }
        }
        vOs << "-+\n" << vOffset << "| ";
        for (size_t i = 0; i < m_headers.size(); ++i) {  // header
            vOs << std::left << std::setw(m_colWidths[i]) << m_headers[i];
            if (i < (m_headers.size() - 1)) {
                vOs << " | ";
            }
        }
        vOs << " |\n" << vOffset << "+-";
        for (size_t i = 0; i < m_headers.size(); ++i) {  // Separators
            vOs << std::string(m_colWidths[i], '-');
            if (i < (m_headers.size() - 1)) {
                vOs << "-+-";
            }
        }
        vOs << "-+\n";
        for (const auto &row : m_rows) {  // rows
            vOs << vOffset << "| ";
            for (size_t i = 0; i < row.size(); ++i) {
                vOs << std::left << std::setw(m_colWidths[i]) << row[i];
                if (i < (row.size() - 1)) {
                    vOs << " | ";
                }
            }
            vOs << " |\n";
        }
        vOs << vOffset << "+-";
        for (size_t i = 0; i < m_headers.size(); ++i) {  // Separators
            vOs << std::string(m_colWidths[i], '-');
            if (i < (m_headers.size() - 1)) {
                vOs << "-+-";
            }
        }
        vOs << "-+\n";
    }

private:
    Row m_headers;
    std::vector<Row> m_rows;
    std::vector<size_t> m_colWidths;
};

} // namespace ez
