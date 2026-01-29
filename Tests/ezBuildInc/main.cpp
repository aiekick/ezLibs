/*
MIT License

Copyright (c) 2022-2024 Stephane Cuillerdier (aka aiekick)

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

#include <ezlibs/ezApp.hpp>
#include <ezlibs/ezArgs.hpp>
#include <ezlibs/ezFigFont.hpp>
#include <ezlibs/ezBuildInc.hpp>

int main(int vArgc, char* vArgv[]) {
    ez::App app(vArgc, vArgv);
    ez::Args args("BuidInc");
    args.addPositional("project").help("prefix of the build id", "<project_name>");
    args.addPositional("file").help("file of the build id", "<file_path_name>");
    args.addOptional("--label").help("label of the project", "<label>").delimiter(' ');
    args.addOptional("-ff/--figfont").help("FigFont file; will add a FigFont based label", "<figFont_file>").delimiter(' ');
    args.addOptional("--no-help").help("will not print the help if the required arguments are not set", {});
    args.addOptional("--jsfile").help("Will generate also a javascript version file", "<js_file_path>").delimiter(' ');
    if (args.parse(vArgc, vArgv)) {
        std::string project = args.getValue<std::string>("project");
        std::string label = args.getValue<std::string>("label");
        if (label.empty()) {
            label = project;
        }
        std::string figFontFile = args.getValue<std::string>("figfont");
        std::string file = args.getValue<std::string>("file");
        std::string jsfile = args.getValue<std::string>("jsfile");
        if (!file.empty()) {
            ez::BuildInc builder(file);
            builder.setProject(project).setLabel(label).setJsFilePath(jsfile).setFigFontFile(figFontFile);
            builder.incBuildNumber().write().printInfos();
        } else {
            if (!args.isPresent("no-help")) {
                args.printHelp();
            }
        }
    } else {
        args.printErrors(" - ");
        std::cout << "See Help : \n";
        args.printHelp();
    }
    return 0;
}
