cmake_minimum_required(VERSION 3.10)

set(PROJECT EzLibs_TestMath)
enable_language(C CXX)
project(${PROJECT} CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_definitions(-DNOMINMAX)
endif()

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
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezMat2.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezQuat.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezPlane.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezAABB.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezAABBCC.hpp
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

StageBuildInc()

##########################################################
## TESTS EzExpr PARSING ##################################
##########################################################

AddTest("TestEzExpr_Parsings_OperatorWithoutOperands")
AddTest("TestEzExpr_Parsings_UnaryOperatorWithoutOperand")
AddTest("TestEzExpr_Parsings_UnmatchedOpeningParenthesis")
AddTest("TestEzExpr_Parsings_UnmatchedClosingParenthesis")
AddTest("TestEzExpr_Parsings_DoubleBinaryOperator")
AddTest("TestEzExpr_Parsings_DoubleUnaryOperator")
AddTest("TestEzExpr_Parsings_UndefinedVariable")
AddTest("TestEzExpr_Parsings_BinaryOperatorWithoutRightOperand")
AddTest("TestEzExpr_Parsings_DoubleOperandWithoutOperator")
AddTest("TestEzExpr_Parsings_FunctionWithoutParentheses")
AddTest("TestEzExpr_Parsings_FunctionWithoutArguments")
AddTest("TestEzExpr_Parsings_FunctionWithTooManyArguments")
AddTest("TestEzExpr_Parsings_FunctionWithOneArgumentInsteadOfTwo")
AddTest("TestEzExpr_Parsings_DivisionByZero")
AddTest("TestEzExpr_Parsings_MisplacedComma")
AddTest("TestEzExpr_Parsings_UndefinedOperator")
AddTest("TestEzExpr_Parsings_MisuseOfFactorialOperator")
AddTest("TestEzExpr_Parsings_UnaryOperatorBeforeParenthesis")
AddTest("TestEzExpr_Parsings_EmptyParenthesis")
AddTest("TestEzExpr_Parsings_DoubleComma")
AddTest("TestEzExpr_Parsings_DoubleBinaryOperatorAtStart")
AddTest("TestEzExpr_Parsings_DoubleBinaryOperatorAtEnd")
AddTest("TestEzExpr_Parsings_EmptyExpression")
AddTest("TestEzExpr_Parsings_UnknownFunctionName")
AddTest("TestEzExpr_Parsings_MissingParenthesisInComplexExpression")
AddTest("TestEzExpr_Parsings_OperatorWithoutOperandsInParenthesis")
AddTest("TestEzExpr_Parsings_NumberFollowedByVariable")
AddTest("TestEzExpr_Parsings_IncorrectUseOfComma")
AddTest("TestEzExpr_Parsings_UnrecognizedCharacter")
AddTest("TestEzExpr_Parsings_OperatorAtEndOfParenthesis")
AddTest("TestEzExpr_Parsings_NumberFollowedByParenthesisWithoutOperator")
AddTest("TestEzExpr_Parsings_VariableFollowedByExpressionWithoutOperator")
AddTest("TestEzExpr_Parsings_MisplacedCommaAfterFunction")
AddTest("TestEzExpr_Parsings_BinaryOperatorMisplacedInFunction")
AddTest("TestEzExpr_Parsings_IncompleteExpression")
AddTest("TestEzExpr_Parsings_MisuseOfFactorialOperator_2")
AddTest("TestEzExpr_Parsings_NumberFollowedByBinaryOperator")
AddTest("TestEzExpr_Parsings_IncorrectUseOfOpeningParenthesis")
AddTest("TestEzExpr_Parsings_IncorrectVariableName")
AddTest("TestEzExpr_Parsings_IncorrectUseOfMultipleOperators")

##########################################################
## TESTS EzExpr CONSTANTS ################################
##########################################################

AddTest("TestEzExpr_Constants_Parsings_PI")
AddTest("TestEzExpr_Constants_Parsings_E")

##########################################################
## TESTS EzExpr BUILTINS #################################
##########################################################

AddTest("TestEzExpr_Builtins_Floor_1")
AddTest("TestEzExpr_Builtins_Floor_2")
AddTest("TestEzExpr_Builtins_Floor_3")
AddTest("TestEzExpr_Builtins_Ceil_1")
AddTest("TestEzExpr_Builtins_Ceil_2")
AddTest("TestEzExpr_Builtins_Ceil_3")
AddTest("TestEzExpr_Builtins_Round_1")
AddTest("TestEzExpr_Builtins_Round_2")
AddTest("TestEzExpr_Builtins_Round_3")
AddTest("TestEzExpr_Builtins_Fract_1")
AddTest("TestEzExpr_Builtins_Fract_2")
AddTest("TestEzExpr_Builtins_Fract_3")
AddTest("TestEzExpr_Builtins_Sign_1")
AddTest("TestEzExpr_Builtins_Sign_2")
AddTest("TestEzExpr_Builtins_Sign_3")
AddTest("TestEzExpr_Builtins_Step_1")
AddTest("TestEzExpr_Builtins_Step_2")
AddTest("TestEzExpr_Builtins_Step_3")
AddTest("TestEzExpr_Builtins_Hypot_1")
AddTest("TestEzExpr_Builtins_Hypot_2")
AddTest("TestEzExpr_Builtins_Hypot_3")
AddTest("TestEzExpr_Builtins_Clamp_1")
AddTest("TestEzExpr_Builtins_Clamp_2")
AddTest("TestEzExpr_Builtins_Clamp_3")
AddTest("TestEzExpr_Builtins_Saturate_1")
AddTest("TestEzExpr_Builtins_Saturate_2")
AddTest("TestEzExpr_Builtins_Saturate_3")
AddTest("TestEzExpr_Builtins_Lerp_1")
AddTest("TestEzExpr_Builtins_Lerp_2")
AddTest("TestEzExpr_Builtins_Lerp_3")
AddTest("TestEzExpr_Builtins_Mix_1")
AddTest("TestEzExpr_Builtins_Mix_2")
AddTest("TestEzExpr_Builtins_Mix_3")
AddTest("TestEzExpr_Builtins_SmoothStep_1")
AddTest("TestEzExpr_Builtins_SmoothStep_2")
AddTest("TestEzExpr_Builtins_SmoothStep_3")
AddTest("TestEzExpr_Builtins_SmoothAbs_1")
AddTest("TestEzExpr_Builtins_SmoothAbs_2")
AddTest("TestEzExpr_Builtins_SmoothAbs_3")
AddTest("TestEzExpr_Builtins_Abs_1")
AddTest("TestEzExpr_Builtins_Abs_2")
AddTest("TestEzExpr_Builtins_Abs_3")
AddTest("TestEzExpr_Builtins_Sin_1")
AddTest("TestEzExpr_Builtins_Sin_2")
AddTest("TestEzExpr_Builtins_Sin_3")
AddTest("TestEzExpr_Builtins_Cos_1")
AddTest("TestEzExpr_Builtins_Cos_2")
AddTest("TestEzExpr_Builtins_Cos_3")
AddTest("TestEzExpr_Builtins_Tan_1")
AddTest("TestEzExpr_Builtins_Tan_2")
AddTest("TestEzExpr_Builtins_Tan_3")
AddTest("TestEzExpr_Builtins_Asin_1")
AddTest("TestEzExpr_Builtins_Asin_2")
AddTest("TestEzExpr_Builtins_Asin_3")
AddTest("TestEzExpr_Builtins_Acos_1")
AddTest("TestEzExpr_Builtins_Acos_2")
AddTest("TestEzExpr_Builtins_Acos_3")
AddTest("TestEzExpr_Builtins_Atan_1")
AddTest("TestEzExpr_Builtins_Atan_2")
AddTest("TestEzExpr_Builtins_Atan_3")
AddTest("TestEzExpr_Builtins_Sinh_1")
AddTest("TestEzExpr_Builtins_Sinh_2")
AddTest("TestEzExpr_Builtins_Sinh_3")
AddTest("TestEzExpr_Builtins_Cosh_1")
AddTest("TestEzExpr_Builtins_Cosh_2")
AddTest("TestEzExpr_Builtins_Cosh_3")
AddTest("TestEzExpr_Builtins_Tanh_1")
AddTest("TestEzExpr_Builtins_Tanh_2")
AddTest("TestEzExpr_Builtins_Tanh_3")
AddTest("TestEzExpr_Builtins_Asinh_1")
AddTest("TestEzExpr_Builtins_Asinh_2")
AddTest("TestEzExpr_Builtins_Asinh_3")
AddTest("TestEzExpr_Builtins_Acosh_1")
AddTest("TestEzExpr_Builtins_Acosh_2")
AddTest("TestEzExpr_Builtins_Acosh_3")
AddTest("TestEzExpr_Builtins_Atanh_1")
AddTest("TestEzExpr_Builtins_Atanh_2")
AddTest("TestEzExpr_Builtins_Atanh_3")
AddTest("TestEzExpr_Builtins_Ln_1")
AddTest("TestEzExpr_Builtins_Ln_2")
AddTest("TestEzExpr_Builtins_Ln_3")
AddTest("TestEzExpr_Builtins_Log_1")
AddTest("TestEzExpr_Builtins_Log_2")
AddTest("TestEzExpr_Builtins_Log_3")
AddTest("TestEzExpr_Builtins_Log1p_1")
AddTest("TestEzExpr_Builtins_Log1p_2")
AddTest("TestEzExpr_Builtins_Log1p_3")
AddTest("TestEzExpr_Builtins_Log2_1")
AddTest("TestEzExpr_Builtins_Log2_2")
AddTest("TestEzExpr_Builtins_Log2_3")
AddTest("TestEzExpr_Builtins_Log10_1")
AddTest("TestEzExpr_Builtins_Log10_2")
AddTest("TestEzExpr_Builtins_Log10_3")
AddTest("TestEzExpr_Builtins_Exp_1")
AddTest("TestEzExpr_Builtins_Exp_2")
AddTest("TestEzExpr_Builtins_Exp_3")
AddTest("TestEzExpr_Builtins_Sqrt_1")
AddTest("TestEzExpr_Builtins_Sqrt_2")
AddTest("TestEzExpr_Builtins_Sqrt_3")
AddTest("TestEzExpr_Builtins_Mod_1")
AddTest("TestEzExpr_Builtins_Mod_2")
AddTest("TestEzExpr_Builtins_Mod_3")
AddTest("TestEzExpr_Builtins_Atan2_1")
AddTest("TestEzExpr_Builtins_Atan2_2")
AddTest("TestEzExpr_Builtins_Atan2_3")
AddTest("TestEzExpr_Builtins_Min_1")
AddTest("TestEzExpr_Builtins_Min_2")
AddTest("TestEzExpr_Builtins_Min_3")
AddTest("TestEzExpr_Builtins_Max_1")
AddTest("TestEzExpr_Builtins_Max_2")
AddTest("TestEzExpr_Builtins_Max_3")
AddTest("TestEzExpr_Builtins_Factorial_1")
AddTest("TestEzExpr_Builtins_Factorial_2")
AddTest("TestEzExpr_Builtins_Factorial_3")
AddTest("TestEzExpr_Builtins_ModuloPercent_1")
AddTest("TestEzExpr_Builtins_ModuloPercent_2")
AddTest("TestEzExpr_Builtins_ModuloPercent_3")
AddTest("TestEzExpr_Builtins_ModuloMod_1")
AddTest("TestEzExpr_Builtins_ModuloMod_2")
AddTest("TestEzExpr_Builtins_ModuloMod_3")

##########################################################
## TESTS EzExpr ERROR CODES ##############################
##########################################################

AddTest("TestEzExpr_Exceptions_DivisionByZero")
AddTest("TestEzExpr_Exceptions_DivisionByZero_Float")
AddTest("TestEzExpr_Exceptions_ModuloByZero")
AddTest("TestEzExpr_Exceptions_UnmatchedParenthesis_Open")
AddTest("TestEzExpr_Exceptions_UnmatchedParenthesis_Close")
AddTest("TestEzExpr_Exceptions_VariableNotFound")
AddTest("TestEzExpr_Exceptions_VariableNotFound_Multiple")
AddTest("TestEzExpr_Exceptions_FunctionNotFound")
AddTest("TestEzExpr_Exceptions_FunctionWrongArgs")
AddTest("TestEzExpr_Exceptions_EvaluationNaN_1")
AddTest("TestEzExpr_Exceptions_EvaluationNaN_2")
AddTest("TestEzExpr_Exceptions_EvaluationNaN_Log")
AddTest("TestEzExpr_Exceptions_EvaluationNaN_DivZeroZero")
AddTest("TestEzExpr_Exceptions_EvaluationInf")
AddTest("TestEzExpr_Exceptions_EvaluationInf_Exp")
AddTest("TestEzExpr_Exceptions_EvaluationInf_Power")

##########################################################
## TESTS EzExpr PERFOS ###################################
##########################################################

if (USE_EZ_EXPR_PERFOS_GENERATION)
	AddTest("TestEzExpr_Perfos_All")
	AddTest("TestEzExpr_Perfos_x_squared")
	AddTest("TestEzExpr_Perfos_x_cubed")
	AddTest("TestEzExpr_Perfos_sin_x")
	AddTest("TestEzExpr_Perfos_sqrt_x")
	AddTest("TestEzExpr_Perfos_cos_x_sin_y")
	AddTest("TestEzExpr_Perfos_a_plus_5")
	AddTest("TestEzExpr_Perfos_5_plus_a_plus_5")
	AddTest("TestEzExpr_Perfos_abs_a_plus_5")
	AddTest("TestEzExpr_Perfos_a_plus_5_times_2")
	AddTest("TestEzExpr_Perfos_a_plus_5_times_2_alt")
	AddTest("TestEzExpr_Perfos_sqrt_pow_a")
	AddTest("TestEzExpr_Perfos_complex_fraction")
endif()

##########################################################
##### TESTS EzMath #######################################
##########################################################

AddTest("TestEzMath_Round<float>")
AddTest("TestEzMath_Round<double>")

AddTest("TestEzMath_Floor<float>")
AddTest("TestEzMath_Floor<double>")

AddTest("TestEzMath_Ceil<float>")
AddTest("TestEzMath_Ceil<double>")

AddTest("TestEzMath_Fract<float>")
AddTest("TestEzMath_Fract<double>")

AddTest("TestEzMath_Cos<float>")
AddTest("TestEzMath_Cos<double>")

AddTest("TestEzMath_Acos<float>")
AddTest("TestEzMath_Acos<double>")

AddTest("TestEzMath_Sin<float>")
AddTest("TestEzMath_Sin<double>")

AddTest("TestEzMath_Asin<float>")
AddTest("TestEzMath_Asin<double>")

AddTest("TestEzMath_Tan<float>")
AddTest("TestEzMath_Tan<double>")

AddTest("TestEzMath_Atan<float>")
AddTest("TestEzMath_Atan<double>")

AddTest("TestEzMath_Mini<float>")
AddTest("TestEzMath_Mini<double>")
AddTest("TestEzMath_Mini<int32_t>")
AddTest("TestEzMath_Mini<uint32_t>")
AddTest("TestEzMath_Mini<int64_t>")
AddTest("TestEzMath_Mini<uint64_t>")

AddTest("TestEzMath_Maxi<float>")
AddTest("TestEzMath_Maxi<double>")
AddTest("TestEzMath_Maxi<int32_t>")
AddTest("TestEzMath_Maxi<uint32_t>")
AddTest("TestEzMath_Maxi<int64_t>")
AddTest("TestEzMath_Maxi<uint64_t>")

AddTest("TestEzMath_Clamp<float>")
AddTest("TestEzMath_Clamp<double>")
AddTest("TestEzMath_Clamp<int32_t>")
AddTest("TestEzMath_Clamp<uint32_t>")
AddTest("TestEzMath_Clamp<int64_t>")
AddTest("TestEzMath_Clamp<uint64_t>")

AddTest("TestEzMath_Abs<float>")
AddTest("TestEzMath_Abs<double>")
AddTest("TestEzMath_Abs<int32_t>")
AddTest("TestEzMath_Abs<int64_t>")

AddTest("TestEzMath_Sign<float>")
AddTest("TestEzMath_Sign<double>")
AddTest("TestEzMath_Sign<int32_t>")
AddTest("TestEzMath_Sign<int64_t>")

AddTest("TestEzMath_Step<float>")
AddTest("TestEzMath_Step<double>")

AddTest("TestEzMath_Mod<float>")
AddTest("TestEzMath_Mod<double>")

AddTest("TestEzMath_InvMix<float>")
AddTest("TestEzMath_InvMix<double>")

AddTest("TestEzMath_Lerp<float>")
AddTest("TestEzMath_Lerp<double>")

AddTest("TestEzMath_Eerp<float>")
AddTest("TestEzMath_Eerp<double>")

AddTest("TestEzMath_Mix<float>")
AddTest("TestEzMath_Mix<double>")

AddTest("TestEzMath_RoundN")

AddTest("TestEzMath_FloatIsValid")

AddTest("TestEzMath_IsDifferent<float>")
AddTest("TestEzMath_IsDifferent<double>")
AddTest("TestEzMath_IsDifferent<int32_t>")
AddTest("TestEzMath_IsDifferent<uint32_t>")
AddTest("TestEzMath_IsDifferent<int64_t>")
AddTest("TestEzMath_IsDifferent<uint64_t>")

AddTest("TestEzMath_IsEqual<float>")
AddTest("TestEzMath_IsEqual<double>")
AddTest("TestEzMath_IsEqual<int32_t>")
AddTest("TestEzMath_IsEqual<uint32_t>")
AddTest("TestEzMath_IsEqual<int64_t>")
AddTest("TestEzMath_IsEqual<uint64_t>")

##########################################################
##### TESTS EzAABBCC #####################################
##########################################################

AddTest("TestEzAABBCC_Constructor")
AddTest("TestEzAABBCC_GetCenter")
AddTest("TestEzAABBCC_GetExtents")
AddTest("TestEzAABBCC_GetPerimeter")
AddTest("TestEzAABBCC_Size")
AddTest("TestEzAABBCC_CombineAABB")
AddTest("TestEzAABBCC_CombineTwoAABBs")
AddTest("TestEzAABBCC_CombinePoint")
AddTest("TestEzAABBCC_Contains")
AddTest("TestEzAABBCC_ContainsPoint")
AddTest("TestEzAABBCC_Operators")

##########################################################
##### TESTS EzVec2 #######################################
##########################################################

AddTest("TestEzVec2_Offset<float>")
AddTest("TestEzVec2_Offset<double>")
AddTest("TestEzVec2_Offset<int32_t>")
AddTest("TestEzVec2_Offset<uint32_t>")
AddTest("TestEzVec2_Offset<int64_t>")
AddTest("TestEzVec2_Offset<uint64_t>")

AddTest("TestEzVec2_Negate<float>")
AddTest("TestEzVec2_Negate<double>")
AddTest("TestEzVec2_Negate<int32_t>")
AddTest("TestEzVec2_Negate<int64_t>")

AddTest("TestEzVec2_Length<float>")
AddTest("TestEzVec2_Length<double>")

AddTest("TestEzVec2_Normalize<float>")
AddTest("TestEzVec2_Normalize<double>")

AddTest("TestEzVec2_Sum<float>")
AddTest("TestEzVec2_Sum<double>")
AddTest("TestEzVec2_Sum<int32_t>")
AddTest("TestEzVec2_Sum<uint32_t>")
AddTest("TestEzVec2_Sum<int64_t>")
AddTest("TestEzVec2_Sum<uint64_t>")

AddTest("TestEzVec2_OperatorAdd<float>")
AddTest("TestEzVec2_OperatorAdd<double>")
AddTest("TestEzVec2_OperatorAdd<int32_t>")
AddTest("TestEzVec2_OperatorAdd<uint32_t>")
AddTest("TestEzVec2_OperatorAdd<int64_t>")
AddTest("TestEzVec2_OperatorAdd<uint64_t>")

AddTest("TestEzVec2_OperatorSubtract<float>")
AddTest("TestEzVec2_OperatorSubtract<double>")
AddTest("TestEzVec2_OperatorSubtract<int32_t>")
AddTest("TestEzVec2_OperatorSubtract<uint32_t>")
AddTest("TestEzVec2_OperatorSubtract<int64_t>")
AddTest("TestEzVec2_OperatorSubtract<uint64_t>")

AddTest("TestEzVec2_OperatorMultiply<float>")
AddTest("TestEzVec2_OperatorMultiply<double>")
AddTest("TestEzVec2_OperatorMultiply<int32_t>")
AddTest("TestEzVec2_OperatorMultiply<uint32_t>")
AddTest("TestEzVec2_OperatorMultiply<int64_t>")
AddTest("TestEzVec2_OperatorMultiply<uint64_t>")

AddTest("TestEzVec2_OperatorDivide<float>")
AddTest("TestEzVec2_OperatorDivide<double>")
AddTest("TestEzVec2_OperatorDivide<int32_t>")
AddTest("TestEzVec2_OperatorDivide<uint32_t>")
AddTest("TestEzVec2_OperatorDivide<int64_t>")
AddTest("TestEzVec2_OperatorDivide<uint64_t>")

AddTest("TestEzVec2_EmptyAND<float>")
AddTest("TestEzVec2_EmptyAND<double>")
AddTest("TestEzVec2_EmptyAND<int32_t>")
AddTest("TestEzVec2_EmptyAND<uint32_t>")
AddTest("TestEzVec2_EmptyAND<int64_t>")
AddTest("TestEzVec2_EmptyAND<uint64_t>")

AddTest("TestEzVec2_EmptyOR<float>")
AddTest("TestEzVec2_EmptyOR<double>")
AddTest("TestEzVec2_EmptyOR<int32_t>")
AddTest("TestEzVec2_EmptyOR<uint32_t>")
AddTest("TestEzVec2_EmptyOR<int64_t>")
AddTest("TestEzVec2_EmptyOR<uint64_t>")

AddTest("TestEzVec2_Min<float>")
AddTest("TestEzVec2_Min<double>")
AddTest("TestEzVec2_Min<int32_t>")
AddTest("TestEzVec2_Min<uint32_t>")
AddTest("TestEzVec2_Min<int64_t>")
AddTest("TestEzVec2_Min<uint64_t>")

AddTest("TestEzVec2_Max<float>")
AddTest("TestEzVec2_Max<double>")
AddTest("TestEzVec2_Max<int32_t>")
AddTest("TestEzVec2_Max<uint32_t>")
AddTest("TestEzVec2_Max<int64_t>")
AddTest("TestEzVec2_Max<uint64_t>")

AddTest("TestEzVec2_GetNormalized<float>")
AddTest("TestEzVec2_GetNormalized<double>")

AddTest("TestEzVec2_Floor<float>")
AddTest("TestEzVec2_Floor<double>")

AddTest("TestEzVec2_Fract<float>")
AddTest("TestEzVec2_Fract<double>")

AddTest("TestEzVec2_Ceil<float>")
AddTest("TestEzVec2_Ceil<double>")

AddTest("TestEzVec2_Mini<float>")
AddTest("TestEzVec2_Mini<double>")
AddTest("TestEzVec2_Mini<int32_t>")
AddTest("TestEzVec2_Mini<uint32_t>")
AddTest("TestEzVec2_Mini<int64_t>")
AddTest("TestEzVec2_Mini<uint64_t>")

AddTest("TestEzVec2_Maxi<float>")
AddTest("TestEzVec2_Maxi<double>")
AddTest("TestEzVec2_Maxi<int32_t>")
AddTest("TestEzVec2_Maxi<uint32_t>")
AddTest("TestEzVec2_Maxi<int64_t>")
AddTest("TestEzVec2_Maxi<uint64_t>")

AddTest("TestEzVec2_Dot<float>")
AddTest("TestEzVec2_Dot<double>")
AddTest("TestEzVec2_Dot<int32_t>")
AddTest("TestEzVec2_Dot<uint32_t>")
AddTest("TestEzVec2_Dot<int64_t>")
AddTest("TestEzVec2_Dot<uint64_t>")

AddTest("TestEzVec2_Det<float>")
AddTest("TestEzVec2_Det<double>")
AddTest("TestEzVec2_Det<int32_t>")
AddTest("TestEzVec2_Det<uint32_t>")
AddTest("TestEzVec2_Det<int64_t>")
AddTest("TestEzVec2_Det<uint64_t>")

AddTest("TestEzVec2_Reflect<float>")
AddTest("TestEzVec2_Reflect<double>")

AddTest("TestEzVec2_Sign<float>")
AddTest("TestEzVec2_Sign<double>")
AddTest("TestEzVec2_Sign<int32_t>")
AddTest("TestEzVec2_Sign<int64_t>")

AddTest("TestEzVec2_Sin<float>")
AddTest("TestEzVec2_Sin<double>")

AddTest("TestEzVec2_Cos<float>")
AddTest("TestEzVec2_Cos<double>")

AddTest("TestEzVec2_Tan<float>")
AddTest("TestEzVec2_Tan<double>")

AddTest("TestEzVec2_Atan<float>")
AddTest("TestEzVec2_Atan<double>")

##########################################################
##### TESTS EzVec3 #######################################
##########################################################

AddTest("TestEzVec3_Offset<float>")
AddTest("TestEzVec3_Offset<double>")
AddTest("TestEzVec3_Offset<int32_t>")
AddTest("TestEzVec3_Offset<uint32_t>")
AddTest("TestEzVec3_Offset<int64_t>")
AddTest("TestEzVec3_Offset<uint64_t>")

AddTest("TestEzVec3_Set<float>")
AddTest("TestEzVec3_Set<double>")
AddTest("TestEzVec3_Set<int32_t>")
AddTest("TestEzVec3_Set<uint32_t>")
AddTest("TestEzVec3_Set<int64_t>")
AddTest("TestEzVec3_Set<uint64_t>")

AddTest("TestEzVec3_Negate<float>")
AddTest("TestEzVec3_Negate<double>")
AddTest("TestEzVec3_Negate<int32_t>")
AddTest("TestEzVec3_Negate<int64_t>")

AddTest("TestEzVec3_LogicalNot<int32_t>")
AddTest("TestEzVec3_LogicalNot<int64_t>")

AddTest("TestEzVec3_XY<float>")
AddTest("TestEzVec3_XY<double>")
AddTest("TestEzVec3_XY<int32_t>")
AddTest("TestEzVec3_XY<uint32_t>")
AddTest("TestEzVec3_XY<int64_t>")
AddTest("TestEzVec3_XY<uint64_t>")

AddTest("TestEzVec3_XZ<float>")
AddTest("TestEzVec3_XZ<double>")
AddTest("TestEzVec3_XZ<int32_t>")
AddTest("TestEzVec3_XZ<uint32_t>")
AddTest("TestEzVec3_XZ<int64_t>")
AddTest("TestEzVec3_XZ<uint64_t>")

AddTest("TestEzVec3_YZ<float>")
AddTest("TestEzVec3_YZ<double>")
AddTest("TestEzVec3_YZ<int32_t>")
AddTest("TestEzVec3_YZ<uint32_t>")
AddTest("TestEzVec3_YZ<int64_t>")
AddTest("TestEzVec3_YZ<uint64_t>")

AddTest("TestEzVec3_YZX<float>")
AddTest("TestEzVec3_YZX<double>")
AddTest("TestEzVec3_YZX<int32_t>")
AddTest("TestEzVec3_YZX<uint32_t>")
AddTest("TestEzVec3_YZX<int64_t>")
AddTest("TestEzVec3_YZX<uint64_t>")

AddTest("TestEzVec3_Increment<float>")
AddTest("TestEzVec3_Increment<double>")
AddTest("TestEzVec3_Increment<int32_t>")
AddTest("TestEzVec3_Increment<uint32_t>")
AddTest("TestEzVec3_Increment<int64_t>")
AddTest("TestEzVec3_Increment<uint64_t>")

AddTest("TestEzVec3_Decrement<float>")
AddTest("TestEzVec3_Decrement<double>")
AddTest("TestEzVec3_Decrement<int32_t>")
AddTest("TestEzVec3_Decrement<uint32_t>")
AddTest("TestEzVec3_Decrement<int64_t>")
AddTest("TestEzVec3_Decrement<uint64_t>")

AddTest("TestEzVec3_Length<float>")
AddTest("TestEzVec3_Length<double>")

AddTest("TestEzVec3_Normalize<float>")
AddTest("TestEzVec3_Normalize<double>")

AddTest("TestEzVec3_Sum<float>")
AddTest("TestEzVec3_Sum<double>")
AddTest("TestEzVec3_Sum<int32_t>")
AddTest("TestEzVec3_Sum<uint32_t>")
AddTest("TestEzVec3_Sum<int64_t>")
AddTest("TestEzVec3_Sum<uint64_t>")

AddTest("TestEzVec3_SumAbs<float>")
AddTest("TestEzVec3_SumAbs<double>")
AddTest("TestEzVec3_SumAbs<int32_t>")
AddTest("TestEzVec3_SumAbs<int64_t>")

AddTest("TestEzVec3_EmptyAND<float>")
AddTest("TestEzVec3_EmptyAND<double>")
AddTest("TestEzVec3_EmptyAND<int32_t>")
AddTest("TestEzVec3_EmptyAND<uint32_t>")
AddTest("TestEzVec3_EmptyAND<int64_t>")
AddTest("TestEzVec3_EmptyAND<uint64_t>")

AddTest("TestEzVec3_EmptyOR<float>")
AddTest("TestEzVec3_EmptyOR<double>")
AddTest("TestEzVec3_EmptyOR<int32_t>")
AddTest("TestEzVec3_EmptyOR<uint32_t>")
AddTest("TestEzVec3_EmptyOR<int64_t>")
AddTest("TestEzVec3_EmptyOR<uint64_t>")

AddTest("TestEzVec3_String<float>")
AddTest("TestEzVec3_String<double>")
AddTest("TestEzVec3_String<int32_t>")
AddTest("TestEzVec3_String<uint32_t>")
AddTest("TestEzVec3_String<int64_t>")
AddTest("TestEzVec3_String<uint64_t>")

AddTest("TestEzVec3_Mini<float>")
AddTest("TestEzVec3_Mini<double>")
AddTest("TestEzVec3_Mini<int32_t>")
AddTest("TestEzVec3_Mini<uint32_t>")
AddTest("TestEzVec3_Mini<int64_t>")
AddTest("TestEzVec3_Mini<uint64_t>")

AddTest("TestEzVec3_Maxi<float>")
AddTest("TestEzVec3_Maxi<double>")
AddTest("TestEzVec3_Maxi<int32_t>")
AddTest("TestEzVec3_Maxi<uint32_t>")
AddTest("TestEzVec3_Maxi<int64_t>")
AddTest("TestEzVec3_Maxi<uint64_t>")

AddTest("TestEzVec3_Equality<float>")
AddTest("TestEzVec3_Equality<double>")
AddTest("TestEzVec3_Equality<int32_t>")
AddTest("TestEzVec3_Equality<uint32_t>")
AddTest("TestEzVec3_Equality<int64_t>")
AddTest("TestEzVec3_Equality<uint64_t>")

AddTest("TestEzVec3_Addition<float>")
AddTest("TestEzVec3_Addition<double>")
AddTest("TestEzVec3_Addition<int32_t>")
AddTest("TestEzVec3_Addition<uint32_t>")
AddTest("TestEzVec3_Addition<int64_t>")
AddTest("TestEzVec3_Addition<uint64_t>")

AddTest("TestEzVec3_Subtraction<float>")
AddTest("TestEzVec3_Subtraction<double>")
AddTest("TestEzVec3_Subtraction<int32_t>")
AddTest("TestEzVec3_Subtraction<uint32_t>")
AddTest("TestEzVec3_Subtraction<int64_t>")
AddTest("TestEzVec3_Subtraction<uint64_t>")

AddTest("TestEzVec3_Multiplication<float>")
AddTest("TestEzVec3_Multiplication<double>")
AddTest("TestEzVec3_Multiplication<int32_t>")
AddTest("TestEzVec3_Multiplication<uint32_t>")
AddTest("TestEzVec3_Multiplication<int64_t>")
AddTest("TestEzVec3_Multiplication<uint64_t>")

AddTest("TestEzVec3_Division<float>")
AddTest("TestEzVec3_Division<double>")
AddTest("TestEzVec3_Division<int32_t>")
AddTest("TestEzVec3_Division<uint32_t>")
AddTest("TestEzVec3_Division<int64_t>")
AddTest("TestEzVec3_Division<uint64_t>")

##########################################################
##### TESTS EzVec4 #######################################
##########################################################

AddTest("TestEzVec4_Offset<float>")
AddTest("TestEzVec4_Offset<double>")
AddTest("TestEzVec4_Offset<int32_t>")
AddTest("TestEzVec4_Offset<uint32_t>")
AddTest("TestEzVec4_Offset<int64_t>")
AddTest("TestEzVec4_Offset<uint64_t>")

AddTest("TestEzVec4_Set<float>")
AddTest("TestEzVec4_Set<double>")
AddTest("TestEzVec4_Set<int32_t>")
AddTest("TestEzVec4_Set<uint32_t>")
AddTest("TestEzVec4_Set<int64_t>")
AddTest("TestEzVec4_Set<uint64_t>")

AddTest("TestEzVec4_Negate<float>")
AddTest("TestEzVec4_Negate<double>")

AddTest("TestEzVec4_LogicalNot<int32_t>")
AddTest("TestEzVec4_LogicalNot<int64_t>")

AddTest("TestEzVec4_XY<float>")
AddTest("TestEzVec4_XY<double>")
AddTest("TestEzVec4_XY<int32_t>")
AddTest("TestEzVec4_XY<uint32_t>")
AddTest("TestEzVec4_XY<int64_t>")
AddTest("TestEzVec4_XY<uint64_t>")

AddTest("TestEzVec4_XYZ<float>")
AddTest("TestEzVec4_XYZ<double>")
AddTest("TestEzVec4_XYZ<int32_t>")
AddTest("TestEzVec4_XYZ<uint32_t>")
AddTest("TestEzVec4_XYZ<int64_t>")
AddTest("TestEzVec4_XYZ<uint64_t>")

AddTest("TestEzVec4_ZW<float>")
AddTest("TestEzVec4_ZW<double>")
AddTest("TestEzVec4_ZW<int32_t>")
AddTest("TestEzVec4_ZW<uint32_t>")
AddTest("TestEzVec4_ZW<int64_t>")
AddTest("TestEzVec4_ZW<uint64_t>")

AddTest("TestEzVec4_Increment<float>")
AddTest("TestEzVec4_Increment<double>")
AddTest("TestEzVec4_Increment<int32_t>")
AddTest("TestEzVec4_Increment<uint32_t>")
AddTest("TestEzVec4_Increment<int64_t>")
AddTest("TestEzVec4_Increment<uint64_t>")

AddTest("TestEzVec4_Decrement<float>")
AddTest("TestEzVec4_Decrement<double>")
AddTest("TestEzVec4_Decrement<int32_t>")
AddTest("TestEzVec4_Decrement<uint32_t>")
AddTest("TestEzVec4_Decrement<int64_t>")
AddTest("TestEzVec4_Decrement<uint64_t>")

AddTest("TestEzVec4_Length<float>")
AddTest("TestEzVec4_Length<double>")

AddTest("TestEzVec4_Normalize<float>")
AddTest("TestEzVec4_Normalize<double>")

AddTest("TestEzVec4_Sum<float>")
AddTest("TestEzVec4_Sum<double>")
AddTest("TestEzVec4_Sum<int32_t>")
AddTest("TestEzVec4_Sum<uint32_t>")
AddTest("TestEzVec4_Sum<int64_t>")
AddTest("TestEzVec4_Sum<uint64_t>")

AddTest("TestEzVec4_SumAbs<float>")
AddTest("TestEzVec4_SumAbs<double>")
AddTest("TestEzVec4_SumAbs<int32_t>")
AddTest("TestEzVec4_SumAbs<int64_t>")

AddTest("TestEzVec4_EmptyAND<float>")
AddTest("TestEzVec4_EmptyAND<double>")
AddTest("TestEzVec4_EmptyAND<int32_t>")
AddTest("TestEzVec4_EmptyAND<uint32_t>")
AddTest("TestEzVec4_EmptyAND<int64_t>")
AddTest("TestEzVec4_EmptyAND<uint64_t>")

AddTest("TestEzVec4_EmptyOR<float>")
AddTest("TestEzVec4_EmptyOR<double>")
AddTest("TestEzVec4_EmptyOR<int32_t>")
AddTest("TestEzVec4_EmptyOR<uint32_t>")
AddTest("TestEzVec4_EmptyOR<int64_t>")
AddTest("TestEzVec4_EmptyOR<uint64_t>")

AddTest("TestEzVec4_String<float>")
AddTest("TestEzVec4_String<double>")
AddTest("TestEzVec4_String<int32_t>")
AddTest("TestEzVec4_String<uint32_t>")
AddTest("TestEzVec4_String<int64_t>")
AddTest("TestEzVec4_String<uint64_t>")

AddTest("TestEzVec4_Mini<float>")
AddTest("TestEzVec4_Mini<double>")
AddTest("TestEzVec4_Mini<int32_t>")
AddTest("TestEzVec4_Mini<uint32_t>")
AddTest("TestEzVec4_Mini<int64_t>")
AddTest("TestEzVec4_Mini<uint64_t>")

AddTest("TestEzVec4_Maxi<float>")
AddTest("TestEzVec4_Maxi<double>")
AddTest("TestEzVec4_Maxi<int32_t>")
AddTest("TestEzVec4_Maxi<uint32_t>")
AddTest("TestEzVec4_Maxi<int64_t>")
AddTest("TestEzVec4_Maxi<uint64_t>")

AddTest("TestEzVec4_Equality<float>")
AddTest("TestEzVec4_Equality<double>")
AddTest("TestEzVec4_Equality<int32_t>")
AddTest("TestEzVec4_Equality<uint32_t>")
AddTest("TestEzVec4_Equality<int64_t>")
AddTest("TestEzVec4_Equality<uint64_t>")

AddTest("TestEzVec4_Addition<float>")
AddTest("TestEzVec4_Addition<double>")
AddTest("TestEzVec4_Addition<int32_t>")
AddTest("TestEzVec4_Addition<uint32_t>")
AddTest("TestEzVec4_Addition<int64_t>")
AddTest("TestEzVec4_Addition<uint64_t>")

AddTest("TestEzVec4_Subtraction<float>")
AddTest("TestEzVec4_Subtraction<double>")
AddTest("TestEzVec4_Subtraction<int32_t>")
AddTest("TestEzVec4_Subtraction<uint32_t>")
AddTest("TestEzVec4_Subtraction<int64_t>")
AddTest("TestEzVec4_Subtraction<uint64_t>")

AddTest("TestEzVec4_Multiplication<float>")
AddTest("TestEzVec4_Multiplication<double>")
AddTest("TestEzVec4_Multiplication<int32_t>")
AddTest("TestEzVec4_Multiplication<uint32_t>")
AddTest("TestEzVec4_Multiplication<int64_t>")
AddTest("TestEzVec4_Multiplication<uint64_t>")

AddTest("TestEzVec4_Division<float>")
AddTest("TestEzVec4_Division<double>")
AddTest("TestEzVec4_Division<int32_t>")
AddTest("TestEzVec4_Division<uint32_t>")
AddTest("TestEzVec4_Division<int64_t>")
AddTest("TestEzVec4_Division<uint64_t>")

##########################################################
##### TESTS EzMat2 ########################################
##########################################################

AddTest("TestEzMat2_DefaultConstructor<float>")
AddTest("TestEzMat2_DefaultConstructor<double>")
AddTest("TestEzMat2_DiagonalConstructor<float>")
AddTest("TestEzMat2_DiagonalConstructor<double>")
AddTest("TestEzMat2_ElementConstructor<float>")
AddTest("TestEzMat2_ElementConstructor<double>")
AddTest("TestEzMat2_Identity<float>")
AddTest("TestEzMat2_Identity<double>")
AddTest("TestEzMat2_Zero<float>")
AddTest("TestEzMat2_Zero<double>")
AddTest("TestEzMat2_Multiplication<float>")
AddTest("TestEzMat2_Multiplication<double>")
AddTest("TestEzMat2_MulVec<float>")
AddTest("TestEzMat2_MulVec<double>")
AddTest("TestEzMat2_Transpose<float>")
AddTest("TestEzMat2_Transpose<double>")
AddTest("TestEzMat2_Determinant<float>")
AddTest("TestEzMat2_Determinant<double>")
AddTest("TestEzMat2_Inverse<float>")
AddTest("TestEzMat2_Inverse<double>")
AddTest("TestEzMat2_InverseSingular<float>")
AddTest("TestEzMat2_InverseSingular<double>")

##########################################################
##### TESTS EzQuat ########################################
##########################################################

AddTest("TestEzQuat_DefaultConstructor<float>")
AddTest("TestEzQuat_DefaultConstructor<double>")
AddTest("TestEzQuat_ParameterConstructor<float>")
AddTest("TestEzQuat_ParameterConstructor<double>")
AddTest("TestEzQuat_Add<float>")
AddTest("TestEzQuat_Add<double>")
AddTest("TestEzQuat_Sub<float>")
AddTest("TestEzQuat_Sub<double>")
AddTest("TestEzQuat_Mul<float>")
AddTest("TestEzQuat_Mul<double>")
AddTest("TestEzQuat_Conjugate<float>")
AddTest("TestEzQuat_Conjugate<double>")
AddTest("TestEzQuat_Scale<float>")
AddTest("TestEzQuat_Scale<double>")
AddTest("TestEzQuat_GetT<float>")
AddTest("TestEzQuat_GetT<double>")

##########################################################
##### TESTS EzAABB ########################################
##########################################################

AddTest("TestEzAABB_DefaultConstructor<float>")
AddTest("TestEzAABB_DefaultConstructor<double>")
AddTest("TestEzAABB_Vec2Constructor<float>")
AddTest("TestEzAABB_Vec2Constructor<double>")
AddTest("TestEzAABB_Vec4Constructor<float>")
AddTest("TestEzAABB_Vec4Constructor<double>")
AddTest("TestEzAABB_AddVec<float>")
AddTest("TestEzAABB_AddVec<double>")
AddTest("TestEzAABB_SubVec<float>")
AddTest("TestEzAABB_SubVec<double>")
AddTest("TestEzAABB_MulScalar<float>")
AddTest("TestEzAABB_MulScalar<double>")
AddTest("TestEzAABB_DivScalar<float>")
AddTest("TestEzAABB_DivScalar<double>")
