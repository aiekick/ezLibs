cmake_minimum_required(VERSION 3.10)

set(PROJECT EzLibs_TestMath)
enable_language(C CXX)
project(${PROJECT} CXX)


option(USE_EZ_EXPR_PERFOS_GENERATION "Enable the perfos file generation of EzExpr" OFF)

file(GLOB_RECURSE PROJECT_TEST_SRC_RECURSE 
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/*.h)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX Src FILES ${PROJECT_TEST_SRC_RECURSE})

file(GLOB SRC_RECURSE 
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezExpr.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezMath.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezVec2.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezVec3.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezVec4.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezQuat.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezPlane.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezAABB.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezAABBCCC.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezVariant.hpp)
source_group(TREE ${EZ_LIBS_INCLUDE_DIR}/ezlibs PREFIX Libs FILES ${SRC_RECURSE})

add_executable(${PROJECT} 
	${SRC_RECURSE}
	${PROJECT_TEST_SRC_RECURSE}
)

set_target_properties(${PROJECT} PROPERTIES FOLDER Tests)

target_include_directories(${PROJECT} PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}
)

if(APPLE)
    target_link_options(${PROJECT} PRIVATE
        "-framework" "CoreFoundation"
    )
endif()

##########################################################
## TESTS EzExpr PARSING ##################################
##########################################################

AddTest(TestEzExpr_Parsings_OperatorWithoutOperands)
AddTest(TestEzExpr_Parsings_UnaryOperatorWithoutOperand)
AddTest(TestEzExpr_Parsings_UnmatchedOpeningParenthesis)
AddTest(TestEzExpr_Parsings_UnmatchedClosingParenthesis)
AddTest(TestEzExpr_Parsings_DoubleBinaryOperator)
AddTest(TestEzExpr_Parsings_DoubleUnaryOperator)
AddTest(TestEzExpr_Parsings_UndefinedVariable)
AddTest(TestEzExpr_Parsings_BinaryOperatorWithoutRightOperand)
AddTest(TestEzExpr_Parsings_DoubleOperandWithoutOperator)
AddTest(TestEzExpr_Parsings_FunctionWithoutParentheses)
AddTest(TestEzExpr_Parsings_FunctionWithoutArguments)
AddTest(TestEzExpr_Parsings_FunctionWithTooManyArguments)
AddTest(TestEzExpr_Parsings_FunctionWithOneArgumentInsteadOfTwo)
AddTest(TestEzExpr_Parsings_DivisionByZero)
AddTest(TestEzExpr_Parsings_MisplacedComma)
AddTest(TestEzExpr_Parsings_UndefinedOperator)
AddTest(TestEzExpr_Parsings_MisuseOfFactorialOperator)
AddTest(TestEzExpr_Parsings_UnaryOperatorBeforeParenthesis)
AddTest(TestEzExpr_Parsings_EmptyParenthesis)
AddTest(TestEzExpr_Parsings_DoubleComma)
AddTest(TestEzExpr_Parsings_DoubleBinaryOperatorAtStart)
AddTest(TestEzExpr_Parsings_DoubleBinaryOperatorAtEnd)
AddTest(TestEzExpr_Parsings_EmptyExpression)
AddTest(TestEzExpr_Parsings_UnknownFunctionName)
AddTest(TestEzExpr_Parsings_MissingParenthesisInComplexExpression)
AddTest(TestEzExpr_Parsings_OperatorWithoutOperandsInParenthesis)
AddTest(TestEzExpr_Parsings_NumberFollowedByVariable)
AddTest(TestEzExpr_Parsings_IncorrectUseOfComma)
AddTest(TestEzExpr_Parsings_UnrecognizedCharacter)
AddTest(TestEzExpr_Parsings_OperatorAtEndOfParenthesis)
AddTest(TestEzExpr_Parsings_NumberFollowedByParenthesisWithoutOperator)
AddTest(TestEzExpr_Parsings_VariableFollowedByExpressionWithoutOperator)
AddTest(TestEzExpr_Parsings_MisplacedCommaAfterFunction)
AddTest(TestEzExpr_Parsings_BinaryOperatorMisplacedInFunction)
AddTest(TestEzExpr_Parsings_IncompleteExpression)
AddTest(TestEzExpr_Parsings_MisuseOfFactorialOperator_2)
AddTest(TestEzExpr_Parsings_NumberFollowedByBinaryOperator)
AddTest(TestEzExpr_Parsings_IncorrectUseOfOpeningParenthesis)
AddTest(TestEzExpr_Parsings_IncorrectVariableName)
AddTest(TestEzExpr_Parsings_IncorrectUseOfMultipleOperators)

##########################################################
## TESTS EzExpr CONSTANTS ################################
##########################################################

AddTest(TestEzExpr_Constants_Parsings_PI)
AddTest(TestEzExpr_Constants_Parsings_E)

##########################################################
## TESTS EzExpr BUILTINS #################################
##########################################################

AddTest(TestEzExpr_Builtins_Floor_1)
AddTest(TestEzExpr_Builtins_Floor_2)
AddTest(TestEzExpr_Builtins_Floor_3)
AddTest(TestEzExpr_Builtins_Ceil_1)
AddTest(TestEzExpr_Builtins_Ceil_2)
AddTest(TestEzExpr_Builtins_Ceil_3)
AddTest(TestEzExpr_Builtins_Round_1)
AddTest(TestEzExpr_Builtins_Round_2)
AddTest(TestEzExpr_Builtins_Round_3)
AddTest(TestEzExpr_Builtins_Fract_1)
AddTest(TestEzExpr_Builtins_Fract_2)
AddTest(TestEzExpr_Builtins_Fract_3)
AddTest(TestEzExpr_Builtins_Sign_1)
AddTest(TestEzExpr_Builtins_Sign_2)
AddTest(TestEzExpr_Builtins_Sign_3)
AddTest(TestEzExpr_Builtins_Step_1)
AddTest(TestEzExpr_Builtins_Step_2)
AddTest(TestEzExpr_Builtins_Step_3)
AddTest(TestEzExpr_Builtins_Hypot_1)
AddTest(TestEzExpr_Builtins_Hypot_2)
AddTest(TestEzExpr_Builtins_Hypot_3)
AddTest(TestEzExpr_Builtins_Clamp_1)
AddTest(TestEzExpr_Builtins_Clamp_2)
AddTest(TestEzExpr_Builtins_Clamp_3)
AddTest(TestEzExpr_Builtins_Saturate_1)
AddTest(TestEzExpr_Builtins_Saturate_2)
AddTest(TestEzExpr_Builtins_Saturate_3)
AddTest(TestEzExpr_Builtins_Lerp_1)
AddTest(TestEzExpr_Builtins_Lerp_2)
AddTest(TestEzExpr_Builtins_Lerp_3)
AddTest(TestEzExpr_Builtins_Mix_1)
AddTest(TestEzExpr_Builtins_Mix_2)
AddTest(TestEzExpr_Builtins_Mix_3)
AddTest(TestEzExpr_Builtins_SmoothStep_1)
AddTest(TestEzExpr_Builtins_SmoothStep_2)
AddTest(TestEzExpr_Builtins_SmoothStep_3)
AddTest(TestEzExpr_Builtins_SmoothAbs_1)
AddTest(TestEzExpr_Builtins_SmoothAbs_2)
AddTest(TestEzExpr_Builtins_SmoothAbs_3)
AddTest(TestEzExpr_Builtins_Abs_1)
AddTest(TestEzExpr_Builtins_Abs_2)
AddTest(TestEzExpr_Builtins_Abs_3)
AddTest(TestEzExpr_Builtins_Sin_1)
AddTest(TestEzExpr_Builtins_Sin_2)
AddTest(TestEzExpr_Builtins_Sin_3)
AddTest(TestEzExpr_Builtins_Cos_1)
AddTest(TestEzExpr_Builtins_Cos_2)
AddTest(TestEzExpr_Builtins_Cos_3)
AddTest(TestEzExpr_Builtins_Tan_1)
AddTest(TestEzExpr_Builtins_Tan_2)
AddTest(TestEzExpr_Builtins_Tan_3)
AddTest(TestEzExpr_Builtins_Asin_1)
AddTest(TestEzExpr_Builtins_Asin_2)
AddTest(TestEzExpr_Builtins_Asin_3)
AddTest(TestEzExpr_Builtins_Acos_1)
AddTest(TestEzExpr_Builtins_Acos_2)
AddTest(TestEzExpr_Builtins_Acos_3)
AddTest(TestEzExpr_Builtins_Atan_1)
AddTest(TestEzExpr_Builtins_Atan_2)
AddTest(TestEzExpr_Builtins_Atan_3)
AddTest(TestEzExpr_Builtins_Sinh_1)
AddTest(TestEzExpr_Builtins_Sinh_2)
AddTest(TestEzExpr_Builtins_Sinh_3)
AddTest(TestEzExpr_Builtins_Cosh_1)
AddTest(TestEzExpr_Builtins_Cosh_2)
AddTest(TestEzExpr_Builtins_Cosh_3)
AddTest(TestEzExpr_Builtins_Tanh_1)
AddTest(TestEzExpr_Builtins_Tanh_2)
AddTest(TestEzExpr_Builtins_Tanh_3)
AddTest(TestEzExpr_Builtins_Asinh_1)
AddTest(TestEzExpr_Builtins_Asinh_2)
AddTest(TestEzExpr_Builtins_Asinh_3)
AddTest(TestEzExpr_Builtins_Acosh_1)
AddTest(TestEzExpr_Builtins_Acosh_2)
AddTest(TestEzExpr_Builtins_Acosh_3)
AddTest(TestEzExpr_Builtins_Atanh_1)
AddTest(TestEzExpr_Builtins_Atanh_2)
AddTest(TestEzExpr_Builtins_Atanh_3)
AddTest(TestEzExpr_Builtins_Ln_1)
AddTest(TestEzExpr_Builtins_Ln_2)
AddTest(TestEzExpr_Builtins_Ln_3)
AddTest(TestEzExpr_Builtins_Log_1)
AddTest(TestEzExpr_Builtins_Log_2)
AddTest(TestEzExpr_Builtins_Log_3)
AddTest(TestEzExpr_Builtins_Log1p_1)
AddTest(TestEzExpr_Builtins_Log1p_2)
AddTest(TestEzExpr_Builtins_Log1p_3)
AddTest(TestEzExpr_Builtins_Log2_1)
AddTest(TestEzExpr_Builtins_Log2_2)
AddTest(TestEzExpr_Builtins_Log2_3)
AddTest(TestEzExpr_Builtins_Log10_1)
AddTest(TestEzExpr_Builtins_Log10_2)
AddTest(TestEzExpr_Builtins_Log10_3)
AddTest(TestEzExpr_Builtins_Exp_1)
AddTest(TestEzExpr_Builtins_Exp_2)
AddTest(TestEzExpr_Builtins_Exp_3)
AddTest(TestEzExpr_Builtins_Sqrt_1)
AddTest(TestEzExpr_Builtins_Sqrt_2)
AddTest(TestEzExpr_Builtins_Sqrt_3)
AddTest(TestEzExpr_Builtins_Mod_1)
AddTest(TestEzExpr_Builtins_Mod_2)
AddTest(TestEzExpr_Builtins_Mod_3)
AddTest(TestEzExpr_Builtins_Atan2_1)
AddTest(TestEzExpr_Builtins_Atan2_2)
AddTest(TestEzExpr_Builtins_Atan2_3)
AddTest(TestEzExpr_Builtins_Min_1)
AddTest(TestEzExpr_Builtins_Min_2)
AddTest(TestEzExpr_Builtins_Min_3)
AddTest(TestEzExpr_Builtins_Max_1)
AddTest(TestEzExpr_Builtins_Max_2)
AddTest(TestEzExpr_Builtins_Max_3)
AddTest(TestEzExpr_Builtins_Factorial_1)
AddTest(TestEzExpr_Builtins_Factorial_2)
AddTest(TestEzExpr_Builtins_Factorial_3)
AddTest(TestEzExpr_Builtins_ModuloPercent_1)
AddTest(TestEzExpr_Builtins_ModuloPercent_2)
AddTest(TestEzExpr_Builtins_ModuloPercent_3)
AddTest(TestEzExpr_Builtins_ModuloMod_1)
AddTest(TestEzExpr_Builtins_ModuloMod_2)
AddTest(TestEzExpr_Builtins_ModuloMod_3)

##########################################################
## TESTS EzExpr ERROR CODES ##############################
##########################################################

AddTest(TestEzExpr_Exceptions_DivisionByZero)
AddTest(TestEzExpr_Exceptions_DivisionByZero_Float)
AddTest(TestEzExpr_Exceptions_ModuloByZero)
AddTest(TestEzExpr_Exceptions_UnmatchedParenthesis_Open)
AddTest(TestEzExpr_Exceptions_UnmatchedParenthesis_Close)
AddTest(TestEzExpr_Exceptions_VariableNotFound)
AddTest(TestEzExpr_Exceptions_VariableNotFound_Multiple)
AddTest(TestEzExpr_Exceptions_FunctionNotFound)
AddTest(TestEzExpr_Exceptions_FunctionWrongArgs)
AddTest(TestEzExpr_Exceptions_EvaluationNaN_1)
AddTest(TestEzExpr_Exceptions_EvaluationNaN_2)
AddTest(TestEzExpr_Exceptions_EvaluationNaN_Log)
AddTest(TestEzExpr_Exceptions_EvaluationNaN_DivZeroZero)
AddTest(TestEzExpr_Exceptions_EvaluationInf)
AddTest(TestEzExpr_Exceptions_EvaluationInf_Exp)
AddTest(TestEzExpr_Exceptions_EvaluationInf_Power)

##########################################################
## TESTS EzExpr PERFOS ###################################
##########################################################

if (USE_EZ_EXPR_PERFOS_GENERATION)
	AddTest(TestEzExpr_Perfos_All)
	AddTest(TestEzExpr_Perfos_x_squared)
	AddTest(TestEzExpr_Perfos_x_cubed)
	AddTest(TestEzExpr_Perfos_sin_x)
	AddTest(TestEzExpr_Perfos_sqrt_x)
	AddTest(TestEzExpr_Perfos_cos_x_sin_y)
	AddTest(TestEzExpr_Perfos_a_plus_5)
	AddTest(TestEzExpr_Perfos_5_plus_a_plus_5)
	AddTest(TestEzExpr_Perfos_abs_a_plus_5)
	AddTest(TestEzExpr_Perfos_a_plus_5_times_2)
	AddTest(TestEzExpr_Perfos_a_plus_5_times_2_alt)
	AddTest(TestEzExpr_Perfos_sqrt_pow_a)
	AddTest(TestEzExpr_Perfos_complex_fraction)
endif()

##########################################################
##### TESTS EzMath #######################################
##########################################################

AddTest("TestEzMathRound<float>")
AddTest("TestEzMathRound<double>")

AddTest("TestEzMathFloor<float>")
AddTest("TestEzMathFloor<double>")

AddTest("TestEzMathCeil<float>")
AddTest("TestEzMathCeil<double>")

AddTest("TestEzMathFract<float>")
AddTest("TestEzMathFract<double>")

AddTest("TestEzMathCos<float>")
AddTest("TestEzMathCos<double>")

AddTest("TestEzMathAcos<float>")
AddTest("TestEzMathAcos<double>")

AddTest("TestEzMathSin<float>")
AddTest("TestEzMathSin<double>")

AddTest("TestEzMathAsin<float>")
AddTest("TestEzMathAsin<double>")

AddTest("TestEzMathTan<float>")
AddTest("TestEzMathTan<double>")

AddTest("TestEzMathAtan<float>")
AddTest("TestEzMathAtan<double>")

AddTest("TestEzMathMini<float>")
AddTest("TestEzMathMini<double>")
AddTest("TestEzMathMini<int32_t>")
AddTest("TestEzMathMini<uint32_t>")
AddTest("TestEzMathMini<int64_t>")
AddTest("TestEzMathMini<uint64_t>")

AddTest("TestEzMathMaxi<float>")
AddTest("TestEzMathMaxi<double>")
AddTest("TestEzMathMaxi<int32_t>")
AddTest("TestEzMathMaxi<uint32_t>")
AddTest("TestEzMathMaxi<int64_t>")
AddTest("TestEzMathMaxi<uint64_t>")

AddTest("TestEzMathClamp<float>")
AddTest("TestEzMathClamp<double>")
AddTest("TestEzMathClamp<int32_t>")
AddTest("TestEzMathClamp<uint32_t>")
AddTest("TestEzMathClamp<int64_t>")
AddTest("TestEzMathClamp<uint64_t>")

AddTest("TestEzMathAbs<float>")
AddTest("TestEzMathAbs<double>")
AddTest("TestEzMathAbs<int32_t>")
AddTest("TestEzMathAbs<int64_t>")

AddTest("TestEzMathSign<float>")
AddTest("TestEzMathSign<double>")
AddTest("TestEzMathSign<int32_t>")
AddTest("TestEzMathSign<int64_t>")

AddTest("TestEzMathStep<float>")
AddTest("TestEzMathStep<double>")

AddTest("TestEzMathMod<float>")
AddTest("TestEzMathMod<double>")

AddTest("TestEzMathInvMix<float>")
AddTest("TestEzMathInvMix<double>")

AddTest("TestEzMathLerp<float>")
AddTest("TestEzMathLerp<double>")

AddTest("TestEzMathEerp<float>")
AddTest("TestEzMathEerp<double>")

AddTest("TestEzMathMix<float>")
AddTest("TestEzMathMix<double>")

AddTest("TestEzMathRoundN")

AddTest("TestEzMathFloatIsValid")

AddTest("TestEzMathIsDifferent<float>")
AddTest("TestEzMathIsDifferent<double>")
AddTest("TestEzMathIsDifferent<int32_t>")
AddTest("TestEzMathIsDifferent<uint32_t>")
AddTest("TestEzMathIsDifferent<int64_t>")
AddTest("TestEzMathIsDifferent<uint64_t>")

AddTest("TestEzMathIsEqual<float>")
AddTest("TestEzMathIsEqual<double>")
AddTest("TestEzMathIsEqual<int32_t>")
AddTest("TestEzMathIsEqual<uint32_t>")
AddTest("TestEzMathIsEqual<int64_t>")
AddTest("TestEzMathIsEqual<uint64_t>")

##########################################################
##### TESTS EzVec2 #######################################
##########################################################

AddTest("TestEzVec2Offset<float>")
AddTest("TestEzVec2Offset<double>")
AddTest("TestEzVec2Offset<int32_t>")
AddTest("TestEzVec2Offset<uint32_t>")
AddTest("TestEzVec2Offset<int64_t>")
AddTest("TestEzVec2Offset<uint64_t>")

AddTest("TestEzVec2Negate<float>")
AddTest("TestEzVec2Negate<double>")
AddTest("TestEzVec2Negate<int32_t>")
AddTest("TestEzVec2Negate<int64_t>")

AddTest("TestEzVec2Length<float>")
AddTest("TestEzVec2Length<double>")

AddTest("TestEzVec2Normalize<float>")
AddTest("TestEzVec2Normalize<double>")

AddTest("TestEzVec2Sum<float>")
AddTest("TestEzVec2Sum<double>")
AddTest("TestEzVec2Sum<int32_t>")
AddTest("TestEzVec2Sum<uint32_t>")
AddTest("TestEzVec2Sum<int64_t>")
AddTest("TestEzVec2Sum<uint64_t>")

AddTest("TestEzVec2OperatorAdd<float>")
AddTest("TestEzVec2OperatorAdd<double>")
AddTest("TestEzVec2OperatorAdd<int32_t>")
AddTest("TestEzVec2OperatorAdd<uint32_t>")
AddTest("TestEzVec2OperatorAdd<int64_t>")
AddTest("TestEzVec2OperatorAdd<uint64_t>")

AddTest("TestEzVec2OperatorSubtract<float>")
AddTest("TestEzVec2OperatorSubtract<double>")
AddTest("TestEzVec2OperatorSubtract<int32_t>")
AddTest("TestEzVec2OperatorSubtract<uint32_t>")
AddTest("TestEzVec2OperatorSubtract<int64_t>")
AddTest("TestEzVec2OperatorSubtract<uint64_t>")

AddTest("TestEzVec2OperatorMultiply<float>")
AddTest("TestEzVec2OperatorMultiply<double>")
AddTest("TestEzVec2OperatorMultiply<int32_t>")
AddTest("TestEzVec2OperatorMultiply<uint32_t>")
AddTest("TestEzVec2OperatorMultiply<int64_t>")
AddTest("TestEzVec2OperatorMultiply<uint64_t>")

AddTest("TestEzVec2OperatorDivide<float>")
AddTest("TestEzVec2OperatorDivide<double>")
AddTest("TestEzVec2OperatorDivide<int32_t>")
AddTest("TestEzVec2OperatorDivide<uint32_t>")
AddTest("TestEzVec2OperatorDivide<int64_t>")
AddTest("TestEzVec2OperatorDivide<uint64_t>")

AddTest("TestEzVec2EmptyAND<float>")
AddTest("TestEzVec2EmptyAND<double>")
AddTest("TestEzVec2EmptyAND<int32_t>")
AddTest("TestEzVec2EmptyAND<uint32_t>")
AddTest("TestEzVec2EmptyAND<int64_t>")
AddTest("TestEzVec2EmptyAND<uint64_t>")

AddTest("TestEzVec2EmptyOR<float>")
AddTest("TestEzVec2EmptyOR<double>")
AddTest("TestEzVec2EmptyOR<int32_t>")
AddTest("TestEzVec2EmptyOR<uint32_t>")
AddTest("TestEzVec2EmptyOR<int64_t>")
AddTest("TestEzVec2EmptyOR<uint64_t>")

AddTest("TestEzVec2Min<float>")
AddTest("TestEzVec2Min<double>")
AddTest("TestEzVec2Min<int32_t>")
AddTest("TestEzVec2Min<uint32_t>")
AddTest("TestEzVec2Min<int64_t>")
AddTest("TestEzVec2Min<uint64_t>")

AddTest("TestEzVec2Max<float>")
AddTest("TestEzVec2Max<double>")
AddTest("TestEzVec2Max<int32_t>")
AddTest("TestEzVec2Max<uint32_t>")
AddTest("TestEzVec2Max<int64_t>")
AddTest("TestEzVec2Max<uint64_t>")

AddTest("TestEzVec2GetNormalized<float>")
AddTest("TestEzVec2GetNormalized<double>")

AddTest("TestEzVec2Floor<float>")
AddTest("TestEzVec2Floor<double>")

AddTest("TestEzVec2Fract<float>")
AddTest("TestEzVec2Fract<double>")

AddTest("TestEzVec2Ceil<float>")
AddTest("TestEzVec2Ceil<double>")

AddTest("TestEzVec2Mini<float>")
AddTest("TestEzVec2Mini<double>")
AddTest("TestEzVec2Mini<int32_t>")
AddTest("TestEzVec2Mini<uint32_t>")
AddTest("TestEzVec2Mini<int64_t>")
AddTest("TestEzVec2Mini<uint64_t>")

AddTest("TestEzVec2Maxi<float>")
AddTest("TestEzVec2Maxi<double>")
AddTest("TestEzVec2Maxi<int32_t>")
AddTest("TestEzVec2Maxi<uint32_t>")
AddTest("TestEzVec2Maxi<int64_t>")
AddTest("TestEzVec2Maxi<uint64_t>")

AddTest("TestEzVec2Dot<float>")
AddTest("TestEzVec2Dot<double>")
AddTest("TestEzVec2Dot<int32_t>")
AddTest("TestEzVec2Dot<uint32_t>")
AddTest("TestEzVec2Dot<int64_t>")
AddTest("TestEzVec2Dot<uint64_t>")

AddTest("TestEzVec2Det<float>")
AddTest("TestEzVec2Det<double>")
AddTest("TestEzVec2Det<int32_t>")
AddTest("TestEzVec2Det<uint32_t>")
AddTest("TestEzVec2Det<int64_t>")
AddTest("TestEzVec2Det<uint64_t>")

AddTest("TestEzVec2Reflect<float>")
AddTest("TestEzVec2Reflect<double>")

AddTest("TestEzVec2Sign<float>")
AddTest("TestEzVec2Sign<double>")
AddTest("TestEzVec2Sign<int32_t>")
AddTest("TestEzVec2Sign<int64_t>")

AddTest("TestEzVec2Sin<float>")
AddTest("TestEzVec2Sin<double>")

AddTest("TestEzVec2Cos<float>")
AddTest("TestEzVec2Cos<double>")

AddTest("TestEzVec2Tan<float>")
AddTest("TestEzVec2Tan<double>")

AddTest("TestEzVec2Atan<float>")
AddTest("TestEzVec2Atan<double>")

##########################################################
##### TESTS EzVec3 #######################################
##########################################################

AddTest("TestEzVec3Offset<float>")
AddTest("TestEzVec3Offset<double>")
AddTest("TestEzVec3Offset<int32_t>")
AddTest("TestEzVec3Offset<uint32_t>")
AddTest("TestEzVec3Offset<int64_t>")
AddTest("TestEzVec3Offset<uint64_t>")

AddTest("TestEzVec3Set<float>")
AddTest("TestEzVec3Set<double>")
AddTest("TestEzVec3Set<int32_t>")
AddTest("TestEzVec3Set<uint32_t>")
AddTest("TestEzVec3Set<int64_t>")
AddTest("TestEzVec3Set<uint64_t>")

AddTest("TestEzVec3Negate<float>")
AddTest("TestEzVec3Negate<double>")
AddTest("TestEzVec3Negate<int32_t>")
AddTest("TestEzVec3Negate<int64_t>")

AddTest("TestEzVec3LogicalNot<int32_t>")
AddTest("TestEzVec3LogicalNot<int64_t>")

AddTest("TestEzVec3XY<float>")
AddTest("TestEzVec3XY<double>")
AddTest("TestEzVec3XY<int32_t>")
AddTest("TestEzVec3XY<uint32_t>")
AddTest("TestEzVec3XY<int64_t>")
AddTest("TestEzVec3XY<uint64_t>")

AddTest("TestEzVec3XZ<float>")
AddTest("TestEzVec3XZ<double>")
AddTest("TestEzVec3XZ<int32_t>")
AddTest("TestEzVec3XZ<uint32_t>")
AddTest("TestEzVec3XZ<int64_t>")
AddTest("TestEzVec3XZ<uint64_t>")

AddTest("TestEzVec3YZ<float>")
AddTest("TestEzVec3YZ<double>")
AddTest("TestEzVec3YZ<int32_t>")
AddTest("TestEzVec3YZ<uint32_t>")
AddTest("TestEzVec3YZ<int64_t>")
AddTest("TestEzVec3YZ<uint64_t>")

AddTest("TestEzVec3YZX<float>")
AddTest("TestEzVec3YZX<double>")
AddTest("TestEzVec3YZX<int32_t>")
AddTest("TestEzVec3YZX<uint32_t>")
AddTest("TestEzVec3YZX<int64_t>")
AddTest("TestEzVec3YZX<uint64_t>")

AddTest("TestEzVec3Increment<float>")
AddTest("TestEzVec3Increment<double>")
AddTest("TestEzVec3Increment<int32_t>")
AddTest("TestEzVec3Increment<uint32_t>")
AddTest("TestEzVec3Increment<int64_t>")
AddTest("TestEzVec3Increment<uint64_t>")

AddTest("TestEzVec3Decrement<float>")
AddTest("TestEzVec3Decrement<double>")
AddTest("TestEzVec3Decrement<int32_t>")
AddTest("TestEzVec3Decrement<uint32_t>")
AddTest("TestEzVec3Decrement<int64_t>")
AddTest("TestEzVec3Decrement<uint64_t>")

AddTest("TestEzVec3Length<float>")
AddTest("TestEzVec3Length<double>")

AddTest("TestEzVec3Normalize<float>")
AddTest("TestEzVec3Normalize<double>")

AddTest("TestEzVec3Sum<float>")
AddTest("TestEzVec3Sum<double>")
AddTest("TestEzVec3Sum<int32_t>")
AddTest("TestEzVec3Sum<uint32_t>")
AddTest("TestEzVec3Sum<int64_t>")
AddTest("TestEzVec3Sum<uint64_t>")

AddTest("TestEzVec3SumAbs<float>")
AddTest("TestEzVec3SumAbs<double>")
AddTest("TestEzVec3SumAbs<int32_t>")
AddTest("TestEzVec3SumAbs<int64_t>")

AddTest("TestEzVec3EmptyAND<float>")
AddTest("TestEzVec3EmptyAND<double>")
AddTest("TestEzVec3EmptyAND<int32_t>")
AddTest("TestEzVec3EmptyAND<uint32_t>")
AddTest("TestEzVec3EmptyAND<int64_t>")
AddTest("TestEzVec3EmptyAND<uint64_t>")

AddTest("TestEzVec3EmptyOR<float>")
AddTest("TestEzVec3EmptyOR<double>")
AddTest("TestEzVec3EmptyOR<int32_t>")
AddTest("TestEzVec3EmptyOR<uint32_t>")
AddTest("TestEzVec3EmptyOR<int64_t>")
AddTest("TestEzVec3EmptyOR<uint64_t>")

AddTest("TestEzVec3String<float>")
AddTest("TestEzVec3String<double>")
AddTest("TestEzVec3String<int32_t>")
AddTest("TestEzVec3String<uint32_t>")
AddTest("TestEzVec3String<int64_t>")
AddTest("TestEzVec3String<uint64_t>")

AddTest("TestEzVec3Mini<float>")
AddTest("TestEzVec3Mini<double>")
AddTest("TestEzVec3Mini<int32_t>")
AddTest("TestEzVec3Mini<uint32_t>")
AddTest("TestEzVec3Mini<int64_t>")
AddTest("TestEzVec3Mini<uint64_t>")

AddTest("TestEzVec3Maxi<float>")
AddTest("TestEzVec3Maxi<double>")
AddTest("TestEzVec3Maxi<int32_t>")
AddTest("TestEzVec3Maxi<uint32_t>")
AddTest("TestEzVec3Maxi<int64_t>")
AddTest("TestEzVec3Maxi<uint64_t>")

AddTest("TestEzVec3Equality<float>")
AddTest("TestEzVec3Equality<double>")
AddTest("TestEzVec3Equality<int32_t>")
AddTest("TestEzVec3Equality<uint32_t>")
AddTest("TestEzVec3Equality<int64_t>")
AddTest("TestEzVec3Equality<uint64_t>")

AddTest("TestEzVec3Addition<float>")
AddTest("TestEzVec3Addition<double>")
AddTest("TestEzVec3Addition<int32_t>")
AddTest("TestEzVec3Addition<uint32_t>")
AddTest("TestEzVec3Addition<int64_t>")
AddTest("TestEzVec3Addition<uint64_t>")

AddTest("TestEzVec3Subtraction<float>")
AddTest("TestEzVec3Subtraction<double>")
AddTest("TestEzVec3Subtraction<int32_t>")
AddTest("TestEzVec3Subtraction<uint32_t>")
AddTest("TestEzVec3Subtraction<int64_t>")
AddTest("TestEzVec3Subtraction<uint64_t>")

AddTest("TestEzVec3Multiplication<float>")
AddTest("TestEzVec3Multiplication<double>")
AddTest("TestEzVec3Multiplication<int32_t>")
AddTest("TestEzVec3Multiplication<uint32_t>")
AddTest("TestEzVec3Multiplication<int64_t>")
AddTest("TestEzVec3Multiplication<uint64_t>")

AddTest("TestEzVec3Division<float>")
AddTest("TestEzVec3Division<double>")
AddTest("TestEzVec3Division<int32_t>")
AddTest("TestEzVec3Division<uint32_t>")
AddTest("TestEzVec3Division<int64_t>")
AddTest("TestEzVec3Division<uint64_t>")

##########################################################
##### TESTS EzVec4 #######################################
##########################################################

AddTest("TestEzVec4Offset<float>")
AddTest("TestEzVec4Offset<double>")
AddTest("TestEzVec4Offset<int32_t>")
AddTest("TestEzVec4Offset<uint32_t>")
AddTest("TestEzVec4Offset<int64_t>")
AddTest("TestEzVec4Offset<uint64_t>")

AddTest("TestEzVec4Set<float>")
AddTest("TestEzVec4Set<double>")
AddTest("TestEzVec4Set<int32_t>")
AddTest("TestEzVec4Set<uint32_t>")
AddTest("TestEzVec4Set<int64_t>")
AddTest("TestEzVec4Set<uint64_t>")

AddTest("TestEzVec4Negate<float>")
AddTest("TestEzVec4Negate<double>")

AddTest("TestEzVec4LogicalNot<int32_t>")
AddTest("TestEzVec4LogicalNot<int64_t>")

AddTest("TestEzVec4XY<float>")
AddTest("TestEzVec4XY<double>")
AddTest("TestEzVec4XY<int32_t>")
AddTest("TestEzVec4XY<uint32_t>")
AddTest("TestEzVec4XY<int64_t>")
AddTest("TestEzVec4XY<uint64_t>")

AddTest("TestEzVec4XYZ<float>")
AddTest("TestEzVec4XYZ<double>")
AddTest("TestEzVec4XYZ<int32_t>")
AddTest("TestEzVec4XYZ<uint32_t>")
AddTest("TestEzVec4XYZ<int64_t>")
AddTest("TestEzVec4XYZ<uint64_t>")

AddTest("TestEzVec4ZW<float>")
AddTest("TestEzVec4ZW<double>")
AddTest("TestEzVec4ZW<int32_t>")
AddTest("TestEzVec4ZW<uint32_t>")
AddTest("TestEzVec4ZW<int64_t>")
AddTest("TestEzVec4ZW<uint64_t>")

AddTest("TestEzVec4Increment<float>")
AddTest("TestEzVec4Increment<double>")
AddTest("TestEzVec4Increment<int32_t>")
AddTest("TestEzVec4Increment<uint32_t>")
AddTest("TestEzVec4Increment<int64_t>")
AddTest("TestEzVec4Increment<uint64_t>")

AddTest("TestEzVec4Decrement<float>")
AddTest("TestEzVec4Decrement<double>")
AddTest("TestEzVec4Decrement<int32_t>")
AddTest("TestEzVec4Decrement<uint32_t>")
AddTest("TestEzVec4Decrement<int64_t>")
AddTest("TestEzVec4Decrement<uint64_t>")

AddTest("TestEzVec4Length<float>")
AddTest("TestEzVec4Length<double>")

AddTest("TestEzVec4Normalize<float>")
AddTest("TestEzVec4Normalize<double>")

AddTest("TestEzVec4Sum<float>")
AddTest("TestEzVec4Sum<double>")
AddTest("TestEzVec4Sum<int32_t>")
AddTest("TestEzVec4Sum<uint32_t>")
AddTest("TestEzVec4Sum<int64_t>")
AddTest("TestEzVec4Sum<uint64_t>")

AddTest("TestEzVec4SumAbs<float>")
AddTest("TestEzVec4SumAbs<double>")
AddTest("TestEzVec4SumAbs<int32_t>")
AddTest("TestEzVec4SumAbs<int64_t>")

AddTest("TestEzVec4EmptyAND<float>")
AddTest("TestEzVec4EmptyAND<double>")
AddTest("TestEzVec4EmptyAND<int32_t>")
AddTest("TestEzVec4EmptyAND<uint32_t>")
AddTest("TestEzVec4EmptyAND<int64_t>")
AddTest("TestEzVec4EmptyAND<uint64_t>")

AddTest("TestEzVec4EmptyOR<float>")
AddTest("TestEzVec4EmptyOR<double>")
AddTest("TestEzVec4EmptyOR<int32_t>")
AddTest("TestEzVec4EmptyOR<uint32_t>")
AddTest("TestEzVec4EmptyOR<int64_t>")
AddTest("TestEzVec4EmptyOR<uint64_t>")

AddTest("TestEzVec4String<float>")
AddTest("TestEzVec4String<double>")
AddTest("TestEzVec4String<int32_t>")
AddTest("TestEzVec4String<uint32_t>")
AddTest("TestEzVec4String<int64_t>")
AddTest("TestEzVec4String<uint64_t>")

AddTest("TestEzVec4Mini<float>")
AddTest("TestEzVec4Mini<double>")
AddTest("TestEzVec4Mini<int32_t>")
AddTest("TestEzVec4Mini<uint32_t>")
AddTest("TestEzVec4Mini<int64_t>")
AddTest("TestEzVec4Mini<uint64_t>")

AddTest("TestEzVec4Maxi<float>")
AddTest("TestEzVec4Maxi<double>")
AddTest("TestEzVec4Maxi<int32_t>")
AddTest("TestEzVec4Maxi<uint32_t>")
AddTest("TestEzVec4Maxi<int64_t>")
AddTest("TestEzVec4Maxi<uint64_t>")

AddTest("TestEzVec4Equality<float>")
AddTest("TestEzVec4Equality<double>")
AddTest("TestEzVec4Equality<int32_t>")
AddTest("TestEzVec4Equality<uint32_t>")
AddTest("TestEzVec4Equality<int64_t>")
AddTest("TestEzVec4Equality<uint64_t>")

AddTest("TestEzVec4Addition<float>")
AddTest("TestEzVec4Addition<double>")
AddTest("TestEzVec4Addition<int32_t>")
AddTest("TestEzVec4Addition<uint32_t>")
AddTest("TestEzVec4Addition<int64_t>")
AddTest("TestEzVec4Addition<uint64_t>")

AddTest("TestEzVec4Subtraction<float>")
AddTest("TestEzVec4Subtraction<double>")
AddTest("TestEzVec4Subtraction<int32_t>")
AddTest("TestEzVec4Subtraction<uint32_t>")
AddTest("TestEzVec4Subtraction<int64_t>")
AddTest("TestEzVec4Subtraction<uint64_t>")

AddTest("TestEzVec4Multiplication<float>")
AddTest("TestEzVec4Multiplication<double>")
AddTest("TestEzVec4Multiplication<int32_t>")
AddTest("TestEzVec4Multiplication<uint32_t>")
AddTest("TestEzVec4Multiplication<int64_t>")
AddTest("TestEzVec4Multiplication<uint64_t>")

AddTest("TestEzVec4Division<float>")
AddTest("TestEzVec4Division<double>")
AddTest("TestEzVec4Division<int32_t>")
AddTest("TestEzVec4Division<uint32_t>")
AddTest("TestEzVec4Division<int64_t>")
AddTest("TestEzVec4Division<uint64_t>")
