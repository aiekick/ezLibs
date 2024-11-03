
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

#include <TestEzExprExceptions.h>
#include <ezlibs/ezCTest.hpp>
#include <ezlibs/ezExpr.hpp>
#include <ezlibs/ezLog.hpp>

////////////////////////////////////////////////////////////////////////////
//// ERROR CODES ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// Test for DIVISION_BY_ZERO exception
bool TestEzExpr_Exceptions_DivisionByZero() {
    ez::Expr ev;
    try {
        ev.parse("1 / 0").eval();  // Division by integer zero
        return false;              // Expected an exception
    } catch (const ez::ExprException& e) {
        CTEST_ASSERT(std::string(e.what()) == std::string("Division by zero"));
        CTEST_ASSERT(e.getCode() == ez::ErrorCode::DIVISION_BY_ZERO);
    }
    return true;
}

bool TestEzExpr_Exceptions_DivisionByZero_Float() {
    ez::Expr ev;
    try {
        ev.parse("5.5 / 0.0").eval();  // Division by floating-point zero
        return false;  // Expected an exception
    } catch (const ez::ExprException& e) {
        CTEST_ASSERT(std::string(e.what()) == std::string("Division by zero"));
        CTEST_ASSERT(e.getCode() == ez::ErrorCode::DIVISION_BY_ZERO);
    }
    return true;
}

bool TestEzExpr_Exceptions_ModuloByZero() {
    ez::Expr ev;
    try {
        ev.parse("10 % 0").eval();  // Modulo by zero
        return false;               // Expected an exception
    } catch (const ez::ExprException& e) {
        CTEST_ASSERT(std::string(e.what()) == std::string("Division by zero"));
        CTEST_ASSERT(e.getCode() == ez::ErrorCode::DIVISION_BY_ZERO);
    }
    return true;
}

// Test for UNMATCHED_PARENTHESIS exception
bool TestEzExpr_Exceptions_UnmatchedParenthesis_Open() {
    ez::Expr ev;
    try {
        ev.parse("1 + (2 * 3").eval();  // Unmatched opening parenthesis
        return false;                   // Expected an exception
    } catch (const ez::ExprException& e) {
        CTEST_ASSERT(std::string(e.what()) == std::string("Unmatched parenthesis"));
        CTEST_ASSERT(e.getCode() == ez::ErrorCode::UNMATCHED_PARENTHESIS);
    }
    return true;
}

bool TestEzExpr_Exceptions_UnmatchedParenthesis_Close() {
    ez::Expr ev;
    try {
        ev.parse("1 + 2) * 3").eval();  // Unmatched closing parenthesis
        return false;                   // Expected an exception
    } catch (const ez::ExprException& e) {
        CTEST_ASSERT(std::string(e.what()) == std::string("Unmatched parenthesis found at the end of the expression"));
        CTEST_ASSERT(e.getCode() == ez::ErrorCode::UNMATCHED_PARENTHESIS);
    }
    return true;
}

// Test for VARIABLE_NOT_FOUND exception
bool TestEzExpr_Exceptions_VariableNotFound() {
    ez::Expr ev;
    try {
        ev.parse("x + 1").eval();  // 'x' is not defined
        return false;              // Expected an exception
    } catch (const ez::ExprException& e) {
        CTEST_ASSERT(std::string(e.what()) == std::string("Variable not found: x"));
        CTEST_ASSERT( e.getCode() == ez::ErrorCode::VARIABLE_NOT_FOUND);
    }
    return true;
}

bool TestEzExpr_Exceptions_VariableNotFound_Multiple() {
    ez::Expr ev;
    try {
        ev.parse("y + x + 1").eval();  // 'x' and 'y' are not defined
        return false;                  // Expected an exception
    } catch (const ez::ExprException& e) {
        CTEST_ASSERT(std::string(e.what()) == std::string("Variable not found: y"));
        CTEST_ASSERT(e.getCode() == ez::ErrorCode::VARIABLE_NOT_FOUND);
    }
    return true;
}

// Test for FUNCTION_NOT_FOUND exception
bool TestEzExpr_Exceptions_FunctionNotFound() {
    ez::Expr ev;
    try {
        ev.parse("unknownFunc(1)").eval();  // 'unknownFunc' is not defined
        return false;                       // Expected an exception
    } catch (const ez::ExprException& e) {
        CTEST_ASSERT(std::string(e.what()) == std::string("Function not found: unknownFunc"));
        CTEST_ASSERT(e.getCode() == ez::ErrorCode::FUNCTION_NOT_FOUND);
    }
    return true;
}

bool TestEzExpr_Exceptions_FunctionWrongArgs() {
    ez::Expr ev;
    try {
        ev.parse("sin(1, 2)").eval();  // 'sin' is defined but with wrong number of arguments
        return false;                  // Expected an exception
    } catch (const ez::ExprException& e) {
        CTEST_ASSERT(std::string(e.what()) == std::string("Incorrect number of arguments for function: sin"));
        CTEST_ASSERT(e.getCode() == ez::ErrorCode::FUNCTION_WRONG_ARGUMENTS_COUNT);
    }
    return true;
}

// Test for EVALUATION_NAN exception
bool TestEzExpr_Exceptions_EvaluationNaN_1() {
    ez::Expr ev;
    try {
        ev.parse("sqrt(-1)").eval();  // sqrt of negative number leads to NaN
        return false;                 // Expected an exception
    } catch (const ez::ExprException& e) {
        CTEST_ASSERT(std::string(e.what()) == std::string("Result is NaN"));
        CTEST_ASSERT(e.getCode() == ez::ErrorCode::EVALUATION_NAN);
    }
    return true;
}
bool TestEzExpr_Exceptions_EvaluationNaN_2() {
    ez::Expr ev;
    try {
        ev.parse("(-1)^0.5").eval();  // sqrt of negative number leads to NaN
        return false;                 // Expected an exception
    } catch (const ez::ExprException& e) {
        CTEST_ASSERT(std::string(e.what()) == std::string("Pow base value is negative"));
        CTEST_ASSERT(e.getCode() == ez::ErrorCode::EVALUATION_NAN);
    }
    return true;
}

bool TestEzExpr_Exceptions_EvaluationNaN_Log() {
    ez::Expr ev;
    try {
        ev.parse("log(-5)").eval();  // Logarithm of negative number leads to NaN
        return false;                // Expected an exception
    } catch (const ez::ExprException& e) {
        CTEST_ASSERT(std::string(e.what()) == std::string("Result is NaN"));
        CTEST_ASSERT(e.getCode() == ez::ErrorCode::EVALUATION_NAN);
    }
    return true;
}

bool TestEzExpr_Exceptions_EvaluationNaN_DivZeroZero() {
    ez::Expr ev;
    try {
        ev.parse("0 / 0.0").eval();  // 0 divided by 0.0 leads to NaN
        return false;                // Expected an exception
    } catch (const ez::ExprException& e) {
        CTEST_ASSERT(std::string(e.what()) == std::string("Division by zero"));
        CTEST_ASSERT(e.getCode() == ez::ErrorCode::DIVISION_BY_ZERO);
    }
    return true;
}

// Test for EVALUATION_INF exception
bool TestEzExpr_Exceptions_EvaluationInf() {
    ez::Expr ev;
    try {
        ev.parse("1 / 0.0").eval();  // Division by zero should lead to Inf in floating point
        return false;                // Expected an exception
    } catch (const ez::ExprException& e) {
        CTEST_ASSERT(std::string(e.what()) == std::string("Division by zero"));
        CTEST_ASSERT(e.getCode() == ez::ErrorCode::DIVISION_BY_ZERO);
    }
    return true;
}

bool TestEzExpr_Exceptions_EvaluationInf_Exp() {
    ez::Expr ev;
    try {
        ev.parse("exp(1000)").eval();  // Exponential of a large number may lead to Inf
        return false;                  // Expected an exception
    } catch (const ez::ExprException& e) {
        CTEST_ASSERT(std::string(e.what()) == std::string("Result is Inf"));
        CTEST_ASSERT(e.getCode() == ez::ErrorCode::EVALUATION_INF);
    }
    return true;
}

bool TestEzExpr_Exceptions_EvaluationInf_Power() {
    ez::Expr ev;
    try {
        ev.parse("10^1000").eval();  // Large power leading to Inf
        return false;                // Expected an exception
    } catch (const ez::ExprException& e) {
        CTEST_ASSERT(std::string(e.what()) == std::string("Result is Inf"));
        CTEST_ASSERT(e.getCode() == ez::ErrorCode::EVALUATION_INF);
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////
//// ENTRY POINT ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzExpr_Exceptions(const std::string& vTest) {
    // Errors Codes
    IfTestExist(TestEzExpr_Exceptions_DivisionByZero);
    else IfTestExist(TestEzExpr_Exceptions_DivisionByZero_Float);
    else IfTestExist(TestEzExpr_Exceptions_ModuloByZero);
    else IfTestExist(TestEzExpr_Exceptions_UnmatchedParenthesis_Open);
    else IfTestExist(TestEzExpr_Exceptions_UnmatchedParenthesis_Close);
    else IfTestExist(TestEzExpr_Exceptions_VariableNotFound);
    else IfTestExist(TestEzExpr_Exceptions_VariableNotFound_Multiple);
    else IfTestExist(TestEzExpr_Exceptions_FunctionNotFound);
    else IfTestExist(TestEzExpr_Exceptions_FunctionWrongArgs);
    else IfTestExist(TestEzExpr_Exceptions_EvaluationNaN_1);
    else IfTestExist(TestEzExpr_Exceptions_EvaluationNaN_2);
    else IfTestExist(TestEzExpr_Exceptions_EvaluationNaN_Log);
    else IfTestExist(TestEzExpr_Exceptions_EvaluationNaN_DivZeroZero);
    else IfTestExist(TestEzExpr_Exceptions_EvaluationInf);
    else IfTestExist(TestEzExpr_Exceptions_EvaluationInf_Exp);
    else IfTestExist(TestEzExpr_Exceptions_EvaluationInf_Power);
    // default
    return false;
}
