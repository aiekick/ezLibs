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

// ezArgs is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include "ezOS.hpp"

#include <vector>
#include <string>
#include <cstdio>   // FILENAME_MAX
#include <cstdint>  // int32_t
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <limits>

#include "ezStr.hpp"
#include "ezLog.hpp"

namespace ez {

class Args {
private:
    class Argument {
        friend class Args;
        friend class CommandArgument;

    protected:
        std::vector<std::string> m_base_args;  // base args
        std::set<std::string> m_full_args;     // full args
        char one_char_arg = 0;
        std::string m_help_text;
        std::string m_help_var_name;
        std::string m_type;
        char m_delimiter = 0;       // delimiter used for arguments : toto a, toto=a, toto:a, etc...
        bool m_is_present = false;  // found during parsing
        bool m_has_value = false;   // found during parsing
        std::string m_value;        // value

        // Array support
        bool m_is_array = false;
        size_t m_array_min_count = 0;                                   // minimum number of elements (0 = no minimum)
        size_t m_array_max_count = std::numeric_limits<size_t>::max();  // maximum number of elements
        std::vector<std::string> m_array_values;                        // array values

    public:
        Argument() = default;

        template <typename TRETURN_TYPE>
        TRETURN_TYPE &help(const std::string &vHelp, const std::string &vVarName) {
            m_help_text = vHelp;
            m_help_var_name = vVarName;
            return *static_cast<TRETURN_TYPE *>(this);
        }

        template <typename TRETURN_TYPE>
        TRETURN_TYPE &def(const std::string &vDefValue) {
            m_value = vDefValue;
            return *static_cast<TRETURN_TYPE *>(this);
        }

        template <typename TRETURN_TYPE>
        TRETURN_TYPE &type(const std::string &vType) {
            m_type = vType;
            return *static_cast<TRETURN_TYPE *>(this);
        }

        template <typename TRETURN_TYPE>
        TRETURN_TYPE &delimiter(char vDelimiter) {
            m_delimiter = vDelimiter;
            return *static_cast<TRETURN_TYPE *>(this);
        }

        template <typename TRETURN_TYPE>
        TRETURN_TYPE &array(size_t vCount) {
            m_is_array = true;
            m_array_min_count = vCount;
            m_array_max_count = vCount;
            return *static_cast<TRETURN_TYPE *>(this);
        }

        template <typename TRETURN_TYPE>
        TRETURN_TYPE &array(size_t vMinCount, size_t vMaxCount) {
            m_is_array = true;
            m_array_min_count = vMinCount;
            m_array_max_count = vMaxCount;
            return *static_cast<TRETURN_TYPE *>(this);
        }

        template <typename TRETURN_TYPE>
        TRETURN_TYPE &arrayUnlimited() {
            m_is_array = true;
            m_array_min_count = 0;
            m_array_max_count = std::numeric_limits<size_t>::max();
            return *static_cast<TRETURN_TYPE *>(this);
        }

    private:
        typedef std::pair<std::string, std::string> HelpCnt;

        void m_getHelpArray(std::stringstream &vSs, const std::string &vPrefix) const {
            if (m_is_array) {
                if (m_array_min_count == m_array_max_count) {
                    vSs << " " << vPrefix << " (x " << m_array_min_count << ") ";
                } else if (m_array_max_count == std::numeric_limits<size_t>::max()) {
                    if (m_array_min_count > 0) {
                        vSs << " " << vPrefix << " (min " << m_array_min_count << ")";
                    } else {
                        vSs << " " << vPrefix << " (unlimited)";
                    }
                } else {
                    vSs << " " << vPrefix << " (" << m_array_min_count << "-" << m_array_max_count << ")";
                }
            }
        }

        HelpCnt m_getHelp(bool vPositional, size_t &vInOutFirstColSize) const {
            HelpCnt res;
            std::stringstream ss;
            if (vPositional) {
                std::string token = m_help_var_name;
                if (token.empty()) {
                    token = *(m_base_args.begin());
                }
                ss << "  " << token;
                m_getHelpArray(ss, {});
            } else {
                size_t idx = 0;
                ss << "  ";
                for (const auto &arg : m_base_args) {
                    if (idx++ > 0) {
                        ss << ", ";
                    }
                    ss << arg;
                }
                if (!m_help_var_name.empty()) {
                    ss << m_delimiter << m_help_var_name;
                    m_getHelpArray(ss, "...");
                }
            }
            auto ret = ss.str();
            if (vInOutFirstColSize < ret.size()) {
                vInOutFirstColSize = ret.size();
            }
            return std::make_pair(ret, m_help_text);
        }
    };

    class PositionalArgument final : public Argument {
        friend class Args;
        friend class CommandArgument;

    public:
        PositionalArgument &help(const std::string &vHelp, const std::string &vVarName) { return Argument::help<PositionalArgument>(vHelp, vVarName); }
        PositionalArgument &type(const std::string &vType) { return Argument::type<PositionalArgument>(vType); }
        PositionalArgument &array(size_t vCount) { return Argument::array<PositionalArgument>(vCount); }
        PositionalArgument &array(size_t vMinCount, size_t vMaxCount) { return Argument::array<PositionalArgument>(vMinCount, vMaxCount); }
        PositionalArgument &arrayUnlimited() { return Argument::arrayUnlimited<PositionalArgument>(); }
    };

    class OptionalArgument final : public Argument {
        friend class Args;
        friend class CommandArgument;

    private:
        bool m_required = false;

    public:
        OptionalArgument &help(const std::string &vHelp, const std::string &vVarName) { return Argument::help<OptionalArgument>(vHelp, vVarName); }
        OptionalArgument &def(const std::string &vDefValue) { return Argument::def<OptionalArgument>(vDefValue); }
        OptionalArgument &type(const std::string &vType) { return Argument::type<OptionalArgument>(vType); }
        OptionalArgument &delimiter(char vDelimiter) { return Argument::delimiter<OptionalArgument>(vDelimiter); }
        OptionalArgument &required(bool vValue) {
            m_required = vValue;
            return *this;
        }
        OptionalArgument &array(size_t vCount) { return Argument::array<OptionalArgument>(vCount); }
        OptionalArgument &array(size_t vMinCount, size_t vMaxCount) { return Argument::array<OptionalArgument>(vMinCount, vMaxCount); }
        OptionalArgument &arrayUnlimited() { return Argument::arrayUnlimited<OptionalArgument>(); }
    };

    // =========================================================================
    // NEW: CommandArgument - une commande avec ses propres sous-arguments
    // =========================================================================
    class CommandArgument final : public Argument {
        friend class Args;

    private:
        std::vector<PositionalArgument> m_subPositionals;
        std::vector<OptionalArgument> m_subOptionals;

    public:
        CommandArgument &help(const std::string &vHelp, const std::string &vVarName) { return Argument::help<CommandArgument>(vHelp, vVarName); }

        // Ajouter un sous-positional (obligatoire quand la commande est active)
        PositionalArgument &addPositional(const std::string &vKey) {
            if (vKey.empty()) {
                throw std::runtime_error("sub-positional cant be empty");
            }
            PositionalArgument res;
            res.m_base_args = ez::str::splitStringToVector(vKey, '/');
            for (const auto &a : res.m_base_args) {
                res.m_full_args.emplace(a);
            }
            m_subPositionals.push_back(res);
            return m_subPositionals.back();
        }

        // Ajouter un sous-optional
        OptionalArgument &addOptional(const std::string &vKey) {
            if (vKey.empty()) {
                throw std::runtime_error("sub-optional cant be empty");
            }
            OptionalArgument res;
            res.m_base_args = ez::str::splitStringToVector(vKey, '/');
            for (const auto &a : res.m_base_args) {
                res.m_full_args.emplace(a);
            }
            // Ajouter aussi les versions sans tirets
            for (const auto &arg : res.m_base_args) {
                auto short_last_minus = arg.find_first_not_of("-");
                if (short_last_minus != std::string::npos) {
                    res.m_full_args.emplace(arg.substr(short_last_minus));
                }
            }
            res.one_char_arg = (vKey.size() == 1U) ? vKey[0] : 0;
            m_subOptionals.push_back(res);
            return m_subOptionals.back();
        }

        // Vérifier si un sous-optional est présent
        bool isSubPresent(const std::string &vKey) const {
            for (const auto &opt : m_subOptionals) {
                if (opt.m_full_args.find(vKey) != opt.m_full_args.end()) {
                    return opt.m_is_present;
                }
            }
            for (const auto &pos : m_subPositionals) {
                if (pos.m_full_args.find(vKey) != pos.m_full_args.end()) {
                    return pos.m_is_present;
                }
            }
            return false;
        }

        // Récupérer la valeur d'un sous-argument
        std::string getSubValue(const std::string &vKey) const {
            for (const auto &opt : m_subOptionals) {
                if (opt.m_full_args.find(vKey) != opt.m_full_args.end()) {
                    return opt.m_value;
                }
            }
            for (const auto &pos : m_subPositionals) {
                if (pos.m_full_args.find(vKey) != pos.m_full_args.end()) {
                    return pos.m_value;
                }
            }
            return {};
        }

        // Récupérer les valeurs array d'un sous-argument
        std::vector<std::string> getSubArrayValues(const std::string &vKey) const {
            for (const auto &opt : m_subOptionals) {
                if (opt.m_full_args.find(vKey) != opt.m_full_args.end()) {
                    return opt.m_array_values;
                }
            }
            for (const auto &pos : m_subPositionals) {
                if (pos.m_full_args.find(vKey) != pos.m_full_args.end()) {
                    return pos.m_array_values;
                }
            }
            return {};
        }

    private:
        // Génère l'aide spécifique à cette commande
        std::string getCommandHelp(size_t vFirstColSize) const {
            std::stringstream ss;

            // Sous-positionals
            if (!m_subPositionals.empty()) {
                for (const auto &pos : m_subPositionals) {
                    auto helpPair = pos.m_getHelp(true, vFirstColSize);
                    ss << "    " << helpPair.first;
                    if (helpPair.first.size() < vFirstColSize) {
                        ss << std::string(vFirstColSize - helpPair.first.size(), ' ');
                    }
                    ss << helpPair.second << std::endl;
                }
            }

            // Sous-optionals
            if (!m_subOptionals.empty()) {
                for (const auto &opt : m_subOptionals) {
                    auto helpPair = opt.m_getHelp(false, vFirstColSize);
                    ss << "    " << helpPair.first;
                    if (helpPair.first.size() < vFirstColSize) {
                        ss << std::string(vFirstColSize - helpPair.first.size(), ' ');
                    }
                    ss << helpPair.second << std::endl;
                }
            }

            return ss.str();
        }
    };

private:
    std::string m_AppName;
    std::string m_HelpHeader;
    std::string m_HelpFooter;
    std::string m_HelpDescription;
    OptionalArgument m_HelpArgument;
    std::vector<PositionalArgument> m_Positionals;
    std::vector<OptionalArgument> m_Optionals;
    std::vector<CommandArgument> m_Commands;     // NEW
    CommandArgument *m_ActiveCommand = nullptr;  // NEW: commande active après parsing
    std::vector<std::string> m_errors;

public:
    Args() = default;
    virtual ~Args() = default;
    Args(const std::string &vName) : m_AppName(vName) {
        if (vName.empty()) {
            throw std::runtime_error("Name cant be empty");
        }
        m_addOptional(m_HelpArgument, "-h/--help").help("Show the usage", {});
    }

    Args &addHeader(const std::string &vHeader) {
        m_HelpHeader = vHeader;
        return *this;
    }

    Args &addFooter(const std::string &vFooter) {
        m_HelpFooter = vFooter;
        return *this;
    }

    Args &addDescription(const std::string &vDescription) {
        m_HelpDescription = vDescription;
        return *this;
    }

    PositionalArgument &addPositional(const std::string &vKey) {
        if (vKey.empty()) {
            throw std::runtime_error("argument cant be empty");
        }
        PositionalArgument res;
        res.m_base_args = ez::str::splitStringToVector(vKey, '/');
        for (const auto &a : res.m_base_args) {
            res.m_full_args.emplace(a);
        }
        m_Positionals.push_back(res);
        return m_Positionals.back();
    }

    OptionalArgument &addOptional(const std::string &vKey) {
        if (vKey.empty()) {
            throw std::runtime_error("optional cant be empty");
        }
        OptionalArgument res;
        m_addOptional(res, vKey);
        m_Optionals.push_back(res);
        return m_Optionals.back();
    }

    // =========================================================================
    // NEW: Ajouter une commande
    // =========================================================================
    CommandArgument &addCommand(const std::string &vKey) {
        if (vKey.empty()) {
            throw std::runtime_error("command cant be empty");
        }
        CommandArgument res;
        res.m_base_args = ez::str::splitStringToVector(vKey, '/');
        for (const auto &a : res.m_base_args) {
            res.m_full_args.emplace(a);
        }
        m_Commands.push_back(res);
        return m_Commands.back();
    }

    // NEW: Récupérer la commande active (nullptr si aucune)
    const CommandArgument *getActiveCommand() const { return m_ActiveCommand; }

    // NEW: Vérifier si une commande est active
    bool isCommand(const std::string &vKey) const {
        if (m_ActiveCommand == nullptr) {
            return false;
        }
        return m_ActiveCommand->m_full_args.find(vKey) != m_ActiveCommand->m_full_args.end();
    }

    // is token present
    bool isPresent(const std::string &vKey) const {
        // D'abord chercher dans la commande active
        if (m_ActiveCommand != nullptr) {
            for (const auto &opt : m_ActiveCommand->m_subOptionals) {
                if (opt.m_full_args.find(vKey) != opt.m_full_args.end()) {
                    return opt.m_is_present;
                }
            }
            for (const auto &pos : m_ActiveCommand->m_subPositionals) {
                if (pos.m_full_args.find(vKey) != pos.m_full_args.end()) {
                    return pos.m_is_present;
                }
            }
        }
        // Sinon chercher dans les globaux
        auto *ptr = m_getArgumentPtr(vKey, true);
        if (ptr != nullptr) {
            return ptr->m_is_present;
        }
        return false;
    }

    // is token have value
    bool hasValue(const std::string &vKey) const {
        if (m_ActiveCommand != nullptr) {
            for (const auto &opt : m_ActiveCommand->m_subOptionals) {
                if (opt.m_full_args.find(vKey) != opt.m_full_args.end()) {
                    return opt.m_has_value;
                }
            }
            for (const auto &pos : m_ActiveCommand->m_subPositionals) {
                if (pos.m_full_args.find(vKey) != pos.m_full_args.end()) {
                    return pos.m_has_value;
                }
            }
        }
        auto *ptr = m_getArgumentPtr(vKey, true);
        if (ptr != nullptr) {
            return ptr->m_has_value;
        }
        return false;
    }

    // is token an array
    bool isArray(const std::string &vKey) const {
        if (m_ActiveCommand != nullptr) {
            for (const auto &opt : m_ActiveCommand->m_subOptionals) {
                if (opt.m_full_args.find(vKey) != opt.m_full_args.end()) {
                    return opt.m_is_array;
                }
            }
            for (const auto &pos : m_ActiveCommand->m_subPositionals) {
                if (pos.m_full_args.find(vKey) != pos.m_full_args.end()) {
                    return pos.m_is_array;
                }
            }
        }
        auto *ptr = m_getArgumentPtr(vKey, true);
        if (ptr != nullptr) {
            return ptr->m_is_array;
        }
        return false;
    }

    template <typename T>
    T getValue(const std::string &vKey, bool vNoExcept = false) const {
        // Chercher dans la commande active d'abord
        if (m_ActiveCommand != nullptr) {
            for (const auto &opt : m_ActiveCommand->m_subOptionals) {
                if (opt.m_full_args.find(vKey) != opt.m_full_args.end() && !opt.m_value.empty()) {
                    return m_convertString<T>(opt.m_value);
                }
            }
            for (const auto &pos : m_ActiveCommand->m_subPositionals) {
                if (pos.m_full_args.find(vKey) != pos.m_full_args.end() && !pos.m_value.empty()) {
                    return m_convertString<T>(pos.m_value);
                }
            }
        }
        auto *ptr = m_getArgumentPtr(vKey, vNoExcept);
        if (ptr != nullptr && !ptr->m_value.empty()) {
            return m_convertString<T>(ptr->m_value);
        }
        return {};
    }

    // Get array values as vector of strings
    std::vector<std::string> getArrayValues(const std::string &vKey, bool vNoExcept = false) const {
        // Chercher dans la commande active d'abord
        if (m_ActiveCommand != nullptr) {
            for (const auto &opt : m_ActiveCommand->m_subOptionals) {
                if (opt.m_full_args.find(vKey) != opt.m_full_args.end()) {
                    return opt.m_array_values;
                }
            }
            for (const auto &pos : m_ActiveCommand->m_subPositionals) {
                if (pos.m_full_args.find(vKey) != pos.m_full_args.end()) {
                    return pos.m_array_values;
                }
            }
        }
        auto *ptr = m_getArgumentPtr(vKey, vNoExcept);
        if (ptr != nullptr) {
            if (!ptr->m_is_array) {
                if (!vNoExcept) {
                    throw std::runtime_error("Argument is not an array");
                }
                return {};
            }
            return ptr->m_array_values;
        }
        return {};
    }

    // Get array values with type conversion
    template <typename T>
    std::vector<T> getArrayValues(const std::string &vKey, bool vNoExcept = false) const {
        auto strValues = getArrayValues(vKey, vNoExcept);
        std::vector<T> result;
        result.reserve(strValues.size());
        for (const auto &val : strValues) {
            try {
                result.push_back(m_convertString<T>(val));
            } catch (const std::exception &e) {
                if (!vNoExcept) {
                    throw;
                }
            }
        }
        return result;
    }

    void getHelp(  //
        std::ostream &vOs,
        const std::string &vPositionalHeader,
        const std::string &vOptionalHeader,
        const std::string &vCommandHeader = "Commands") const {
        if (!m_HelpHeader.empty()) {
            vOs << m_HelpHeader << std::endl << std::endl;
        }
        vOs << m_getCmdLineHelp();
        vOs << std::endl;
        if (!m_HelpDescription.empty()) {
            vOs << std::endl << " " << m_HelpDescription << std::endl;
        }
        vOs << m_getHelpDetails(vPositionalHeader, vOptionalHeader, vCommandHeader);
        if (!m_HelpFooter.empty()) {
            vOs << std::endl << m_HelpFooter << std::endl;
        }
    }

    void printHelp(
        std::ostream &vOs = std::cout,
        const std::string &vPositionalHeader = "Positionnal arguments",
        const std::string &vOptionalHeader = "Optional arguments",
        const std::string &vCommandHeader = "Commands") const {
        getHelp(vOs, vPositionalHeader, vOptionalHeader, vCommandHeader);
        vOs << std::endl;
    }

    void printErrors(const std::string &vIndent) const {
        if (m_errors.empty()) {
            return;
        }
        for (const auto &error : m_errors) {
            std::cout << vIndent << error << std::endl;
        }
    }

    bool parse(const int32_t vArgc, char **vArgv, const int32_t vStartIdx = 1U) {
        size_t positional_idx = 0;
        m_ActiveCommand = nullptr;

        for (int32_t idx = vStartIdx; idx < vArgc; ++idx) {
            std::string arg = m_trim(vArgv[idx]);

            // print help
            if (m_HelpArgument.m_full_args.find(arg) != m_HelpArgument.m_full_args.end()) {
                printHelp();
                return false;  // interrupt parsing
            }

            // =========================================================
            // NEW: Vérifier si c'est une commande
            // =========================================================
            bool is_command = false;
            if (m_ActiveCommand == nullptr) {  // Une seule commande active
                for (auto &cmd : m_Commands) {
                    if (cmd.m_full_args.find(arg) != cmd.m_full_args.end()) {
                        cmd.m_is_present = true;
                        m_ActiveCommand = &cmd;
                        is_command = true;
                        break;
                    }
                }
            }
            if (is_command) {
                continue;  // Passer à l'argument suivant
            }

            // check optionals (globaux et de la commande active)
            std::string token = arg;
            std::string value;
            bool is_optional = false;
            bool check_for_value = false;
            OptionalArgument *current_optional = nullptr;

            // D'abord vérifier les sous-optionals de la commande active
            if (m_ActiveCommand != nullptr) {
                for (auto &arg_ref : m_ActiveCommand->m_subOptionals) {
                    check_for_value = false;
                    if (arg_ref.m_delimiter != 0) {
                        if (arg_ref.m_delimiter != ' ') {
                            if (token.find(arg_ref.m_delimiter) != std::string::npos) {
                                auto arr = ez::str::splitStringToVector(token, arg_ref.m_delimiter);
                                if (arr.size() == 2) {
                                    token = arr.at(0);
                                    value = arr.at(1);
                                }
                            }
                        }
                    }
                    if (arg_ref.one_char_arg != 0) {
                        auto p = token.find(arg_ref.one_char_arg);
                        if (p != std::string::npos) {
                            arg_ref.m_is_present = true;
                            is_optional = true;
                            current_optional = &arg_ref;
                            if (p == (token.size() - 1)) {
                                check_for_value = true;
                            }
                        }
                    } else if (arg_ref.m_full_args.find(token) != arg_ref.m_full_args.end()) {
                        arg_ref.m_is_present = true;
                        is_optional = true;
                        current_optional = &arg_ref;
                        check_for_value = true;
                    }

                    if (check_for_value && current_optional != nullptr) {
                        m_parseOptionalValue(current_optional, idx, vArgc, vArgv, value);
                        break;
                    }
                }
            }

            // Puis vérifier les optionals globaux
            if (!is_optional) {
                token = arg;  // Reset token
                for (auto &arg_ref : m_Optionals) {
                    check_for_value = false;
                    if (arg_ref.m_delimiter != 0) {
                        if (arg_ref.m_delimiter != ' ') {
                            if (token.find(arg_ref.m_delimiter) != std::string::npos) {
                                auto arr = ez::str::splitStringToVector(token, arg_ref.m_delimiter);
                                if (arr.size() == 2) {
                                    token = arr.at(0);
                                    value = arr.at(1);
                                } else {
                                    if (arr.size() < 2) {
                                        m_addError("bad parsing of key \"" + token + "\". no value");
                                    } else if (arr.size() > 2) {
                                        m_addError("bad parsing of key \"" + token + "\". more than one value");
                                    }
                                }
                            }
                        }
                    }
                    if (arg_ref.one_char_arg != 0) {
                        auto p = token.find(arg_ref.one_char_arg);
                        if (p != std::string::npos) {
                            arg_ref.m_is_present = true;
                            is_optional = true;
                            current_optional = &arg_ref;
                            if (p == (token.size() - 1)) {
                                check_for_value = true;
                            }
                        }
                    } else if (arg_ref.m_full_args.find(token) != arg_ref.m_full_args.end()) {
                        arg_ref.m_is_present = true;
                        is_optional = true;
                        current_optional = &arg_ref;
                        check_for_value = true;
                    }

                    if (check_for_value && current_optional != nullptr) {
                        m_parseOptionalValue(current_optional, idx, vArgc, vArgv, value);
                        break;
                    }
                }
            }

            // check positionals (globaux puis de la commande active)
            if (!is_optional) {
                // D'abord les positionals globaux
                if (positional_idx < m_Positionals.size()) {
                    auto &positional = m_Positionals.at(positional_idx);
                    m_parsePositionalValue(positional, arg, idx, vArgc, vArgv);
                    ++positional_idx;
                }
                // Puis les sous-positionals de la commande active
                else if (m_ActiveCommand != nullptr) {
                    size_t sub_pos_idx = positional_idx - m_Positionals.size();
                    if (sub_pos_idx < m_ActiveCommand->m_subPositionals.size()) {
                        auto &positional = m_ActiveCommand->m_subPositionals.at(sub_pos_idx);
                        m_parsePositionalValue(positional, arg, idx, vArgc, vArgv);
                        ++positional_idx;
                    } else {
                        m_addError("Unknown argument: " + arg);
                    }
                } else {
                    m_addError("Unknown argument: " + arg);
                }
            }
        }

        // Validation des positionals globaux
        for (const auto &pos : m_Positionals) {
            if (!pos.m_is_present) {
                m_addError("Positional <" + pos.m_base_args.at(0) + "> not present");
            } else if (pos.m_is_array) {
                size_t count = pos.m_array_values.size();
                if (count < pos.m_array_min_count) {
                    m_addError(
                        "Positional array <" + pos.m_base_args.at(0) + "> expects at least " + std::to_string(pos.m_array_min_count) + " values, got " +
                        std::to_string(count));
                }
                if (count > pos.m_array_max_count) {
                    m_addError(
                        "Positional array <" + pos.m_base_args.at(0) + "> expects at most " + std::to_string(pos.m_array_max_count) + " values, got " +
                        std::to_string(count));
                }
            }
        }

        // Validation des optionals globaux
        for (const auto &opt : m_Optionals) {
            if (opt.m_required && !opt.m_is_present) {
                m_addError("Optional <" + opt.m_base_args.at(0) + "> not present");
            } else if (opt.m_is_present && opt.m_is_array) {
                size_t count = opt.m_array_values.size();
                if (count < opt.m_array_min_count) {
                    m_addError(
                        "Optional array <" + opt.m_base_args.at(0) + "> expects at least " + std::to_string(opt.m_array_min_count) + " values, got " +
                        std::to_string(count));
                }
                if (count > opt.m_array_max_count) {
                    m_addError(
                        "Optional array <" + opt.m_base_args.at(0) + "> expects at most " + std::to_string(opt.m_array_max_count) + " values, got " +
                        std::to_string(count));
                }
            }
        }

        // NEW: Validation des sous-arguments de la commande active
        if (m_ActiveCommand != nullptr) {
            for (const auto &pos : m_ActiveCommand->m_subPositionals) {
                if (!pos.m_is_present) {
                    m_addError("Command '" + m_ActiveCommand->m_base_args.at(0) + "' requires <" + pos.m_base_args.at(0) + ">");
                }
            }
            for (const auto &opt : m_ActiveCommand->m_subOptionals) {
                if (opt.m_required && !opt.m_is_present) {
                    m_addError("Command '" + m_ActiveCommand->m_base_args.at(0) + "' requires " + opt.m_base_args.at(0));
                }
            }
        }

        return m_errors.empty();
    }

    const std::vector<std::string> &getErrors() { return m_errors; }

private:
    const Argument *m_getArgumentPtr(const std::string &vKey, bool vNoExcept = false) const {
        const Argument *ret = nullptr;
        for (const auto &arg : m_Positionals) {
            if (arg.m_full_args.find(vKey) != arg.m_full_args.end()) {
                ret = &arg;
            }
        }
        if (ret == nullptr) {
            for (const auto &arg : m_Optionals) {
                if (arg.m_full_args.find(vKey) != arg.m_full_args.end()) {
                    ret = &arg;
                }
            }
        }
        if (ret == nullptr && vNoExcept == false) {
            throw std::runtime_error("Argument not found");
        }
        return ret;
    }

    Argument *m_getArgumentPtr(const std::string &vKey, bool vNoExcept = false) {
        return const_cast<Argument *>(static_cast<const Args *>(this)->m_getArgumentPtr(vKey, vNoExcept));
    }

    OptionalArgument &m_addOptional(OptionalArgument &vInOutArgument, const std::string &vKey) {
        if (vKey.empty()) {
            throw std::runtime_error("optional cant be empty");
        }
        vInOutArgument.m_base_args = ez::str::splitStringToVector(vKey, '/');
        for (const auto &a : vInOutArgument.m_base_args) {
            vInOutArgument.m_full_args.emplace(a);
        }
        for (const auto &arg : vInOutArgument.m_base_args) {
            vInOutArgument.m_full_args.emplace(m_trim(arg));
        }
        vInOutArgument.one_char_arg = (vKey.size() == 1U) ? vKey[0] : 0;
        return vInOutArgument;
    }

    // NEW: Helper pour parser la valeur d'un optional
    void m_parseOptionalValue(OptionalArgument *opt, int32_t &idx, int32_t vArgc, char **vArgv, const std::string &value) {
        if (opt->m_is_array) {
            size_t count = 0;
            if (opt->m_delimiter == ' ') {
                while ((idx + 1) < vArgc && count < opt->m_array_max_count) {
                    std::string nextArg = m_trim(vArgv[idx + 1]);
                    // Vérifier si c'est un autre argument connu
                    if (m_isKnownArgument(nextArg)) {
                        break;
                    }
                    opt->m_array_values.push_back(vArgv[++idx]);
                    ++count;
                }
            } else if (opt->m_delimiter != 0 && !value.empty()) {
                opt->m_array_values.push_back(value);
                ++count;
            }
            if (count > 0) {
                opt->m_has_value = true;
                opt->m_value = opt->m_array_values[0];
            }
        } else {
            if (opt->m_delimiter == ' ') {
                if ((idx + 1) < vArgc) {
                    std::string nextArg = m_trim(vArgv[idx + 1]);
                    if (!m_isKnownArgument(nextArg)) {
                        opt->m_value = vArgv[++idx];
                        opt->m_has_value = true;
                    }
                }
            } else if (opt->m_delimiter != 0 && !value.empty()) {
                opt->m_value = value;
                opt->m_has_value = true;
            }
        }
    }

    // NEW: Helper pour parser un positional
    void m_parsePositionalValue(PositionalArgument &pos, const std::string &arg, int32_t &idx, int32_t vArgc, char **vArgv) {
        pos.m_is_present = true;
        if (pos.m_is_array) {
            pos.m_array_values.push_back(arg);
            size_t count = 1;
            while ((idx + 1) < vArgc && count < pos.m_array_max_count) {
                std::string next_arg = m_trim(vArgv[idx + 1]);
                if (m_isKnownArgument(next_arg)) {
                    break;
                }
                pos.m_array_values.push_back(vArgv[++idx]);
                ++count;
            }
            pos.m_has_value = true;
            pos.m_value = pos.m_array_values[0];
        } else {
            pos.m_value = arg;
            pos.m_has_value = true;
        }
    }

    // NEW: Vérifie si un argument est connu (optional, command, etc.)
    bool m_isKnownArgument(const std::string &arg) const {
        // Vérifier les optionals globaux
        for (const auto &opt : m_Optionals) {
            if (opt.m_full_args.find(arg) != opt.m_full_args.end()) {
                return true;
            }
        }
        // Vérifier les commandes
        for (const auto &cmd : m_Commands) {
            if (cmd.m_full_args.find(arg) != cmd.m_full_args.end()) {
                return true;
            }
        }
        // Vérifier les sous-optionals de la commande active
        if (m_ActiveCommand != nullptr) {
            for (const auto &opt : m_ActiveCommand->m_subOptionals) {
                if (opt.m_full_args.find(arg) != opt.m_full_args.end()) {
                    return true;
                }
            }
        }
        // Vérifier le help
        if (m_HelpArgument.m_full_args.find(arg) != m_HelpArgument.m_full_args.end()) {
            return true;
        }
        return false;
    }

    void m_getCmdLineOptional(const OptionalArgument &vOptionalArgument, std::stringstream &vStream) const {
        vStream << " [";
        size_t idx = 0;
        for (const auto &o : vOptionalArgument.m_base_args) {
            if (idx++ > 0) {
                vStream << ':';
            }
            vStream << o;
        }
        if (!vOptionalArgument.m_help_var_name.empty()) {
            vStream << vOptionalArgument.m_delimiter << vOptionalArgument.m_help_var_name;
            if (vOptionalArgument.m_is_array) {
                vStream << " ...";
            }
        }
        vStream << "]";
    }

    void m_getCmdLinePositional(const PositionalArgument &vPositionalArgument, std::stringstream &vStream) const {
        std::string token = vPositionalArgument.m_help_var_name;
        if (token.empty()) {
            token = *(vPositionalArgument.m_base_args.begin());
        }
        vStream << " " << token;
        if (vPositionalArgument.m_is_array) {
            vStream << " ...";
        }
    }

    std::string m_getCmdLineHelp() const {
        std::stringstream ss;
        ss << " Usage : " << m_AppName;
        m_getCmdLineOptional(m_HelpArgument, ss);
        for (const auto &arg : m_Optionals) {
            m_getCmdLineOptional(arg, ss);
        }
        for (const auto &arg : m_Positionals) {
            m_getCmdLinePositional(arg, ss);
        }
        if (!m_Commands.empty()) {
            ss << " <command> [options]";
        }
        return ss.str();
    }

    void m_clearErrors() { m_errors.clear(); }
    void m_addError(const std::string &vError) { m_errors.push_back("Error : " + vError); }

    std::string m_getHelpDetails(  //
        const std::string &vPositionalHeader,
        const std::string &vOptionalHeader,
        const std::string &vCommandHeader = "Commands") const {
        // collect infos with padding
        size_t first_col_size = 0U;
        std::vector<Argument::HelpCnt> cnt_pos;
        for (const auto &arg : m_Positionals) {
            cnt_pos.push_back(arg.m_getHelp(true, first_col_size));
        }
        std::vector<Argument::HelpCnt> cnt_opt;
        for (const auto &opt : m_Optionals) {
            cnt_opt.push_back(opt.m_getHelp(false, first_col_size));
        }
        // NEW: Collecter les commandes
        std::vector<Argument::HelpCnt> cnt_cmd;
        for (const auto &cmd : m_Commands) {
            cnt_cmd.push_back(cmd.m_getHelp(false, first_col_size));
        }

        // display
        first_col_size += 4U;
        std::stringstream ss;
        if (!cnt_pos.empty()) {
            ss << std::endl << " " << vPositionalHeader << " : " << std::endl;
            for (const auto &it : cnt_pos) {
                ss << it.first << std::string(first_col_size - it.first.size(), ' ') << it.second << std::endl;
            }
        }
        if (!cnt_opt.empty()) {
            ss << std::endl << " " << vOptionalHeader << " : " << std::endl;
            for (const auto &it : cnt_opt) {
                ss << it.first << std::string(first_col_size - it.first.size(), ' ') << it.second << std::endl;
            }
        }
        // NEW: Afficher les commandes avec leurs sous-arguments
        if (!cnt_cmd.empty()) {
            ss << std::endl << " " << vCommandHeader << " : " << std::endl;
            size_t cmd_idx = 0;
            for (const auto &it : cnt_cmd) {
                ss << it.first << std::string(first_col_size - it.first.size(), ' ') << it.second << std::endl;
                // Afficher les sous-arguments de cette commande
                const auto &cmd = m_Commands.at(cmd_idx);
                if (!cmd.m_subPositionals.empty() || !cmd.m_subOptionals.empty()) {
                    ss << cmd.getCommandHelp(first_col_size);
                }
                ++cmd_idx;
            }
        }
        return ss.str();
    }

    // remove the first '-' of a token
    std::string m_trim(const std::string &vToken) {
        auto short_last_minus = vToken.find_first_not_of("-");
        if (short_last_minus != std::string::npos) {
            return vToken.substr(short_last_minus);
        }
        return {};
    }

    template <typename T>
    T m_convertString(const std::string &str) const {
        std::istringstream iss(str);
        T value;
        if (!(iss >> value)) {
            throw std::runtime_error("Conversion failed");
        }
        return value;
    }
};

template <>
inline bool Args::m_convertString<bool>(const std::string &str) const {
    if (str == "true" || str == "1") {
        return true;
    } else if (str == "false" || str == "0") {
        return false;
    }
    throw std::runtime_error("Invalid boolean string");
}

}  // namespace ez
