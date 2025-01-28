#include "TestEzCmdProcessor.h"
#include <ezlibs/ezCmdProcessor.hpp>
#include <ezlibs/ezCTest.hpp>
#include <string>
#include <utility>

// Desactivation des warnings de conversion
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)  // Conversion from 'double' to 'float', possible loss of data
#pragma warning(disable : 4305)  // Truncation from 'double' to 'float'
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

struct Cmd {
    ez::CmdProcessor::Command cmd;
    ez::CmdProcessor::Arguments args;
    ez::CmdProcessor::ProcessedCommand processed;
    char delimiter{';'};
    Cmd(ez::CmdProcessor::Command  cmd,
        ez::CmdProcessor::Arguments  args,
        ez::CmdProcessor::ProcessedCommand  processed,
        char delimiter = ';')
        : cmd(std::move(cmd)), args(std::move(args)), processed(std::move(processed)), delimiter(delimiter) {}
};

bool TestEzCmdProcessor_Basis() {
    std::vector<Cmd> good_commands{
        Cmd("ManyArgsCmd", {"23.5", "50.6", "0.8", "0.9", "0.7", "1.0"}, "ManyArgsCmd(;23.5;50.6;0.8;0.9;0.7;1.0)", ';'),
        Cmd("ManyArgsCmd2", {"120.6", "100.75"}, "ManyArgsCmd2(@120.6@100.75)", '@'),
        Cmd("ManyEmptyArgsCmd", {"", "", ""}, "ManyEmptyArgsCmd(%%%)",'%'),
        Cmd("OneArgCmd", {"120.6"}, "OneArgCmd(120.6)"),
        Cmd("NoArgCmd", {}, "NoArgCmd()")
    };

    ez::CmdProcessor cmdProc;

    // registering
    ez::CmdProcessor::Command lambdaCmd;
    ez::CmdProcessor::Arguments lambdaArgs;
    for (const auto& cmd : good_commands) {
        auto cmdRegistered = cmdProc.registerCmd(
            cmd.cmd,  //
            [&lambdaCmd, &lambdaArgs](const ez::CmdProcessor::Command& vCmd, const ez::CmdProcessor::Arguments& vArgs) {
                lambdaCmd = vCmd;
                lambdaArgs = vArgs;
            });
        CTEST_ASSERT_MESSAGE(cmdRegistered, "Cmd Registered ?");
    }
    
    // failed registering conditions 
    const auto lambda = [](const ez::CmdProcessor::Command& vCmd, const ez::CmdProcessor::Arguments& vArgs) {};
    CTEST_ASSERT_MESSAGE(!cmdProc.registerCmd({}, lambda), "Failing to register empty command ?");
    CTEST_ASSERT_MESSAGE(!cmdProc.isCmdRegistered({}), "Cmd is not registered ?");
    CTEST_ASSERT_MESSAGE(!cmdProc.registerCmd("toto", {}), "Failing to register 'toto'cmd with empty functor ?");
    CTEST_ASSERT_MESSAGE(!cmdProc.isCmdRegistered("toto"), "Cmd 'toto' is not registered ?");
    
    // check registering
    for (const auto& cmd : good_commands) {
        CTEST_ASSERT_MESSAGE(cmdProc.isCmdRegistered(cmd.cmd), "Cmd registered sucessfully");
    }
    
    // encoding command
    std::vector<ez::CmdProcessor::ProcessedCommand> processedCommands;
    for (const auto& cmd : good_commands) {
        const auto processedCmd = cmdProc.encode(cmd.cmd, cmd.args, cmd.delimiter);
        CTEST_ASSERT_MESSAGE(processedCmd == cmd.processed, "Processed Cmd is the good one ?");
        processedCommands.push_back(processedCmd);
    }

    // failed encoding command if delimiter is container is some args
    CTEST_ASSERT_MESSAGE(cmdProc.encode("toto", {"eee", "tete"}, 'e').empty(), "Cmd 'toto' failing to encode since delimiter is rpesent in some args ?");
    
    // decoding command
    size_t idx = 0;
    for (const auto& processedCmd : processedCommands) {
        Cmd lambdaAttendeeCmd = good_commands.at(idx++);
        lambdaCmd.clear();
        lambdaArgs.clear();
        CTEST_ASSERT_MESSAGE(cmdProc.decode(processedCmd), "Cmd decoded ?");
        // lambdaCmd and lambdaArgs are filled by the lambda called by cmdProc.decode
        CTEST_ASSERT_MESSAGE(lambdaCmd == lambdaAttendeeCmd.cmd, "Cmd is the good one ?");
        CTEST_ASSERT_MESSAGE(lambdaArgs.size() == lambdaAttendeeCmd.args.size(), "Cmd args have the good count ?");
        for (size_t idx = 0; idx < lambdaArgs.size(); ++idx) {
            CTEST_ASSERT_MESSAGE(lambdaArgs.at(idx) == lambdaAttendeeCmd.args.at(idx), "same arg at same index ?");
        }
    }
    
    // un registering
    for (const auto& cmd : good_commands) {
        CTEST_ASSERT_MESSAGE(cmdProc.isCmdRegistered(cmd.cmd), "Cmd registered ?");
        CTEST_ASSERT_MESSAGE(cmdProc.unRegisterCmd(cmd.cmd), "Cmd unregistered ?");
        CTEST_ASSERT_MESSAGE(!cmdProc.isCmdRegistered(cmd.cmd), "Cmd not registered ?");
    }

    // decoding command faling since unregistered
    for (const auto& processedCmd : processedCommands) {
        CTEST_ASSERT_MESSAGE(!cmdProc.decode(processedCmd), "Cmd not decoded ?");
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzCmdProcessor(const std::string& vTest) {
    IfTestExist(TestEzCmdProcessor_Basis);
    return false;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
