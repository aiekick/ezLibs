
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

#include <TestEzExprBuiltins.h>
#include <ezlibs/ezExpr.hpp>

////////////////////////////////////////////////////////////////////////////
//// BUILTINS //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// Tests for floor
bool TestEzExpr_Builtins_Floor_1() {
    ez::Expr ev;
    return ev.parse("floor(4.7)").eval().check(4);  // floor(4.7) = 4
}

bool TestEzExpr_Builtins_Floor_2() {
    ez::Expr ev;
    return ev.parse("floor(-4.7)").eval().check(-5);  // floor(-4.7) = -5
}

bool TestEzExpr_Builtins_Floor_3() {
    ez::Expr ev;
    return ev.parse("floor(0.3)").eval().check(0);  // floor(0.3) = 0
}

// Tests for ceil
bool TestEzExpr_Builtins_Ceil_1() {
    ez::Expr ev;
    return ev.parse("ceil(4.3)").eval().check(5);  // ceil(4.3) = 5
}

bool TestEzExpr_Builtins_Ceil_2() {
    ez::Expr ev;
    return ev.parse("ceil(-4.3)").eval().check(-4);  // ceil(-4.3) = -4
}

bool TestEzExpr_Builtins_Ceil_3() {
    ez::Expr ev;
    return ev.parse("ceil(0.7)").eval().check(1);  // ceil(0.7) = 1
}

// Tests for round
bool TestEzExpr_Builtins_Round_1() {
    ez::Expr ev;
    return ev.parse("round(4.5)").eval().check(5);  // round(4.5) = 5
}

bool TestEzExpr_Builtins_Round_2() {
    ez::Expr ev;
    return ev.parse("round(-4.5)").eval().check(-5);  // round(-4.5) = -5
}

bool TestEzExpr_Builtins_Round_3() {
    ez::Expr ev;
    return ev.parse("round(2.3)").eval().check(2);  // round(2.3) = 2
}

// Tests for fract
bool TestEzExpr_Builtins_Fract_1() {
    ez::Expr ev;
    return ev.parse("fract(4.7)").eval().check(0.7);  // fract(4.7) = 0.7
}

bool TestEzExpr_Builtins_Fract_2() {
    ez::Expr ev;
    return ev.parse("fract(-4.7)").eval().check(0.3);  // fract(-4.7) = 0.3 (since fract(-x) = 1 - fract(x))
}

bool TestEzExpr_Builtins_Fract_3() {
    ez::Expr ev;
    return ev.parse("fract(5)").eval().check(0);  // fract(5) = 0
}

// Tests for sign
bool TestEzExpr_Builtins_Sign_1() {
    ez::Expr ev;
    return ev.parse("sign(-3)").eval().check(-1);  // sign(-3) = -1
}

bool TestEzExpr_Builtins_Sign_2() {
    ez::Expr ev;
    return ev.parse("sign(3)").eval().check(1);  // sign(3) = 1
}

bool TestEzExpr_Builtins_Sign_3() {
    ez::Expr ev;
    return ev.parse("sign(0)").eval().check(0);  // sign(0) = 0
}

// Tests for step
bool TestEzExpr_Builtins_Step_1() {
    ez::Expr ev;
    return ev.parse("step(1, 0.5)").eval().check(0);  // step(1, 0.5) = 0
}

bool TestEzExpr_Builtins_Step_2() {
    ez::Expr ev;
    return ev.parse("step(1, 1)").eval().check(1);  // step(1, 1) = 1
}

bool TestEzExpr_Builtins_Step_3() {
    ez::Expr ev;
    return ev.parse("step(1, 2)").eval().check(1);  // step(1, 2) = 1
}

// Tests for hypot
bool TestEzExpr_Builtins_Hypot_1() {
    ez::Expr ev;
    return ev.parse("hypot(3, 4)").eval().check(5);  // hypot(3, 4) = 5
}

bool TestEzExpr_Builtins_Hypot_2() {
    ez::Expr ev;
    return ev.parse("hypot(5, 12)").eval().check(13);  // hypot(5, 12) = 13
}

bool TestEzExpr_Builtins_Hypot_3() {
    ez::Expr ev;
    return ev.parse("hypot(8, 15)").eval().check(17);  // hypot(8, 15) = 17
}

// Tests for clamp
bool TestEzExpr_Builtins_Clamp_1() {
    ez::Expr ev;
    return ev.parse("clamp(5, 0, 10)").eval().check(5);  // clamp(5, 0, 10) = 5
}

bool TestEzExpr_Builtins_Clamp_2() {
    ez::Expr ev;
    return ev.parse("clamp(-1, 0, 10)").eval().check(0);  // clamp(-1, 0, 10) = 0
}

bool TestEzExpr_Builtins_Clamp_3() {
    ez::Expr ev;
    return ev.parse("clamp(15, 0, 10)").eval().check(10);  // clamp(15, 0, 10) = 10
}

// Tests for saturate
bool TestEzExpr_Builtins_Saturate_1() {
    ez::Expr ev;
    return ev.parse("saturate(0.5)").eval().check(0.5);  // saturate(0.5, 0, 1) = 0.5
}

bool TestEzExpr_Builtins_Saturate_2() {
    ez::Expr ev;
    return ev.parse("saturate(-0.5)").eval().check(0);  // saturate(-0.5, 0, 1) = 0
}

bool TestEzExpr_Builtins_Saturate_3() {
    ez::Expr ev;
    return ev.parse("saturate(1.5)").eval().check(1);  // saturate(1.5, 0, 1) = 1
}

// Tests for lerp
bool TestEzExpr_Builtins_Lerp_1() {
    ez::Expr ev;
    return ev.parse("lerp(0, 10, 0.5)").eval().check(5);  // lerp(0, 10, 0.5) = 5
}

bool TestEzExpr_Builtins_Lerp_2() {
    ez::Expr ev;
    return ev.parse("lerp(10, 20, 0.25)").eval().check(12.5);  // lerp(10, 20, 0.25) = 12.5
}

bool TestEzExpr_Builtins_Lerp_3() {
    ez::Expr ev;
    return ev.parse("lerp(-10, 10, 0.5)").eval().check(0);  // lerp(-10, 10, 0.5) = 0
}

// Tests for mix
bool TestEzExpr_Builtins_Mix_1() {
    ez::Expr ev;
    return ev.parse("mix(0, 10, 0.5)").eval().check(5);  // mix(0, 10, 0.5) = 5
}

bool TestEzExpr_Builtins_Mix_2() {
    ez::Expr ev;
    return ev.parse("mix(10, 20, 0.25)").eval().check(12.5);  // mix(10, 20, 0.25) = 12.5
}

bool TestEzExpr_Builtins_Mix_3() {
    ez::Expr ev;
    return ev.parse("mix(-10, 10, 0.5)").eval().check(0);  // mix(-10, 10, 0.5) = 0
}

// Tests for smoothstep
bool TestEzExpr_Builtins_SmoothStep_1() {
    ez::Expr ev;
    return ev.parse("smoothstep(0, 1, 0.5)").eval().check(0.5);  // smoothstep(0, 1, 0.5) = 0.5
}

bool TestEzExpr_Builtins_SmoothStep_2() {
    ez::Expr ev;
    return ev.parse("smoothstep(0, 1, 0.25)").eval().check(0.15625);  // smoothstep(0, 1, 0.25) = 0.15625
}

bool TestEzExpr_Builtins_SmoothStep_3() {
    ez::Expr ev;
    return ev.parse("smoothstep(0, 1, 0.75)").eval().check(0.84375);  // smoothstep(0, 1, 0.75) = 0.84375
}

// Tests for smoothabs
bool TestEzExpr_Builtins_SmoothAbs_1() {
    ez::Expr ev;
    return ev.parse("smoothabs(-0.5, -2.8)").eval().check(std::sqrt(3.05));  // smoothabs(-0.5, -2.8) = sqrt(3.05)
}

bool TestEzExpr_Builtins_SmoothAbs_2() {
    ez::Expr ev;
    return ev.parse("smoothabs(-0.5, 0.0)").eval().check(0.5);  // smoothabs(-0.5, 0.0) = 0.25
}

bool TestEzExpr_Builtins_SmoothAbs_3() {
    ez::Expr ev;
    return ev.parse("smoothabs(-0.5, 2.8)").eval().check(std::sqrt(3.05));  // smoothabs(-0.5, -2.8) = sqrt(3.05)
}

// Tests for abs
bool TestEzExpr_Builtins_Abs_1() {
    ez::Expr ev;
    return ev.parse("abs(-5)").eval().check(5);  // abs(-5) = 5
}

bool TestEzExpr_Builtins_Abs_2() {
    ez::Expr ev;
    return ev.parse("abs(5)").eval().check(5);  // abs(5) = 5
}

bool TestEzExpr_Builtins_Abs_3() {
    ez::Expr ev;
    return ev.parse("abs(0)").eval().check(0);  // abs(0) = 0
}

// Tests for sin
bool TestEzExpr_Builtins_Sin_1() {
    ez::Expr ev;
    return ev.parse("sin(0)").eval().check(0);  // sin(0) = 0
}

bool TestEzExpr_Builtins_Sin_2() {
    ez::Expr ev;
    return ev.parse("sin(pi / 2)").eval().check(1);  // sin(pi / 2) = 1
}

bool TestEzExpr_Builtins_Sin_3() {
    ez::Expr ev;
    return ev.parse("sin(pi)").eval().check(0);  // sin(pi) ≈ 0
}

// Tests for cos
bool TestEzExpr_Builtins_Cos_1() {
    ez::Expr ev;
    return ev.parse("cos(0)").eval().check(1);  // cos(0) = 1
}

bool TestEzExpr_Builtins_Cos_2() {
    ez::Expr ev;
    return ev.parse("cos(pi)").eval().check(-1);  // cos(pi) = -1
}

bool TestEzExpr_Builtins_Cos_3() {
    ez::Expr ev;
    return ev.parse("cos(pi / 2)").eval().check(0);  // cos(pi / 2) ≈ 0
}

// Tests for tan
bool TestEzExpr_Builtins_Tan_1() {
    ez::Expr ev;
    return ev.parse("tan(0)").eval().check(0);  // tan(0) = 0
}

bool TestEzExpr_Builtins_Tan_2() {
    ez::Expr ev;
    return ev.parse("tan(pi / 4)").eval().check(1);  // tan(pi / 4) = 1
}

bool TestEzExpr_Builtins_Tan_3() {
    ez::Expr ev;
    return ev.parse("tan(pi)").eval().check(0);  // tan(pi) ≈ 0
}

// Tests for asin
bool TestEzExpr_Builtins_Asin_1() {
    ez::Expr ev;
    return ev.parse("asin(0)").eval().check(0);  // asin(0) = 0
}

bool TestEzExpr_Builtins_Asin_2() {
    ez::Expr ev;
    return ev.parse("asin(1)").eval().check(M_PI / 2);  // asin(1) = pi/2
}

bool TestEzExpr_Builtins_Asin_3() {
    ez::Expr ev;
    return ev.parse("asin(-1)").eval().check(-M_PI / 2);  // asin(-1) = -pi/2
}

// Tests for acos
bool TestEzExpr_Builtins_Acos_1() {
    ez::Expr ev;
    return ev.parse("acos(1)").eval().check(0);  // acos(1) = 0
}

bool TestEzExpr_Builtins_Acos_2() {
    ez::Expr ev;
    return ev.parse("acos(0)").eval().check(M_PI / 2);  // acos(0) = pi/2
}

bool TestEzExpr_Builtins_Acos_3() {
    ez::Expr ev;
    return ev.parse("acos(-1)").eval().check(M_PI);  // acos(-1) = pi
}

// Tests for atan
bool TestEzExpr_Builtins_Atan_1() {
    ez::Expr ev;
    return ev.parse("atan(0)").eval().check(0);  // atan(0) = 0
}

bool TestEzExpr_Builtins_Atan_2() {
    ez::Expr ev;
    return ev.parse("atan(1)").eval().check(M_PI / 4);  // atan(1) = pi/4
}

bool TestEzExpr_Builtins_Atan_3() {
    ez::Expr ev;
    return ev.parse("atan(-1)").eval().check(-M_PI / 4);  // atan(-1) = -pi/4
}

// Tests for sinh
bool TestEzExpr_Builtins_Sinh_1() {
    ez::Expr ev;
    return ev.parse("sinh(0)").eval().check(0);  // sinh(0) = 0
}

bool TestEzExpr_Builtins_Sinh_2() {
    ez::Expr ev;
    return ev.parse("sinh(1)").eval().check(::std::sinh(1));  // sinh(1) = sinh(1)
}

bool TestEzExpr_Builtins_Sinh_3() {
    ez::Expr ev;
    return ev.parse("sinh(-1)").eval().check(::std::sinh(-1));  // sinh(-1) = sinh(-1)
}

// Tests for cosh
bool TestEzExpr_Builtins_Cosh_1() {
    ez::Expr ev;
    return ev.parse("cosh(0)").eval().check(1);  // cosh(0) = 1
}

bool TestEzExpr_Builtins_Cosh_2() {
    ez::Expr ev;
    return ev.parse("cosh(1)").eval().check(::std::cosh(1));  // cosh(1) = cosh(1)
}

bool TestEzExpr_Builtins_Cosh_3() {
    ez::Expr ev;
    return ev.parse("cosh(-1)").eval().check(::std::cosh(-1));  // cosh(-1) = cosh(-1)
}

// Tests for tanh
bool TestEzExpr_Builtins_Tanh_1() {
    ez::Expr ev;
    return ev.parse("tanh(0)").eval().check(0);  // tanh(0) = 0
}

bool TestEzExpr_Builtins_Tanh_2() {
    ez::Expr ev;
    return ev.parse("tanh(1)").eval().check(::std::tanh(1));  // tanh(1) = tanh(1)
}

bool TestEzExpr_Builtins_Tanh_3() {
    ez::Expr ev;
    return ev.parse("tanh(-1)").eval().check(::std::tanh(-1));  // tanh(-1) = tanh(-1)
}

// Tests for asinh
bool TestEzExpr_Builtins_Asinh_1() {
    ez::Expr ev;
    return ev.parse("asinh(0)").eval().check(0);  // asinh(0) = 0
}

bool TestEzExpr_Builtins_Asinh_2() {
    ez::Expr ev;
    return ev.parse("asinh(1)").eval().check(::std::asinh(1));  // asinh(1) = asinh(1)
}

bool TestEzExpr_Builtins_Asinh_3() {
    ez::Expr ev;
    return ev.parse("asinh(-1)").eval().check(::std::asinh(-1));  // asinh(-1) = asinh(-1)
}

// Tests for acosh
bool TestEzExpr_Builtins_Acosh_1() {
    ez::Expr ev;
    return ev.parse("acosh(1)").eval().check(0);  // acosh(1) = 0
}

bool TestEzExpr_Builtins_Acosh_2() {
    ez::Expr ev;
    return ev.parse("acosh(2)").eval().check(::std::acosh(2));  // acosh(2) = acosh(2)
}

bool TestEzExpr_Builtins_Acosh_3() {
    ez::Expr ev;
    return ev.parse("acosh(10)").eval().check(::std::acosh(10));  // acosh(10) = acosh(10)
}

// Tests for atanh
bool TestEzExpr_Builtins_Atanh_1() {
    ez::Expr ev;
    return ev.parse("atanh(0)").eval().check(0);  // atanh(0) = 0
}

bool TestEzExpr_Builtins_Atanh_2() {
    ez::Expr ev;
    return ev.parse("atanh(0.5)").eval().check(::std::atanh(0.5));  // atanh(0.5) = atanh(0.5)
}

bool TestEzExpr_Builtins_Atanh_3() {
    ez::Expr ev;
    return ev.parse("atanh(-0.5)").eval().check(::std::atanh(-0.5));  // atanh(-0.5) = atanh(-0.5)
}

// Tests for ln
bool TestEzExpr_Builtins_Ln_1() {
    ez::Expr ev;
    return ev.parse("ln(1)").eval().check(0);  // ln(1) = 0
}

bool TestEzExpr_Builtins_Ln_2() {
    ez::Expr ev;
    return ev.parse("ln(e)").eval().check(1);  // ln(e) = 1
}

bool TestEzExpr_Builtins_Ln_3() {
    ez::Expr ev;
    return ev.parse("ln(e^2)").eval().check(2);  // ln(e^2) = 2
}

// Tests for log
bool TestEzExpr_Builtins_Log_1() {
    ez::Expr ev;
    return ev.parse("log(1)").eval().check(0);  // log(1) = 0
}

bool TestEzExpr_Builtins_Log_2() {
    ez::Expr ev;
    return ev.parse("log(e)").eval().check(1);  // log(e) = 1
}

bool TestEzExpr_Builtins_Log_3() {
    ez::Expr ev;
    return ev.parse("log(e^2)").eval().check(2);  // log(e^2) = 2
}

// Tests for log1p
bool TestEzExpr_Builtins_Log1p_1() {
    ez::Expr ev;
    return ev.parse("log1p(1)").eval().check(::std::log1p(1));  // log1p(1) = log(1 + 1)
}

bool TestEzExpr_Builtins_Log1p_2() {
    ez::Expr ev;
    return ev.parse("log1p(0)").eval().check(0);  // log1p(0) = log(1) = 0
}

bool TestEzExpr_Builtins_Log1p_3() {
    ez::Expr ev;
    return ev.parse("log1p(2)").eval().check(::std::log1p(2));  // log1p(2) = log(1 + 2)
}

// Tests for log2
bool TestEzExpr_Builtins_Log2_1() {
    ez::Expr ev;
    return ev.parse("log2(8)").eval().check(3);  // log2(8) = 3
}

bool TestEzExpr_Builtins_Log2_2() {
    ez::Expr ev;
    return ev.parse("log2(1)").eval().check(0);  // log2(1) = 0
}

bool TestEzExpr_Builtins_Log2_3() {
    ez::Expr ev;
    return ev.parse("log2(16)").eval().check(4);  // log2(16) = 4
}

// Tests for log10
bool TestEzExpr_Builtins_Log10_1() {
    ez::Expr ev;
    return ev.parse("log10(10)").eval().check(1);  // log10(10) = 1
}

bool TestEzExpr_Builtins_Log10_2() {
    ez::Expr ev;
    return ev.parse("log10(100)").eval().check(2);  // log10(100) = 2
}

bool TestEzExpr_Builtins_Log10_3() {
    ez::Expr ev;
    return ev.parse("log10(1000)").eval().check(3);  // log10(1000) = 3
}

// Tests for exp
bool TestEzExpr_Builtins_Exp_1() {
    ez::Expr ev;
    return ev.parse("exp(0)").eval().check(1);  // exp(0) = 1
}

bool TestEzExpr_Builtins_Exp_2() {
    ez::Expr ev;
    return ev.parse("exp(1)").eval().check(M_E);  // exp(1) = e
}

bool TestEzExpr_Builtins_Exp_3() {
    ez::Expr ev;
    return ev.parse("exp(2)").eval().check(M_E * M_E);  // exp(2) = e^2
}

// Tests for sqrt
bool TestEzExpr_Builtins_Sqrt_1() {
    ez::Expr ev;
    return ev.parse("sqrt(0)").eval().check(0);  // sqrt(0) = 0
}

bool TestEzExpr_Builtins_Sqrt_2() {
    ez::Expr ev;
    return ev.parse("sqrt(4)").eval().check(2);  // sqrt(4) = 2
}

bool TestEzExpr_Builtins_Sqrt_3() {
    ez::Expr ev;
    return ev.parse("sqrt(2)").eval().check(::std::sqrt(2));  // sqrt(2) = √2
}

// Tests for mod
bool TestEzExpr_Builtins_Mod_1() {
    ez::Expr ev;
    return ev.parse("mod(10, 3)").eval().check(1);  // mod(10, 3) = 1
}

bool TestEzExpr_Builtins_Mod_2() {
    ez::Expr ev;
    return ev.parse("mod(10, 2)").eval().check(0);  // mod(10, 2) = 0
}

bool TestEzExpr_Builtins_Mod_3() {
    ez::Expr ev;
    return ev.parse("mod(10, 4)").eval().check(2);  // mod(10, 4) = 2
}

// Tests for atan2
bool TestEzExpr_Builtins_Atan2_1() {
    ez::Expr ev;
    return ev.parse("atan2(0, 1)").eval().check(0);  // atan2(0, 1) = 0
}

bool TestEzExpr_Builtins_Atan2_2() {
    ez::Expr ev;
    return ev.parse("atan2(1, 1)").eval().check(M_PI / 4);  // atan2(1, 1) = pi/4
}

bool TestEzExpr_Builtins_Atan2_3() {
    ez::Expr ev;
    return ev.parse("atan2(-1, -1)").eval().check(-3 * M_PI / 4);  // atan2(-1, -1) = -3pi/4
}

// Tests for min
bool TestEzExpr_Builtins_Min_1() {
    ez::Expr ev;
    return ev.parse("min(1, 2)").eval().check(1);  // min(1, 2) = 1
}

bool TestEzExpr_Builtins_Min_2() {
    ez::Expr ev;
    return ev.parse("min(5, 3)").eval().check(3);  // min(5, 3) = 3
}

bool TestEzExpr_Builtins_Min_3() {
    ez::Expr ev;
    return ev.parse("min(0, -1)").eval().check(-1);  // min(0, -1) = -1
}

// Tests for max
bool TestEzExpr_Builtins_Max_1() {
    ez::Expr ev;
    return ev.parse("max(1, 2)").eval().check(2);  // max(1, 2) = 2
}

bool TestEzExpr_Builtins_Max_2() {
    ez::Expr ev;
    return ev.parse("max(5, 3)").eval().check(5);  // max(5, 3) = 5
}

bool TestEzExpr_Builtins_Max_3() {
    ez::Expr ev;
    return ev.parse("max(0, -1)").eval().check(0);  // max(0, -1) = 0
}

bool TestEzExpr_Builtins_Factorial_1() {
    ez::Expr ev;
    return ev.parse("5!").eval().check(120);  // 5! = 120
}

bool TestEzExpr_Builtins_Factorial_2() {
    ez::Expr ev;
    return ev.parse("0!").eval().check(1);  // 0! = 1
}

bool TestEzExpr_Builtins_Factorial_3() {
    ez::Expr ev;
    return ev.parse("3!").eval().check(6);  // 3! = 6
}
bool TestEzExpr_Builtins_ModuloPercent_1() {
    ez::Expr ev;
    return ev.parse("10 % 3").eval().check(1);  // 10 % 3 = 1
}

bool TestEzExpr_Builtins_ModuloPercent_2() {
    ez::Expr ev;
    return ev.parse("25 % 5").eval().check(0);  // 25 % 5 = 0
}

bool TestEzExpr_Builtins_ModuloPercent_3() {
    ez::Expr ev;
    return ev.parse("7 % 4").eval().check(3);  // 7 % 4 = 3
}

bool TestEzExpr_Builtins_ModuloMod_1() {
    ez::Expr ev;
    return ev.parse("mod(10, 3)").eval().check(1);  // mod(10, 3) = 1
}

bool TestEzExpr_Builtins_ModuloMod_2() {
    ez::Expr ev;
    return ev.parse("mod(10, 2)").eval().check(0);  // mod(10, 2) = 0
}

bool TestEzExpr_Builtins_ModuloMod_3() {
    ez::Expr ev;
    return ev.parse("mod(10, 4)").eval().check(2);  // mod(10, 4) = 2
}

////////////////////////////////////////////////////////////////////////////
//// ENTRY POINT ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzExpr_Builtins(const std::string& vTest) {
    // Builtins
    IfTestExist(TestEzExpr_Builtins_Floor_1);
    else IfTestExist(TestEzExpr_Builtins_Floor_2);
    else IfTestExist(TestEzExpr_Builtins_Floor_3);
    else IfTestExist(TestEzExpr_Builtins_Ceil_1);
    else IfTestExist(TestEzExpr_Builtins_Ceil_2);
    else IfTestExist(TestEzExpr_Builtins_Ceil_3);
    else IfTestExist(TestEzExpr_Builtins_Round_1);
    else IfTestExist(TestEzExpr_Builtins_Round_2);
    else IfTestExist(TestEzExpr_Builtins_Round_3);
    else IfTestExist(TestEzExpr_Builtins_Fract_1);
    else IfTestExist(TestEzExpr_Builtins_Fract_2);
    else IfTestExist(TestEzExpr_Builtins_Fract_3);
    else IfTestExist(TestEzExpr_Builtins_Sign_1);
    else IfTestExist(TestEzExpr_Builtins_Sign_2);
    else IfTestExist(TestEzExpr_Builtins_Sign_3);
    else IfTestExist(TestEzExpr_Builtins_Step_1);
    else IfTestExist(TestEzExpr_Builtins_Step_2);
    else IfTestExist(TestEzExpr_Builtins_Step_3);
    else IfTestExist(TestEzExpr_Builtins_Hypot_1);
    else IfTestExist(TestEzExpr_Builtins_Hypot_2);
    else IfTestExist(TestEzExpr_Builtins_Hypot_3);
    else IfTestExist(TestEzExpr_Builtins_Clamp_1);
    else IfTestExist(TestEzExpr_Builtins_Clamp_2);
    else IfTestExist(TestEzExpr_Builtins_Clamp_3);
    else IfTestExist(TestEzExpr_Builtins_Saturate_1);
    else IfTestExist(TestEzExpr_Builtins_Saturate_2);
    else IfTestExist(TestEzExpr_Builtins_Saturate_3);
    else IfTestExist(TestEzExpr_Builtins_Lerp_1);
    else IfTestExist(TestEzExpr_Builtins_Lerp_2);
    else IfTestExist(TestEzExpr_Builtins_Lerp_3);
    else IfTestExist(TestEzExpr_Builtins_Mix_1);
    else IfTestExist(TestEzExpr_Builtins_Mix_2);
    else IfTestExist(TestEzExpr_Builtins_Mix_3);
    else IfTestExist(TestEzExpr_Builtins_SmoothStep_1);
    else IfTestExist(TestEzExpr_Builtins_SmoothStep_2);
    else IfTestExist(TestEzExpr_Builtins_SmoothStep_3);
    else IfTestExist(TestEzExpr_Builtins_SmoothAbs_1);
    else IfTestExist(TestEzExpr_Builtins_SmoothAbs_2);
    else IfTestExist(TestEzExpr_Builtins_SmoothAbs_3);
    else IfTestExist(TestEzExpr_Builtins_Abs_1);
    else IfTestExist(TestEzExpr_Builtins_Abs_2);
    else IfTestExist(TestEzExpr_Builtins_Abs_3);
    else IfTestExist(TestEzExpr_Builtins_Sin_1);
    else IfTestExist(TestEzExpr_Builtins_Sin_2);
    else IfTestExist(TestEzExpr_Builtins_Sin_3);
    else IfTestExist(TestEzExpr_Builtins_Cos_1);
    else IfTestExist(TestEzExpr_Builtins_Cos_2);
    else IfTestExist(TestEzExpr_Builtins_Cos_3);
    else IfTestExist(TestEzExpr_Builtins_Tan_1);
    else IfTestExist(TestEzExpr_Builtins_Tan_2);
    else IfTestExist(TestEzExpr_Builtins_Tan_3);
    else IfTestExist(TestEzExpr_Builtins_Asin_1);
    else IfTestExist(TestEzExpr_Builtins_Asin_2);
    else IfTestExist(TestEzExpr_Builtins_Asin_3);
    else IfTestExist(TestEzExpr_Builtins_Acos_1);
    else IfTestExist(TestEzExpr_Builtins_Acos_2);
    else IfTestExist(TestEzExpr_Builtins_Acos_3);
    else IfTestExist(TestEzExpr_Builtins_Atan_1);
    else IfTestExist(TestEzExpr_Builtins_Atan_2);
    else IfTestExist(TestEzExpr_Builtins_Atan_3);
    else IfTestExist(TestEzExpr_Builtins_Sinh_1);
    else IfTestExist(TestEzExpr_Builtins_Sinh_2);
    else IfTestExist(TestEzExpr_Builtins_Sinh_3);
    else IfTestExist(TestEzExpr_Builtins_Cosh_1);
    else IfTestExist(TestEzExpr_Builtins_Cosh_2);
    else IfTestExist(TestEzExpr_Builtins_Cosh_3);
    else IfTestExist(TestEzExpr_Builtins_Tanh_1);
    else IfTestExist(TestEzExpr_Builtins_Tanh_2);
    else IfTestExist(TestEzExpr_Builtins_Tanh_3);
    else IfTestExist(TestEzExpr_Builtins_Asinh_1);
    else IfTestExist(TestEzExpr_Builtins_Asinh_2);
    else IfTestExist(TestEzExpr_Builtins_Asinh_3);
    else IfTestExist(TestEzExpr_Builtins_Acosh_1);
    else IfTestExist(TestEzExpr_Builtins_Acosh_2);
    else IfTestExist(TestEzExpr_Builtins_Acosh_3);
    else IfTestExist(TestEzExpr_Builtins_Atanh_1);
    else IfTestExist(TestEzExpr_Builtins_Atanh_2);
    else IfTestExist(TestEzExpr_Builtins_Atanh_3);
    else IfTestExist(TestEzExpr_Builtins_Ln_1);
    else IfTestExist(TestEzExpr_Builtins_Ln_2);
    else IfTestExist(TestEzExpr_Builtins_Ln_3);
    else IfTestExist(TestEzExpr_Builtins_Log_1);
    else IfTestExist(TestEzExpr_Builtins_Log_2);
    else IfTestExist(TestEzExpr_Builtins_Log_3);
    else IfTestExist(TestEzExpr_Builtins_Log1p_1);
    else IfTestExist(TestEzExpr_Builtins_Log1p_2);
    else IfTestExist(TestEzExpr_Builtins_Log1p_3);
    else IfTestExist(TestEzExpr_Builtins_Log2_1);
    else IfTestExist(TestEzExpr_Builtins_Log2_2);
    else IfTestExist(TestEzExpr_Builtins_Log2_3);
    else IfTestExist(TestEzExpr_Builtins_Log10_1);
    else IfTestExist(TestEzExpr_Builtins_Log10_2);
    else IfTestExist(TestEzExpr_Builtins_Log10_3);
    else IfTestExist(TestEzExpr_Builtins_Exp_1);
    else IfTestExist(TestEzExpr_Builtins_Exp_2);
    else IfTestExist(TestEzExpr_Builtins_Exp_3);
    else IfTestExist(TestEzExpr_Builtins_Sqrt_1);
    else IfTestExist(TestEzExpr_Builtins_Sqrt_2);
    else IfTestExist(TestEzExpr_Builtins_Sqrt_3);
    else IfTestExist(TestEzExpr_Builtins_Mod_1);
    else IfTestExist(TestEzExpr_Builtins_Mod_2);
    else IfTestExist(TestEzExpr_Builtins_Mod_3);
    else IfTestExist(TestEzExpr_Builtins_Atan2_1);
    else IfTestExist(TestEzExpr_Builtins_Atan2_2);
    else IfTestExist(TestEzExpr_Builtins_Atan2_3);
    else IfTestExist(TestEzExpr_Builtins_Min_1);
    else IfTestExist(TestEzExpr_Builtins_Min_2);
    else IfTestExist(TestEzExpr_Builtins_Min_3);
    else IfTestExist(TestEzExpr_Builtins_Max_1);
    else IfTestExist(TestEzExpr_Builtins_Max_2);
    else IfTestExist(TestEzExpr_Builtins_Max_3);
    else IfTestExist(TestEzExpr_Builtins_Factorial_1);
    else IfTestExist(TestEzExpr_Builtins_Factorial_2);
    else IfTestExist(TestEzExpr_Builtins_Factorial_3);
    else IfTestExist(TestEzExpr_Builtins_ModuloPercent_1);
    else IfTestExist(TestEzExpr_Builtins_ModuloPercent_2);
    else IfTestExist(TestEzExpr_Builtins_ModuloPercent_3);
    else IfTestExist(TestEzExpr_Builtins_ModuloMod_1);
    else IfTestExist(TestEzExpr_Builtins_ModuloMod_2);
    else IfTestExist(TestEzExpr_Builtins_ModuloMod_3);
    // default
    return false;
}
