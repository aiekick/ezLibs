# EzExpr

EzExpr is a easy to use and flexible expression parser / evaluator 
in a one file header only style for cpp11 at least

# Quick Start

```cpp
ez::Expr ev;
auto result0 = ev.parse("x + 2.5 * d").set("x", 0.5).set("d", 1.5).eval().getResult();
auto result1 = ev.set("x", 10.5).eval().getResult();
auto result2 = ev.set("d", 2.5).eval().getResult();
```

```cpp
ez::Expr ev;
ev.addFunction("fma", [](double a, double b, double c) { return a*b+c; }); // ternary function
auto result0 = ev.parse("floor(fma(x,y,k))").set("x", 0.5).set("y", 1.5).set("k", 3.2).eval().getResult();
```

# Features

* Modern use
* One header file only
* Parsing and evaluation on separated calls
* User can easily add their custom Unary/Binary/Ternary Function
* Cpp11 compatible at least
* Can identify and return to user the variables
* Many exceptions are emited for many not conformant issue during parsing and during evaluation
* Use their own String class for faster response
* all numbers and computation are in double precision

# Builtins


```cpp
+ - * / ^ % 
! (factorielle : N!)
```

## Constants

* pi
* e

## Unary function

* abs, floor, ceil, round, fract, sign
* sin, cos, tan
* asin, acos, atan
* sinh, cosh, tanh
* asinh, acosh, atanh
* ln, log, log1p, logb, log2, log10
* exp
* sqrt

## Binary function

* mod, atan2, min, max, step, hypot, smoothabs

## Ternary function

* clamp, mis, smoothstep (glsl functions)
* saturate, lerp (hlsl functions)
