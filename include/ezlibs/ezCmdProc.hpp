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

// ezCmdProc is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

namespace ez {
    /*
    this class will help process commands like serialize/deserialize operations
    usually for :
    - server/client communication
    - undo/redo operation
    - ..
    */

    class CmdProcessor {
    public:
        typedef std::string Command;
        typedef std::string ProcessedCommand;
        typedef std::vector<std::string> Arguments;
        typedef std::function<void(const Command &, const Arguments &)> CmdFunctor;

    private:
        std::unordered_map<Command, CmdFunctor> m_cmdFunctors;

        /* a processed commande will have this format : (space are not part of the process command, jsut for syntax clarity)
        * COMMAND(DELIMITER ARG1 DELIMITER ARG2 DELIMITER ARG3)
        *
        * if not argument are sent :
        * COMMAND()
        *
        * if one argument is sent :
        * COMMAND(ARG1)
        */

    public:
        /// <summary>
        /// will encode a command ready to send
        /// the encoding will fail if the delimiter exist in args words
        /// </summary>
        /// <param name="vCmd">the command</param>
        /// <param name="vArgs">the command arguments</param>
        /// <returns>the processed command</returns>
        ProcessedCommand encode(const Command &vCmd, Arguments vArgs, char vDelimiter = ';') {
            ProcessedCommand ret;
            if (!vCmd.empty()) {
                if (vArgs.empty()) {
                    // no args
                    ret = vCmd + "()";
                } else {
                    ret = vCmd + "(";
                    bool first = true;
                    if (vArgs.size() == 1U) {
                        // one args
                        ret += *vArgs.begin();
                    } else {
                        for (const auto &arg: vArgs) {
                            // many args
                            if (arg.find(vDelimiter) != std::string::npos) {
                                // found
                                return {};
                            }
                            ret += vDelimiter + arg;
                            first = false;
                        }
                    }
                    ret += ")";
                }
            } else {
#ifdef EZ_TOOLS_LOG
                LogVarError("Err : the cmd is empty");
#endif // EZ_TOOLS_LOG
            }
            return ret;
        }

        /// <summary>
        /// will decode a command and call the registered functor
        /// </summary>
        /// <param name="vCmd"></param>
        /// <returns>true is sucessfully decoded</returns>
        bool decode(const ProcessedCommand &vCmd) {
            bool ret = false;
            if (!vCmd.empty()) {
                auto first_par = vCmd.find('(');
                if (first_par != std::string::npos) {
                    const Command cmd = vCmd.substr(0, first_par);
                    const auto end_par = vCmd.find(')', first_par);
                    if (end_par != std::string::npos) {
                        if (m_cmdFunctors.find(cmd) != m_cmdFunctors.end()) {
                            auto functor = m_cmdFunctors.at(cmd);
                            if (functor != nullptr) {
                                // normally impossible to have null functor
                                ret = true;
                                ++first_par;
                                Arguments args;
                                if (first_par != end_par) {
                                    char delimiter = vCmd.at(first_par);
                                    auto next_sep = vCmd.find(delimiter, first_par + 1);
                                    if (next_sep == std::string::npos) {
                                        // one arg
                                        args.push_back(vCmd.substr(first_par, end_par - first_par));
                                    } else {
                                        // many args
                                        size_t last_sep = first_par + 1; // we go after the delimiter
                                        do {
                                            args.push_back(vCmd.substr(last_sep, next_sep - last_sep));
                                            last_sep = next_sep + 1;
                                        } while ((next_sep = vCmd.find(delimiter, last_sep)) != std::string::npos);
                                        args.push_back(vCmd.substr(last_sep, end_par - last_sep));
                                    }
                                }
                                // call functor
                                functor(cmd, args);
                            }
                        }
                    }
                }
            }
            return ret;
        }

        /// <summary>
        /// will check is a commande is registered. si a functor is available for this command
        /// </summary>
        /// <param name="vCmd">the command to check</param>
        /// <returns>true if registered</returns>
        bool isCmdRegistered(const Command &vCmd) { return (m_cmdFunctors.find(vCmd) != m_cmdFunctors.end()); }

        /// <summary>
        /// will register a functor for a command
        /// </summary>
        /// <param name="vCmd">the commande to register</param>
        /// <returns>true if successfully registered. false if command is empty or functor is null</returns>
        bool registerCmd(const Command &vCmd, const CmdFunctor &vFunctor) {
            bool ret = false;
            if (!vCmd.empty() && vFunctor != nullptr) {
                if (m_cmdFunctors.find(vCmd) == m_cmdFunctors.end()) {
                    // not found
                    m_cmdFunctors[vCmd] = vFunctor;
                    ret = true;
                }
            }
            return ret;
        }

        /// <summary>
        /// will unregister the command. the functor will be removed for this command
        /// </summary>
        /// <param name="vCmd">the commande to unregister</param>
        /// <returns>true is successfully unregistered; false if the commande was not registered before</returns>
        bool unRegisterCmd(const Command &vCmd) {
            bool ret = false;
            if (m_cmdFunctors.find(vCmd) != m_cmdFunctors.end()) {
                // not found
                m_cmdFunctors.erase(vCmd);
                ret = true;
            }
            return ret;
        }
    };
} // namespace ez
