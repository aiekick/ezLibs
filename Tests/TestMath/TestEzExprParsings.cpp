
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
#include <ezlibs/ezExpr.hpp>
#include <ezlibs/ezLog.hpp>

////////////////////////////////////////////////////////////////////////////
//// PARSING ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzExpr_Parsings_OperatorWithoutOperands() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("+").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::PARSE_ERROR)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_UnaryOperatorWithoutOperand() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("!").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::PARSE_ERROR)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_UnmatchedOpeningParenthesis() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("(5 + 3").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::UNMATCHED_PARENTHESIS)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_UnmatchedClosingParenthesis() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("5 + 3)").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::UNMATCHED_PARENTHESIS)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_DoubleBinaryOperator() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("5 ++ 3").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::OPERATOR_NOT_FOUND)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_DoubleUnaryOperator() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("--5").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::PARSE_ERROR)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_UndefinedVariable() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("x + 5").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::VARIABLE_NOT_FOUND)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_BinaryOperatorWithoutRightOperand() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("5 + ").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::PARSE_ERROR)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_DoubleOperandWithoutOperator() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("5 5").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::PARSE_ERROR)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_FunctionWithoutParentheses() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("sin 5").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::PARSE_ERROR)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_FunctionWithoutArguments() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("sin()").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::FUNCTION_WRONG_ARGUMENTS_COUNT)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_FunctionWithTooManyArguments() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("atan2(1, 2, 3)").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::FUNCTION_WRONG_ARGUMENTS_COUNT)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_FunctionWithOneArgumentInsteadOfTwo() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("atan2(1)").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::FUNCTION_WRONG_ARGUMENTS_COUNT)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_DivisionByZero() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("1 / 0").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::DIVISION_BY_ZERO)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_MisplacedComma() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("1,2 + 3").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::PARSE_ERROR)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_UndefinedOperator() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("5 $ 3").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::OPERATOR_NOT_FOUND)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_MisuseOfFactorialOperator() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("5 ! 3").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::PARSE_ERROR)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_UnaryOperatorBeforeParenthesis() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("!(5)").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::PARSE_ERROR)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_EmptyParenthesis() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("()").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::EMPTY_PARENTHESIS)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_DoubleComma() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("atan2(1,,1)").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::PARSE_ERROR)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_DoubleBinaryOperatorAtStart() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("++1").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::PARSE_ERROR)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_DoubleBinaryOperatorAtEnd() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("1++").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::OPERATOR_NOT_FOUND)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_EmptyExpression() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::PARSE_ERROR)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_UnknownFunctionName() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("unknownFunction(5)").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::FUNCTION_NOT_FOUND)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_MissingParenthesisInComplexExpression() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("5 + 3 * 2 )").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::UNMATCHED_PARENTHESIS)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_OperatorWithoutOperandsInParenthesis() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("( + )").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::PARSE_ERROR)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_NumberFollowedByVariable() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("5x + 3").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::PARSE_ERROR)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_IncorrectUseOfComma() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("5, 3").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::PARSE_ERROR)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_UnrecognizedCharacter() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("5 + @").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::PARSE_ERROR)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_OperatorAtEndOfParenthesis() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("(5 + 3 + )").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::PARSE_ERROR)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_NumberFollowedByParenthesisWithoutOperator() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("5(3 + 2)").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::PARSE_ERROR)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_VariableFollowedByExpressionWithoutOperator() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("x(3 + 2)").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::FUNCTION_NOT_FOUND)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_MisplacedCommaAfterFunction() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("sin(,pi / 2)").set("pi", 3.14159).eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::PARSE_ERROR)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_BinaryOperatorMisplacedInFunction() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("atan2(,1)").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::PARSE_ERROR)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_IncompleteExpression() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("3 +").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::PARSE_ERROR)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_MisuseOfFactorialOperator_2() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("5!3").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::PARSE_ERROR)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_NumberFollowedByBinaryOperator() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("5*").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::PARSE_ERROR)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_IncorrectUseOfOpeningParenthesis() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("((5)").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::UNMATCHED_PARENTHESIS)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_IncorrectVariableName() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("5 + x$").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::OPERATOR_NOT_FOUND)
            return false;
    }
    return true;
}

bool TestEzExpr_Parsings_IncorrectUseOfMultipleOperators() {
    ez::Expr ev;
    ev.setVerbose(true);
    try {
        if (!ev.parse("5 + - * 3").eval().check(0))
            return false;
    } catch (const ez::ExprException& e) {
        LogVarError("Exception : %s", e.what());
        if (e.getCode() != ez::ErrorCode::PARSE_ERROR)
            return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////
//// ENTRY POINT ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzExpr_Parsings(const std::string& vTest) {
    // Parsing
    IfTestExist(TestEzExpr_Parsings_OperatorWithoutOperands);
    else IfTestExist(TestEzExpr_Parsings_UnaryOperatorWithoutOperand);
    else IfTestExist(TestEzExpr_Parsings_UnmatchedOpeningParenthesis);
    else IfTestExist(TestEzExpr_Parsings_UnmatchedClosingParenthesis);
    else IfTestExist(TestEzExpr_Parsings_DoubleBinaryOperator);
    else IfTestExist(TestEzExpr_Parsings_DoubleUnaryOperator);
    else IfTestExist(TestEzExpr_Parsings_UndefinedVariable);
    else IfTestExist(TestEzExpr_Parsings_BinaryOperatorWithoutRightOperand);
    else IfTestExist(TestEzExpr_Parsings_DoubleOperandWithoutOperator);
    else IfTestExist(TestEzExpr_Parsings_FunctionWithoutParentheses);
    else IfTestExist(TestEzExpr_Parsings_FunctionWithoutArguments);
    else IfTestExist(TestEzExpr_Parsings_FunctionWithTooManyArguments);
    else IfTestExist(TestEzExpr_Parsings_FunctionWithOneArgumentInsteadOfTwo);
    else IfTestExist(TestEzExpr_Parsings_DivisionByZero);
    else IfTestExist(TestEzExpr_Parsings_MisplacedComma);
    else IfTestExist(TestEzExpr_Parsings_UndefinedOperator);
    else IfTestExist(TestEzExpr_Parsings_MisuseOfFactorialOperator);
    else IfTestExist(TestEzExpr_Parsings_UnaryOperatorBeforeParenthesis);
    else IfTestExist(TestEzExpr_Parsings_EmptyParenthesis);
    else IfTestExist(TestEzExpr_Parsings_DoubleComma);
    else IfTestExist(TestEzExpr_Parsings_DoubleBinaryOperatorAtStart);
    else IfTestExist(TestEzExpr_Parsings_DoubleBinaryOperatorAtEnd);
    else IfTestExist(TestEzExpr_Parsings_EmptyExpression);
    else IfTestExist(TestEzExpr_Parsings_UnknownFunctionName);
    else IfTestExist(TestEzExpr_Parsings_MissingParenthesisInComplexExpression);
    else IfTestExist(TestEzExpr_Parsings_OperatorWithoutOperandsInParenthesis);
    else IfTestExist(TestEzExpr_Parsings_NumberFollowedByVariable);
    else IfTestExist(TestEzExpr_Parsings_IncorrectUseOfComma);
    else IfTestExist(TestEzExpr_Parsings_UnrecognizedCharacter);
    else IfTestExist(TestEzExpr_Parsings_OperatorAtEndOfParenthesis);
    else IfTestExist(TestEzExpr_Parsings_NumberFollowedByParenthesisWithoutOperator);
    else IfTestExist(TestEzExpr_Parsings_VariableFollowedByExpressionWithoutOperator);
    else IfTestExist(TestEzExpr_Parsings_MisplacedCommaAfterFunction);
    else IfTestExist(TestEzExpr_Parsings_BinaryOperatorMisplacedInFunction);
    else IfTestExist(TestEzExpr_Parsings_IncompleteExpression);
    else IfTestExist(TestEzExpr_Parsings_MisuseOfFactorialOperator_2);
    else IfTestExist(TestEzExpr_Parsings_NumberFollowedByBinaryOperator);
    else IfTestExist(TestEzExpr_Parsings_IncorrectUseOfOpeningParenthesis);
    else IfTestExist(TestEzExpr_Parsings_IncorrectVariableName);
    else IfTestExist(TestEzExpr_Parsings_IncorrectUseOfMultipleOperators);
    // default
    return false;
}
