
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

#include <TestEzExprPerfos.h>
#include <ezlibs/ezExpr.hpp>
#include <functional>
#include <iostream>
#include <cassert>
#include <chrono>
#include <fstream>

static bool Bench(const std::string& vEzExpr,
                  const std::function<double(ez::Expr& vEv, double)>& vEzExprIteration,
                  const std::function<double(double)>& vCppExpr,
                  int vIterations,
                  double slowdownThreshold,
                  double& vOutEzExprTotalTime,
                  double& vOutEzExprAvgTime,
                  double& vOutCppTotalTime,
                  double& vOutCppAvgTime,
                  std::ostream* outputStreamPtr) {
    try {
        assert(!vEzExpr.empty());
        ez::Expr ev;
        ev.parse(vEzExpr);

        double ezExprAccumulation = 0.0;
        double cppExprAccumulation = 0.0;
        double tmp;

        // EzExpr Eval
        auto start_ez = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < vIterations; ++j) {
            for (int i = 0; i < vIterations; ++i) {
                tmp = i;
                ezExprAccumulation += vEzExprIteration(ev, tmp);
            }
        }
        auto end_ez = std::chrono::high_resolution_clock::now();
        vOutEzExprTotalTime = std::chrono::duration<double, std::milli>(end_ez - start_ez).count();

        // Cpp Eval
        auto start_cpp = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < vIterations; ++j) {
            for (int i = 0; i < vIterations; ++i) {
                tmp = i;
                cppExprAccumulation += vCppExpr(tmp);
            }
        }
        auto end_cpp = std::chrono::high_resolution_clock::now();
        vOutCppTotalTime = std::chrono::duration<double, std::milli>(end_cpp - start_cpp).count();

        double slowdown_total_percentage = ((vOutEzExprTotalTime / vOutCppTotalTime) - 1.0) * 100.0;

        if (outputStreamPtr != nullptr) {
            (*outputStreamPtr) <<                                            //
                "| " << vEzExpr <<                                           //
                " | " << std::floor(vOutEzExprTotalTime * 100.0) / 100.0 <<  //
                " | " << std::floor(vOutCppTotalTime * 100.0) / 100.0 <<     //
                " | " << slowdown_total_percentage <<                        //
                "% |\n";
        }

        // the test will always been true, the goal is to geenrete a report
        /*if (slowdown_total_percentage > slowdownThreshold) {
            return false;
        }*/

        // Comparaison des résultats accumulés
        if (std::abs(ezExprAccumulation - cppExprAccumulation) > std::numeric_limits<double>::epsilon()) {
            if (outputStreamPtr != nullptr) {  //
                (*outputStreamPtr) << "| " <<  //
                    vEzExpr << " | Accumulation mismatch |" << std::endl;
            }
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////
//// PERFOS ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzExpr_Perfos_x_squared(double slowdownThreshold, int iterations, std::ostream* outputStreamPtr) {
    double ez_expr_total_time, ez_expr_avg_time;
    double cpp_expr_total_time, cpp_expr_avg_time;

    return Bench(
        "x * x",  //
        [](ez::Expr& ev, double t) { return ev.set("x", t).eval().getResult(); },
        [](double t) { return t * t; },
        iterations,
        slowdownThreshold,
        ez_expr_total_time,
        ez_expr_avg_time,
        cpp_expr_total_time,
        cpp_expr_avg_time,
        outputStreamPtr);
}

bool TestEzExpr_Perfos_x_cubed(double slowdownThreshold, int iterations, std::ostream* outputStreamPtr) {
    double ez_expr_total_time, ez_expr_avg_time;
    double cpp_expr_total_time, cpp_expr_avg_time;

    return Bench(
        "x * x * x",  //
        [](ez::Expr& ev, double t) { return ev.set("x", t).eval().getResult(); },
        [](double t) { return t * t * t; },
        iterations,
        slowdownThreshold,
        ez_expr_total_time,
        ez_expr_avg_time,
        cpp_expr_total_time,
        cpp_expr_avg_time,
        outputStreamPtr);
}

bool TestEzExpr_Perfos_sin_x(double slowdownThreshold, int iterations, std::ostream* outputStreamPtr) {
    double ez_expr_total_time, ez_expr_avg_time;
    double cpp_expr_total_time, cpp_expr_avg_time;

    return Bench(
        "sin(x)",  //
        [](ez::Expr& ev, double t) { return ev.set("x", t).eval().getResult(); },
        [](double t) { return std::sin(t); },
        iterations,
        slowdownThreshold,
        ez_expr_total_time,
        ez_expr_avg_time,
        cpp_expr_total_time,
        cpp_expr_avg_time,
        outputStreamPtr);
}

bool TestEzExpr_Perfos_sqrt_x(double slowdownThreshold, int iterations, std::ostream* outputStreamPtr) {
    double ez_expr_total_time, ez_expr_avg_time;
    double cpp_expr_total_time, cpp_expr_avg_time;

    return Bench(
        "sqrt(x)",  //
        [](ez::Expr& ev, double t) { return ev.set("x", t).eval().getResult(); },
        [](double t) { return std::sqrt(t); },
        iterations,
        slowdownThreshold,
        ez_expr_total_time,
        ez_expr_avg_time,
        cpp_expr_total_time,
        cpp_expr_avg_time,
        outputStreamPtr);
}

bool TestEzExpr_Perfos_cos_x_sin_y(double slowdownThreshold, int iterations, std::ostream* outputStreamPtr) {
    double ez_expr_total_time, ez_expr_avg_time;
    double cpp_expr_total_time, cpp_expr_avg_time;

    return Bench(
        "cos(x) * sin(y)",  //
        [](ez::Expr& ev, double t) { return ev.set("x", t).set("y", t + 1.0).eval().getResult(); },
        [](double t) { return std::cos(t) * std::sin(t + 1.0); },
        iterations,
        slowdownThreshold,
        ez_expr_total_time,
        ez_expr_avg_time,
        cpp_expr_total_time,
        cpp_expr_avg_time,
        outputStreamPtr);
}

bool TestEzExpr_Perfos_a_plus_5(double slowdownThreshold, int iterations, std::ostream* outputStreamPtr) {
    double ez_expr_total_time, ez_expr_avg_time;
    double cpp_expr_total_time, cpp_expr_avg_time;

    return Bench(
        "a + 5",  //
        [](ez::Expr& ev, double t) { return ev.set("a", t).eval().getResult(); },
        [](double t) { return t + 5.0; },
        iterations,
        slowdownThreshold,
        ez_expr_total_time,
        ez_expr_avg_time,
        cpp_expr_total_time,
        cpp_expr_avg_time,
        outputStreamPtr);
}

bool TestEzExpr_Perfos_5_plus_a_plus_5(double slowdownThreshold, int iterations, std::ostream* outputStreamPtr) {
    double ez_expr_total_time, ez_expr_avg_time;
    double cpp_expr_total_time, cpp_expr_avg_time;

    return Bench(
        "5 + a + 5",  //
        [](ez::Expr& ev, double t) { return ev.set("a", t).eval().getResult(); },
        [](double t) { return 5.0 + t + 5.0; },
        iterations,
        slowdownThreshold,
        ez_expr_total_time,
        ez_expr_avg_time,
        cpp_expr_total_time,
        cpp_expr_avg_time,
        outputStreamPtr);
}

bool TestEzExpr_Perfos_abs_a_plus_5(double slowdownThreshold, int iterations, std::ostream* outputStreamPtr) {
    double ez_expr_total_time, ez_expr_avg_time;
    double cpp_expr_total_time, cpp_expr_avg_time;

    return Bench(
        "abs(a + 5)",  //
        [](ez::Expr& ev, double t) { return ev.set("a", t).eval().getResult(); },
        [](double t) { return std::abs(t + 5.0); },
        iterations,
        slowdownThreshold,
        ez_expr_total_time,
        ez_expr_avg_time,
        cpp_expr_total_time,
        cpp_expr_avg_time,
        outputStreamPtr);
}

bool TestEzExpr_Perfos_a_plus_5_times_2(double slowdownThreshold, int iterations, std::ostream* outputStreamPtr) {
    double ez_expr_total_time, ez_expr_avg_time;
    double cpp_expr_total_time, cpp_expr_avg_time;

    return Bench(
        "(a + 5) * 2",  //
        [](ez::Expr& ev, double t) { return ev.set("a", t).eval().getResult(); },
        [](double t) { return (t + 5.0) * 2.0; },
        iterations,
        slowdownThreshold,
        ez_expr_total_time,
        ez_expr_avg_time,
        cpp_expr_total_time,
        cpp_expr_avg_time,
        outputStreamPtr);
}

bool TestEzExpr_Perfos_a_plus_5_times_2_alt(double slowdownThreshold, int iterations, std::ostream* outputStreamPtr) {
    double ez_expr_total_time, ez_expr_avg_time;
    double cpp_expr_total_time, cpp_expr_avg_time;

    return Bench(
        "a + (5 * 2)",  //
        [](ez::Expr& ev, double t) { return ev.set("a", t).eval().getResult(); },
        [](double t) { return t + (5.0 * 2.0); },
        iterations,
        slowdownThreshold,
        ez_expr_total_time,
        ez_expr_avg_time,
        cpp_expr_total_time,
        cpp_expr_avg_time,
        outputStreamPtr);
}

bool TestEzExpr_Perfos_sqrt_pow_a(double slowdownThreshold, int iterations, std::ostream* outputStreamPtr) {
    double ez_expr_total_time, ez_expr_avg_time;
    double cpp_expr_total_time, cpp_expr_avg_time;

    return Bench(
        "sqrt(pow(a, 1.5) + pow(a, 2.5))",  //
        [](ez::Expr& ev, double t) { return ev.set("a", t).eval().getResult(); },
        [](double t) { return std::sqrt(std::pow(t, 1.5) + std::pow(t, 2.5)); },
        iterations,
        slowdownThreshold,
        ez_expr_total_time,
        ez_expr_avg_time,
        cpp_expr_total_time,
        cpp_expr_avg_time,
        outputStreamPtr);
}

bool TestEzExpr_Perfos_complex_fraction(double slowdownThreshold, int iterations, std::ostream* outputStreamPtr) {
    double ez_expr_total_time, ez_expr_avg_time;
    double cpp_expr_total_time, cpp_expr_avg_time;

    return Bench(
        "(1/(a + 1) + 2/(a + 2) + 3/(a + 3))",  //
        [](ez::Expr& ev, double t) { return ev.set("a", t).eval().getResult(); },
        [](double t) { return (1.0 / (t + 1.0)) + (2.0 / (t + 2.0)) + (3.0 / (t + 3.0)); },
        iterations,
        slowdownThreshold,
        ez_expr_total_time,
        ez_expr_avg_time,
        cpp_expr_total_time,
        cpp_expr_avg_time,
        outputStreamPtr);
}

bool TestEzExpr_Perfos_All(double slowdownThreshold, int iterations) {
    bool res = true;

    // Ouverture d'un fichier pour enregistrer les résultats
    std::ofstream resultsFile(RESULTS_PATH "benchmark_results.md");

    if (resultsFile.is_open()) {
        // Imprimer l'en-tête du tableau
        resultsFile << "| Expression | EzExpr Total Time (ms) | C++ Total Time (ms) | Slowdown (%) |\n";
        resultsFile << "|------------|------------------|-----------------|----------------|--------------|---------------------|-------------------|\n";

        // Exécution des tests
        res &= TestEzExpr_Perfos_x_squared(slowdownThreshold, iterations, &resultsFile);
        res &= TestEzExpr_Perfos_x_cubed(slowdownThreshold, iterations, &resultsFile);
        res &= TestEzExpr_Perfos_sin_x(slowdownThreshold, iterations, &resultsFile);
        res &= TestEzExpr_Perfos_sqrt_x(slowdownThreshold, iterations, &resultsFile);
        res &= TestEzExpr_Perfos_cos_x_sin_y(slowdownThreshold, iterations, &resultsFile);
        res &= TestEzExpr_Perfos_a_plus_5(slowdownThreshold, iterations, &resultsFile);
        res &= TestEzExpr_Perfos_5_plus_a_plus_5(slowdownThreshold, iterations, &resultsFile);
        res &= TestEzExpr_Perfos_abs_a_plus_5(slowdownThreshold, iterations, &resultsFile);
        res &= TestEzExpr_Perfos_a_plus_5_times_2(slowdownThreshold, iterations, &resultsFile);
        res &= TestEzExpr_Perfos_a_plus_5_times_2_alt(slowdownThreshold, iterations, &resultsFile);
        res &= TestEzExpr_Perfos_sqrt_pow_a(slowdownThreshold, iterations, &resultsFile);
        res &= TestEzExpr_Perfos_complex_fraction(slowdownThreshold, iterations, &resultsFile);

        resultsFile.close();  // Fermer le fichier après les tests
    } else {
        std::cerr << "Failed to open results file." << std::endl;
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////
//// ENTRY POINT ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v(slowdownThreshold, iterations)

#define IfTestBenchExist(v)       \
    if (vTest == std::string(#v)) \
    return v(slowdownThreshold, iterations, nullptr)

bool TestEzExpr_Perfos(const std::string& vTest) {
    double slowdownThreshold = 5000.0;  // 50% de ralentissement maximum acceptable
    int iterations = 1000;
    // Parsing
    IfTestExist(TestEzExpr_Perfos_All);
    else IfTestBenchExist(TestEzExpr_Perfos_x_squared);
    else IfTestBenchExist(TestEzExpr_Perfos_x_cubed);
    else IfTestBenchExist(TestEzExpr_Perfos_sin_x);
    else IfTestBenchExist(TestEzExpr_Perfos_sqrt_x);
    else IfTestBenchExist(TestEzExpr_Perfos_cos_x_sin_y);
    else IfTestBenchExist(TestEzExpr_Perfos_a_plus_5);
    else IfTestBenchExist(TestEzExpr_Perfos_5_plus_a_plus_5);
    else IfTestBenchExist(TestEzExpr_Perfos_abs_a_plus_5);
    else IfTestBenchExist(TestEzExpr_Perfos_a_plus_5_times_2);
    else IfTestBenchExist(TestEzExpr_Perfos_a_plus_5_times_2_alt);
    else IfTestBenchExist(TestEzExpr_Perfos_sqrt_pow_a);
    else IfTestBenchExist(TestEzExpr_Perfos_complex_fraction);
    // default
    return false;
}
