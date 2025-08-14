#include <ezlibs/ezSqLite.hpp>  // ton header parser
#include <ezlibs/ezCTest.hpp>
#include <string>

// Désactivation des warnings de conversion
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4305)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzSqlite_EmptySQL() {
    ez::sqlite::Parser::ParserOptions opts;
    opts.trackAllTokens = false;
    auto parser = ez::sqlite::Parser::create(opts);
    CTEST_ASSERT(parser != nullptr);

    ez::sqlite::Parser::ParseReport report;
    CTEST_ASSERT(parser->parse("", report) == true);
    CTEST_ASSERT(report.ok == true);
    CTEST_ASSERT(report.statements.empty());
    return true;
}

bool TestEzSqlite_SimpleSelect() {
    ez::sqlite::Parser::ParserOptions opts;
    opts.trackAllTokens = true;
    auto parser = ez::sqlite::Parser::create(opts);
    CTEST_ASSERT(parser != nullptr);

    ez::sqlite::Parser::ParseReport report;
    const std::string sql = "SELECT 1;";
    CTEST_ASSERT(parser->parse(sql, report) == true);
    CTEST_ASSERT(report.ok == true);
    CTEST_ASSERT(report.statements.size() == 1);
    CTEST_ASSERT(report.statements[0].kind == ez::sqlite::Parser::StatementKind::Select);
    return true;
}

bool TestEzSqlite_SyntaxError() {
    ez::sqlite::Parser::ParserOptions opts;
    auto parser = ez::sqlite::Parser::create(opts);
    CTEST_ASSERT(parser != nullptr);
    ez::sqlite::Parser::ParseReport report;
    const std::string sql = "SELECT FROM;";
    CTEST_ASSERT(parser->parse(sql, report) == true);
    CTEST_ASSERT(report.ok == false);
    CTEST_ASSERT(!report.errors.empty());
    return true;
}

bool TestEzSqlite_MultiLineError() {
    ez::sqlite::Parser::ParserOptions opts;
    auto parser = ez::sqlite::Parser::create(opts);
    CTEST_ASSERT(parser != nullptr);

    const std::string sql = "CREATE TABLE test (\n"
                            "    id INTEGER PRIMARY KEY,\n"
                            "    name TEXT\n"
                            ");\n"
                            "SELECT id,\n"
                            "       FROM test;\n";  // Erreur volontaire ici (virgule avant FROM)

    ez::sqlite::Parser::ParseReport report;
    CTEST_ASSERT(parser->parse(sql, report) == true);
    CTEST_ASSERT(report.ok == false);
    CTEST_ASSERT(!report.errors.empty());

    // On affiche la première erreur détectée
    const ez::sqlite::Parser::Error& e = report.errors.front();
    std::printf("Erreur détectée: ligne %u, colonne %u (offset %u) : %s\n", e.pos.line, e.pos.column, e.pos.offset, e.message.c_str());
    if (!e.expectedHint.empty()) {
        std::printf("  Hint: %s\n", e.expectedHint.c_str());
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzSqlite(const std::string& vTest) {
    IfTestExist(TestEzSqlite_EmptySQL);
    else IfTestExist(TestEzSqlite_SimpleSelect);
    else IfTestExist(TestEzSqlite_SyntaxError);
    else IfTestExist(TestEzSqlite_MultiLineError);
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
