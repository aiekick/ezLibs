
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

#include <TestEzExprParsings.h>
#include <TestEzExprBuiltins.h>
#include <TestEzExprConstants.h>
#include <TestEzExprExceptions.h>
#include <TestEzExprPerfos.h>

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestCollectionExist(v)             \
    if (vTest.find(#v) != std::string::npos) \
    return v(vTest)

bool TestEzExpr(const std::string& vTest) {
    IfTestCollectionExist(TestEzExpr_Parsings);
    IfTestCollectionExist(TestEzExpr_Constants);
    IfTestCollectionExist(TestEzExpr_Builtins);
    IfTestCollectionExist(TestEzExpr_Exceptions);
    IfTestCollectionExist(TestEzExpr_Perfos);
    // default
    return false;
}