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
    ez::sqlite::Parser::Options opts;
    opts.trackAllTokens = false;
    auto parser = ez::sqlite::Parser(opts);
    ez::sqlite::Parser::Report report;
    CTEST_ASSERT(parser.parse("", report) == true);
    CTEST_ASSERT(report.ok == true);
    CTEST_ASSERT(report.statements.empty());
    return true;
}

bool TestEzSqlite_SimpleSelect() {
    ez::sqlite::Parser::Options opts;
    opts.trackAllTokens = true;
    auto parser = ez::sqlite::Parser(opts);
    ez::sqlite::Parser::Report report;
    const std::string sql = "SELECT 1;";
    CTEST_ASSERT(parser.parse(sql, report) == true);
    CTEST_ASSERT(report.ok == true);
    CTEST_ASSERT(report.statements.size() == 1);
    CTEST_ASSERT(report.statements[0].kind == ez::sqlite::Parser::StatementKind::Select);
    return true;
}

bool TestEzSqlite_SyntaxError_SelectFrom() {
    ez::sqlite::Parser::Options opts;
    auto parser = ez::sqlite::Parser(opts);
    ez::sqlite::Parser::Report report;
    const std::string sql = "SELECT FROM;";
    CTEST_ASSERT(parser.parse(sql, report) == true);
    CTEST_ASSERT(report.ok == false);
    CTEST_ASSERT(!report.errors.empty());
    return true;
}

bool TestEzSqlite_SyntaxError_CreateTableMissingParenOrAs() {
    ez::sqlite::Parser::Options opts;
    auto parser = ez::sqlite::Parser(opts);
    ez::sqlite::Parser::Report report;
    // manque '(' ou 'AS' après le nom de table
    const std::string sql = "CREATE TABLE t name_only;";
    CTEST_ASSERT(parser.parse(sql, report) == true);
    CTEST_ASSERT(report.ok == false);
    CTEST_ASSERT(!report.errors.empty());
    return true;
}

bool TestEzSqlite_SyntaxError_InsertMissingInto() {
    ez::sqlite::Parser::Options opts;
    auto parser = ez::sqlite::Parser(opts);
    ez::sqlite::Parser::Report report;
    const std::string sql = "INSERT t VALUES (1);";
    CTEST_ASSERT(parser.parse(sql, report) == true);
    CTEST_ASSERT(report.ok == false);
    CTEST_ASSERT(!report.errors.empty());
    return true;
}

bool TestEzSqlite_SyntaxError_InsertValuesNoParens() {
    ez::sqlite::Parser::Options opts;
    auto parser = ez::sqlite::Parser(opts);
    ez::sqlite::Parser::Report report;
    // VALUES sans liste parenthésée
    const std::string sql = "INSERT INTO t VALUES 1, 2, 3;";
    CTEST_ASSERT(parser.parse(sql, report) == true);
    CTEST_ASSERT(report.ok == false);
    CTEST_ASSERT(!report.errors.empty());
    return true;
}

bool TestEzSqlite_SyntaxError_UpdateNoSet() {
    ez::sqlite::Parser::Options opts;
    auto parser = ez::sqlite::Parser(opts);
    ez::sqlite::Parser::Report report;
    const std::string sql = "UPDATE t WHERE id=1;";
    CTEST_ASSERT(parser.parse(sql, report) == true);
    CTEST_ASSERT(report.ok == false);
    CTEST_ASSERT(!report.errors.empty());
    return true;
}

bool TestEzSqlite_SyntaxError_DeleteNoFrom() {
    ez::sqlite::Parser::Options opts;
    auto parser = ez::sqlite::Parser(opts);
    ez::sqlite::Parser::Report report;
    const std::string sql = "DELETE t;";
    CTEST_ASSERT(parser.parse(sql, report) == true);
    CTEST_ASSERT(report.ok == false);
    CTEST_ASSERT(!report.errors.empty());
    return true;
}

bool TestEzSqlite_SyntaxError_OrderWithoutBy() {
    ez::sqlite::Parser::Options opts;
    auto parser = ez::sqlite::Parser(opts);
    ez::sqlite::Parser::Report report;
    const std::string sql = "SELECT 1 FROM x ORDER;";
    CTEST_ASSERT(parser.parse(sql, report) == true);
    CTEST_ASSERT(report.ok == false);
    CTEST_ASSERT(!report.errors.empty());
    return true;
}

bool TestEzSqlite_SyntaxError_LimitNoValue() {
    ez::sqlite::Parser::Options opts;
    auto parser = ez::sqlite::Parser(opts);
    ez::sqlite::Parser::Report report;
    const std::string sql = "SELECT 1 FROM x LIMIT;";
    CTEST_ASSERT(parser.parse(sql, report) == true);
    CTEST_ASSERT(report.ok == false);
    CTEST_ASSERT(!report.errors.empty());
    return true;
}

bool TestEzSqlite_SyntaxError_ParensUnbalancedOpen() {
    ez::sqlite::Parser::Options opts;
    auto parser = ez::sqlite::Parser(opts);
    ez::sqlite::Parser::Report report;
    const std::string sql = "SELECT (1 FROM x;";
    CTEST_ASSERT(parser.parse(sql, report) == true);
    CTEST_ASSERT(report.ok == false);
    CTEST_ASSERT(!report.errors.empty());
    return true;
}

bool TestEzSqlite_SyntaxError_ParensUnbalancedClose() {
    ez::sqlite::Parser::Options opts;
    auto parser = ez::sqlite::Parser(opts);
    ez::sqlite::Parser::Report report;
    const std::string sql = "SELECT 1) FROM x;";
    CTEST_ASSERT(parser.parse(sql, report) == true);
    CTEST_ASSERT(report.ok == false);
    CTEST_ASSERT(!report.errors.empty());
    return true;
}

bool TestEzSqlite_SyntaxError_SelectTrailingCommaBeforeFrom() {
    ez::sqlite::Parser::Options opts;
    auto parser = ez::sqlite::Parser(opts);
    ez::sqlite::Parser::Report report;
    const std::string sql = "SELECT id, \n"
                            "FROM t;";
    CTEST_ASSERT(parser.parse(sql, report) == true);
    CTEST_ASSERT(report.ok == false);
    CTEST_ASSERT(!report.errors.empty());
    return true;
}

// multi-ligne + impression de l’erreur principale (utile pour vérifier la localisation)
bool TestEzSqlite_SyntaxError_MultiLine_ShowLocation() {
    ez::sqlite::Parser::Options opts;
    auto parser = ez::sqlite::Parser(opts);
    const std::string sql = "CREATE TABLE test (\n"
                            "    id INTEGER PRIMARY KEY,\n"
                            "    name TEXT\n"
                            ");\n"
                            "SELECT id,\n"
                            "       FROM test;\n";  // virgule orpheline avant FROM
    ez::sqlite::Parser::Report report;
    CTEST_ASSERT(parser.parse(sql, report) == true);
    CTEST_ASSERT(report.ok == false);
    CTEST_ASSERT(!report.errors.empty());
    CTEST_ASSERT(report.errors.size() == 1U);
    const auto& err = report.errors.at(0);
    CTEST_ASSERT(err.pos.line == 6);
    CTEST_ASSERT(err.pos.column == 8);
    CTEST_ASSERT(err.pos.offset == 83);
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
    else IfTestExist(TestEzSqlite_SyntaxError_SelectFrom);
    else IfTestExist(TestEzSqlite_SyntaxError_CreateTableMissingParenOrAs);
    else IfTestExist(TestEzSqlite_SyntaxError_InsertMissingInto);
    else IfTestExist(TestEzSqlite_SyntaxError_InsertValuesNoParens);
    else IfTestExist(TestEzSqlite_SyntaxError_UpdateNoSet);
    else IfTestExist(TestEzSqlite_SyntaxError_DeleteNoFrom);
    else IfTestExist(TestEzSqlite_SyntaxError_OrderWithoutBy);
    else IfTestExist(TestEzSqlite_SyntaxError_LimitNoValue);
    else IfTestExist(TestEzSqlite_SyntaxError_ParensUnbalancedOpen);
    else IfTestExist(TestEzSqlite_SyntaxError_ParensUnbalancedClose);
    else IfTestExist(TestEzSqlite_SyntaxError_SelectTrailingCommaBeforeFrom);
    else IfTestExist(TestEzSqlite_SyntaxError_MultiLine_ShowLocation);
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
