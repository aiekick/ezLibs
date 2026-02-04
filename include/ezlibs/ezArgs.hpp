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
#include <cstdio>
#include <cstdint>
#include <iostream>
#include <algorithm>
#include <limits>
#include <set>
#include <sstream>

#include "ezStr.hpp"
#include "ezLog.hpp"

namespace ez {

class Args {
private:
    class Argument {
        friend class Args;
        friend class CommandArgument;

    protected:
        std::vector<std::string> m_base_args;
        std::set<std::string> m_full_args;
        char m_one_char_arg = 0;
        std::string m_one_char_prefix;
        std::string m_help_text;
        std::string m_help_var_name;
        std::string m_type;
        char m_delimiter = 0;
        bool m_is_present = false;
        bool m_has_value = false;
        std::string m_value;
        bool m_is_array = false;
        size_t m_array_min_count = 0;
        size_t m_array_max_count = std::numeric_limits<size_t>::max();
        std::vector<std::string> m_array_values;

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

        HelpCnt m_getHelp(bool vPositional, size_t &vInOutFirstColSize, const std::string &vIndent) const {
            HelpCnt res;
            std::stringstream ss;
            if (vPositional) {
                std::string token = m_help_var_name;
                if (token.empty()) {
                    token = *(m_base_args.begin());
                }
                ss << vIndent << token;
                m_getHelpArray(ss, {});
            } else {
                size_t idx = 0;
                ss << vIndent;
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

    class CommandArgument final : public Argument {
        friend class Args;

    private:
        std::vector<PositionalArgument> m_subPositionals;
        std::vector<OptionalArgument> m_subOptionals;

    public:
        CommandArgument &help(const std::string &vHelp, const std::string &vVarName) { return Argument::help<CommandArgument>(vHelp, vVarName); }

        PositionalArgument &addPositional(const std::string &vKey) {
            PositionalArgument res;
            res.m_base_args = ez::str::splitStringToVector(vKey, '/');
            for (const auto &a : res.m_base_args) {
                res.m_full_args.emplace(a);
            }
            m_subPositionals.push_back(res);
            return m_subPositionals.back();
        }

        OptionalArgument &addOptional(const std::string &vKey) {
            OptionalArgument res;
            res.m_base_args = ez::str::splitStringToVector(vKey, '/');
            for (const auto &a : res.m_base_args) {
                res.m_full_args.emplace(a);
                auto pos = a.find_first_not_of("-");
                if (pos != std::string::npos) {
                    res.m_full_args.emplace(a.substr(pos));
                }
                if (res.m_one_char_arg == 0) {
                    Args::m_extractOneChar(res, a);
                }
            }
            m_subOptionals.push_back(res);
            return m_subOptionals.back();
        }

    private:
        std::string getCommandHelp(size_t vFirstColSize, const std::string &vIndent) const {
            std::stringstream ss;
            for (const auto &pos : m_subPositionals) {
                auto helpPair = pos.m_getHelp(true, vFirstColSize, vIndent);
                ss << vIndent << helpPair.first;
                if (helpPair.first.size() < vFirstColSize) {
                    ss << std::string(vFirstColSize - helpPair.first.size(), ' ');
                }
                ss << helpPair.second << std::endl;
            }
            for (const auto &opt : m_subOptionals) {
                auto helpPair = opt.m_getHelp(false, vFirstColSize, vIndent);
                ss << vIndent << helpPair.first;
                if (helpPair.first.size() < vFirstColSize) {
                    ss << std::string(vFirstColSize - helpPair.first.size(), ' ');
                }
                ss << helpPair.second << std::endl;
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
    std::vector<CommandArgument> m_Commands;
    CommandArgument *m_ActiveCommand = nullptr;
    std::vector<std::string> m_errors;

public:
    Args() = default;
    virtual ~Args() = default;

    bool init(const std::string &vName, const std::string &vHelpOptionalKey) {
        if (vName.empty()) {
            m_addError("Name cant be empty");
            return false;
        }
        m_AppName = vName;
        m_addOptional(m_HelpArgument, vHelpOptionalKey).help("Show the usage", {});
        return true;
    }

    Args(const std::string &vName, const std::string &vHelpOptionalKey = "-h/--help") { init(vName, vHelpOptionalKey); }

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
        PositionalArgument res;
        if (vKey.empty()) {
            m_addError("Positional argument cant be empty");
            m_Positionals.push_back(res);
            return m_Positionals.back();
        }
        res.m_base_args = ez::str::splitStringToVector(vKey, '/');
        for (const auto &a : res.m_base_args) {
            res.m_full_args.emplace(a);
        }
        m_Positionals.push_back(res);
        return m_Positionals.back();
    }

    OptionalArgument &addOptional(const std::string &vKey) {
        OptionalArgument res;
        if (vKey.empty()) {
            m_addError("Optional argument cant be empty");
            m_Optionals.push_back(res);
            return m_Optionals.back();
        }
        m_addOptional(res, vKey);
        m_Optionals.push_back(res);
        return m_Optionals.back();
    }

    CommandArgument &addCommand(const std::string &vKey) {
        CommandArgument res;
        if (vKey.empty()) {
            m_addError("Command cant be empty");
            m_Commands.push_back(res);
            return m_Commands.back();
        }
        res.m_base_args = ez::str::splitStringToVector(vKey, '/');
        for (const auto &a : res.m_base_args) {
            res.m_full_args.emplace(a);
        }
        m_Commands.push_back(res);
        return m_Commands.back();
    }

    const CommandArgument *getActiveCommand() const { return m_ActiveCommand; }

    bool isCommand(const std::string &vKey) const {
        if (m_ActiveCommand == nullptr) {
            return false;
        }
        return m_ActiveCommand->m_full_args.find(vKey) != m_ActiveCommand->m_full_args.end();
    }

    bool isPresent(const std::string &vKey) const {
        auto *ptr = m_getArgumentPtr(vKey);
        return (ptr != nullptr) ? ptr->m_is_present : false;
    }

    bool hasValue(const std::string &vKey) const {
        auto *ptr = m_getArgumentPtr(vKey);
        return (ptr != nullptr) ? ptr->m_has_value : false;
    }

    bool isArray(const std::string &vKey) const {
        auto *ptr = m_getArgumentPtr(vKey);
        return (ptr != nullptr) ? ptr->m_is_array : false;
    }

    template <typename T>
    T getValue(const std::string &vKey) const {
        auto *ptr = m_getArgumentPtr(vKey);
        if (ptr != nullptr && !ptr->m_value.empty()) {
            return m_convertString<T>(ptr->m_value);
        }
        return {};
    }

    std::vector<std::string> getArrayValues(const std::string &vKey) const {
        auto *ptr = m_getArgumentPtr(vKey);
        if (ptr != nullptr && ptr->m_is_array) {
            return ptr->m_array_values;
        }
        return {};
    }

    template <typename T>
    std::vector<T> getArrayValues(const std::string &vKey) const {
        auto strValues = getArrayValues(vKey);
        std::vector<T> result;
        result.reserve(strValues.size());
        for (const auto &val : strValues) {
            result.push_back(m_convertString<T>(val));
        }
        return result;
    }

    void getHelp(
        std::ostream &vOs,
        const std::string &vIndent,
        const std::string &vPositionalHeader = "Positionnal arguments",
        const std::string &vOptionalHeader = "Optional arguments",
        const std::string &vCommandHeader = "Commands") const {
        if (!m_HelpHeader.empty()) {
            vOs << m_HelpHeader << std::endl << std::endl;
        }
        vOs << m_getCmdLineHelp();
        vOs << std::endl;
        if (!m_HelpDescription.empty()) {
            vOs << std::endl << vIndent << m_HelpDescription << std::endl;
        }
        vOs << m_getHelpDetails(vIndent, vPositionalHeader, vOptionalHeader, vCommandHeader);
        if (!m_HelpFooter.empty()) {
            vOs << std::endl << m_HelpFooter << std::endl;
        }
    }

    void printHelp(
        std::ostream &vOs = std::cout,
        const std::string &vIndent = "  ",
        const std::string &vPositionalHeader = "Positionnal arguments",
        const std::string &vOptionalHeader = "Optional arguments",
        const std::string &vCommandHeader = "Commands") const {
        getHelp(vOs, vIndent, vPositionalHeader, vOptionalHeader, vCommandHeader);
        vOs << std::endl;
    }

    void printErrors(const std::string &vIndent = " - ") const {
        for (const auto &error : m_errors) {
            std::cout << vIndent << error << std::endl;
        }
    }

    bool parse(const int32_t vArgc, char **vArgv, const int32_t vStartIdx = 1U) {
        size_t positional_idx = 0;
        m_ActiveCommand = nullptr;

        for (int32_t idx = vStartIdx; idx < vArgc; ++idx) {
            std::string arg = vArgv[idx];
            std::string trimmedArg = m_trim(arg);

            // Check help
            if (m_HelpArgument.m_full_args.find(trimmedArg) != m_HelpArgument.m_full_args.end()) {
                printHelp();
                return false;
            }

            // Check if it's a command
            bool is_command = false;
            if (m_ActiveCommand == nullptr) {
                for (auto &cmd : m_Commands) {
                    if (cmd.m_full_args.find(trimmedArg) != cmd.m_full_args.end()) {
                        cmd.m_is_present = true;
                        m_ActiveCommand = &cmd;
                        is_command = true;
                        break;
                    }
                }
            }
            if (is_command) {
                continue;
            }

            // Check optionals
            std::string token = trimmedArg;
            std::string value;
            bool is_optional = false;

            // Handle combined short args like -blm or #fbc
            // Extract the non-alphanumeric prefix, then check if each remaining char is a known one_char_arg with that prefix
            // But first, skip this if the full arg is already a known optional (e.g., -ff defined as its own option)
            {
                auto alnum_pos = arg.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
                if (alnum_pos != std::string::npos && alnum_pos > 0) {
                    std::string prefix = arg.substr(0, alnum_pos);
                    std::string suffix = arg.substr(alnum_pos);
                    if (suffix.size() > 1 && !m_isKnownFullArgument(arg)) {
                        // More than one alnum char after prefix and not a known full option: could be combined short args
                        bool all_short = true;
                        char unknown_char = 0;
                        for (const auto &ch : suffix) {
                            if (!m_isShortArg(prefix, ch)) {
                                all_short = false;
                                unknown_char = ch;
                                break;
                            }
                        }
                        if (all_short) {
                            for (const auto &ch : suffix) {
                                m_markShortArgPresent(prefix, ch);
                            }
                            is_optional = true;
                        } else {
                            // Not all chars are known short args with this prefix.
                            // Don't treat as combined short args, let it fall through to normal optional matching.
                            // This allows -ff to be matched as a normal optional if defined.
                        }
                    }
                }
            }

            // Check sub-optionals of active command
            if (!is_optional && m_ActiveCommand != nullptr) {
                for (auto &arg_ref : m_ActiveCommand->m_subOptionals) {
                    if (m_matchOptional(arg_ref, token, value)) {
                        arg_ref.m_is_present = true;
                        is_optional = true;
                        m_parseOptionalValue(&arg_ref, idx, vArgc, vArgv, value);
                        break;
                    }
                }
            }

            // Check global optionals
            if (!is_optional) {
                token = trimmedArg;
                for (auto &arg_ref : m_Optionals) {
                    if (m_matchOptional(arg_ref, token, value)) {
                        arg_ref.m_is_present = true;
                        is_optional = true;
                        m_parseOptionalValue(&arg_ref, idx, vArgc, vArgv, value);
                        break;
                    }
                }
            }

            // Check positionals
            if (!is_optional) {
                if (positional_idx < m_Positionals.size()) {
                    auto &positional = m_Positionals.at(positional_idx);
                    if (!m_parsePositionalValue(positional, arg, idx, vArgc, vArgv)) {
                        return false;
                    }
                    ++positional_idx;
                } else if (m_ActiveCommand != nullptr) {
                    size_t sub_pos_idx = positional_idx - m_Positionals.size();
                    if (sub_pos_idx < m_ActiveCommand->m_subPositionals.size()) {
                        auto &positional = m_ActiveCommand->m_subPositionals.at(sub_pos_idx);
                        if (!m_parsePositionalValue(positional, arg, idx, vArgc, vArgv)) {
                            return false;
                        }
                        ++positional_idx;
                    } else {
                        m_addError("Unknown argument: " + arg);
                    }
                } else {
                    m_addError("Unknown argument: " + arg);
                }
            }
        }

        // Validate global positionals
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

        // Validate global optionals
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

        // Validate active command sub-arguments
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

    const std::vector<std::string> &getErrors() const { return m_errors; }
    bool hasErrors() const { return !m_errors.empty(); }

private:
    // Search in active command first, then in global arguments
    const Argument *m_getArgumentPtr(const std::string &vKey) const {
        if (m_ActiveCommand != nullptr) {
            for (const auto &opt : m_ActiveCommand->m_subOptionals) {
                if (opt.m_full_args.find(vKey) != opt.m_full_args.end()) {
                    return &opt;
                }
            }
            for (const auto &pos : m_ActiveCommand->m_subPositionals) {
                if (pos.m_full_args.find(vKey) != pos.m_full_args.end()) {
                    return &pos;
                }
            }
        }
        for (const auto &arg : m_Positionals) {
            if (arg.m_full_args.find(vKey) != arg.m_full_args.end()) {
                return &arg;
            }
        }
        for (const auto &arg : m_Optionals) {
            if (arg.m_full_args.find(vKey) != arg.m_full_args.end()) {
                return &arg;
            }
        }
        return nullptr;
    }

    Argument *m_getArgumentPtr(const std::string &vKey) { return const_cast<Argument *>(static_cast<const Args *>(this)->m_getArgumentPtr(vKey)); }

    // Extract the non-alphanumeric prefix and check if the remaining part is a single char.
    // If so, the argument is "combinable" (can be grouped like -abc or #fbc).
    // Examples: "-f" -> prefix="-", char='f' | "-ff" -> not combinable | "#f" -> prefix="#", char='f'
    static void m_extractOneChar(OptionalArgument &vArg, const std::string &a) {
        auto pos = a.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
        if (pos != std::string::npos && pos > 0) {
            std::string prefix = a.substr(0, pos);
            std::string suffix = a.substr(pos);
            if (suffix.size() == 1) {
                vArg.m_one_char_arg = suffix[0];
                vArg.m_one_char_prefix = prefix;
            }
        }
    }

    OptionalArgument &m_addOptional(OptionalArgument &vInOutArgument, const std::string &vKey) {
        vInOutArgument.m_base_args = ez::str::splitStringToVector(vKey, '/');
        for (const auto &a : vInOutArgument.m_base_args) {
            vInOutArgument.m_full_args.emplace(a);
            auto pos = a.find_first_not_of("-");
            if (pos != std::string::npos) {
                vInOutArgument.m_full_args.emplace(a.substr(pos));
            }
            if (vInOutArgument.m_one_char_arg == 0) {
                m_extractOneChar(vInOutArgument, a);
            }
        }
        return vInOutArgument;
    }

    // Check if a char is a known short arg for the given prefix
    bool m_isShortArg(const std::string &vPrefix, char c) const {
        if (m_HelpArgument.m_one_char_arg == c && m_HelpArgument.m_one_char_prefix == vPrefix) {
            return true;
        }
        for (const auto &opt : m_Optionals) {
            if (opt.m_one_char_arg == c && opt.m_one_char_prefix == vPrefix) {
                return true;
            }
        }
        if (m_ActiveCommand != nullptr) {
            for (const auto &opt : m_ActiveCommand->m_subOptionals) {
                if (opt.m_one_char_arg == c && opt.m_one_char_prefix == vPrefix) {
                    return true;
                }
            }
        }
        return false;
    }

    // Mark a short arg as present
    void m_markShortArgPresent(const std::string &vPrefix, char c) {
        for (auto &opt : m_Optionals) {
            if (opt.m_one_char_arg == c && opt.m_one_char_prefix == vPrefix) {
                opt.m_is_present = true;
                return;
            }
        }
        if (m_ActiveCommand != nullptr) {
            for (auto &opt : m_ActiveCommand->m_subOptionals) {
                if (opt.m_one_char_arg == c && opt.m_one_char_prefix == vPrefix) {
                    opt.m_is_present = true;
                    return;
                }
            }
        }
    }

    // Match an optional argument
    bool m_matchOptional(OptionalArgument &arg_ref, std::string &token, std::string &value) {
        if (arg_ref.m_delimiter != 0 && arg_ref.m_delimiter != ' ') {
            if (token.find(arg_ref.m_delimiter) != std::string::npos) {
                auto arr = ez::str::splitStringToVector(token, arg_ref.m_delimiter);
                if (arr.size() == 2) {
                    token = arr.at(0);
                    value = arr.at(1);
                }
            }
        }
        if (arg_ref.m_one_char_arg != 0 && token.size() == 1 && token[0] == arg_ref.m_one_char_arg) {
            return true;
        }
        if (arg_ref.m_full_args.find(token) != arg_ref.m_full_args.end()) {
            return true;
        }
        return false;
    }

    void m_parseOptionalValue(OptionalArgument *opt, int32_t &idx, int32_t vArgc, char **vArgv, const std::string &value) {
        if (opt->m_is_array) {
            size_t count = 0;
            if (opt->m_delimiter == ' ') {
                while ((idx + 1) < vArgc && count < opt->m_array_max_count) {
                    std::string nextArg = m_trim(vArgv[idx + 1]);
                    if (m_isKnownArgument(nextArg) || m_isKnownArgument(vArgv[idx + 1])) {
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
                    if (!m_isKnownArgument(nextArg) && !m_isKnownArgument(vArgv[idx + 1])) {
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

    bool m_parsePositionalValue(PositionalArgument &pos, const std::string &arg, int32_t &idx, int32_t vArgc, char **vArgv) {
        if (!arg.empty() && arg[0] == '-') {
            m_addError("Unexpected option in positional argument: " + arg);
            return false;
        }
        pos.m_is_present = true;
        if (pos.m_is_array) {
            pos.m_array_values.push_back(arg);
            size_t count = 1;
            while ((idx + 1) < vArgc && count < pos.m_array_max_count) {
                std::string next = vArgv[idx + 1];
                if (!next.empty() && next[0] == '-') {
                    break;
                }

                std::string next_arg = m_trim(vArgv[idx + 1]);
                if (m_isKnownArgument(next_arg) || m_isKnownArgument(vArgv[idx + 1])) {
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
        return true;
    }

    bool m_isKnownArgument(const std::string &arg) const {
        // Check combined short args like -blm or #fbc
        auto alnum_pos = arg.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
        if (alnum_pos != std::string::npos && alnum_pos > 0) {
            std::string prefix = arg.substr(0, alnum_pos);
            std::string suffix = arg.substr(alnum_pos);
            if (suffix.size() > 1) {
                for (const auto &ch : suffix) {
                    if (m_isShortArg(prefix, ch)) {
                        return true;  // At least one valid short arg
                    }
                }
            }
        }
        return m_isKnownFullArgument(arg);
    }

    // Check if the argument matches a fully defined optional/command (no combined short arg detection)
    bool m_isKnownFullArgument(const std::string &arg) const {
        std::string trimmed = m_trim(arg);
        for (const auto &opt : m_Optionals) {
            if (opt.m_full_args.find(trimmed) != opt.m_full_args.end()) {
                return true;
            }
        }
        for (const auto &cmd : m_Commands) {
            if (cmd.m_full_args.find(trimmed) != cmd.m_full_args.end()) {
                return true;
            }
        }
        if (m_ActiveCommand != nullptr) {
            for (const auto &opt : m_ActiveCommand->m_subOptionals) {
                if (opt.m_full_args.find(trimmed) != opt.m_full_args.end()) {
                    return true;
                }
            }
        }
        if (m_HelpArgument.m_full_args.find(trimmed) != m_HelpArgument.m_full_args.end()) {
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
        ss << "usage : " << m_AppName;
        m_getCmdLineOptional(m_HelpArgument, ss);
        for (const auto &arg : m_Optionals) {
            m_getCmdLineOptional(arg, ss);
        }
        for (const auto &arg : m_Positionals) {
            m_getCmdLinePositional(arg, ss);
        }
        if (!m_Commands.empty()) {
            ss << " <command> [<options>]";
        }
        return ss.str();
    }

    void m_addError(const std::string &vError) { m_errors.push_back("Error : " + vError); }

    std::string m_getHelpDetails(const std::string &vIndent, const std::string &vPositionalHeader, const std::string &vOptionalHeader, const std::string &vCommandHeader)
        const {
        size_t first_col_size = 0U;

        // right column sizes
        std::vector<Argument::HelpCnt> cnt_pos;
        for (const auto &arg : m_Positionals) {
            cnt_pos.push_back(arg.m_getHelp(true, first_col_size, vIndent));
        }
        std::vector<Argument::HelpCnt> cnt_opt;
        for (const auto &opt : m_Optionals) {
            cnt_opt.push_back(opt.m_getHelp(false, first_col_size, vIndent));
        }
        std::vector<Argument::HelpCnt> cnt_cmd;
        for (const auto &cmd : m_Commands) {
            cnt_cmd.push_back(cmd.m_getHelp(false, first_col_size, vIndent));
            // Include sub-arguments in size calculation
            for (const auto &pos : cmd.m_subPositionals) {
                pos.m_getHelp(true, first_col_size, vIndent);
            }
            for (const auto &opt : cmd.m_subOptionals) {
                opt.m_getHelp(false, first_col_size, vIndent);
            }
        }

        // space between columns
        first_col_size += 4U;

        // left column
        std::stringstream ss;
        if (!cnt_pos.empty()) {
            ss << std::endl << vPositionalHeader << " :" << std::endl;
            for (const auto &it : cnt_pos) {
                ss << it.first << std::string(first_col_size - it.first.size(), ' ') << it.second << std::endl;
            }
        }
        if (!cnt_opt.empty()) {
            ss << std::endl << vOptionalHeader << " :" << std::endl;
            for (const auto &it : cnt_opt) {
                ss << it.first << std::string(first_col_size - it.first.size(), ' ') << it.second << std::endl;
            }
        }
        if (!cnt_cmd.empty()) {
            ss << std::endl << vCommandHeader << " :" << std::endl;
            size_t cmd_idx = 0;
            for (const auto &it : cnt_cmd) {
                ss << it.first << std::string(first_col_size - it.first.size(), ' ') << it.second << std::endl;
                const auto &cmd = m_Commands.at(cmd_idx);
                if (!cmd.m_subPositionals.empty() || !cmd.m_subOptionals.empty()) {
                    ss << cmd.getCommandHelp(first_col_size, vIndent);
                }
                ++cmd_idx;
            }
        }
        return ss.str();
    }

    std::string m_trim(const std::string &vToken) const {
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
        iss >> value;
        return value;
    }
};

template <>
inline bool Args::m_convertString<bool>(const std::string &str) const {
    return (str == "true" || str == "1");
}

}  // namespace ez
