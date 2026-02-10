#include <ezlibs/ezSqlite.hpp>  // ton header parser
#include <ezlibs/ezCTest.hpp>
#include <string>

// D�sactivation des warnings de conversion
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
    // manque '(' ou 'AS' apr�s le nom de table
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
    // VALUES sans liste parenth�s�e
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

// multi-ligne + impression de l�erreur principale (utile pour v�rifier la localisation)
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

bool TestEzSqlite_QueryBuilder_InsertQuery() {
    ez::sqlite::QueryBuilder qb;
    qb.setTable("users")
      .addOrSetField("name", "John")
      .addOrSetField("age", 30)
      .addOrSetField("email", "john@test.com");

    std::string query = qb.build(ez::sqlite::QueryType::INSERT);
    CTEST_ASSERT(!query.empty());
    CTEST_ASSERT(query.find("INSERT INTO users") != std::string::npos);
    CTEST_ASSERT(query.find("name") != std::string::npos);
    CTEST_ASSERT(query.find("age") != std::string::npos);
    CTEST_ASSERT(query.find("email") != std::string::npos);
    return true;
}

bool TestEzSqlite_QueryBuilder_UpdateQuery() {
    ez::sqlite::QueryBuilder qb;
    qb.setTable("users")
      .addOrSetField("name", "Jane")
      .addOrSetField("age", 25)
      .addWhere("id = 1");

    std::string query = qb.build(ez::sqlite::QueryType::UPDATE);
    CTEST_ASSERT(!query.empty());
    CTEST_ASSERT(query.find("UPDATE users") != std::string::npos);
    CTEST_ASSERT(query.find("SET") != std::string::npos);
    CTEST_ASSERT(query.find("WHERE") != std::string::npos);
    return true;
}

bool TestEzSqlite_QueryBuilder_InsertIfNotExist() {
    ez::sqlite::QueryBuilder qb;
    qb.setTable("config")
      .addOrSetField("key", "theme")
      .addOrSetField("value", "dark");

    std::string query = qb.build(ez::sqlite::QueryType::INSERT_IF_NOT_EXIST);
    CTEST_ASSERT(!query.empty());
    CTEST_ASSERT(query.find("SELECT") != std::string::npos);
    CTEST_ASSERT(query.find("WHERE NOT EXISTS") != std::string::npos);
    return true;
}

bool TestEzSqlite_QueryBuilder_WithSubQuery() {
    ez::sqlite::QueryBuilder qb;
    qb.setTable("posts")
      .addOrSetField("title", "Test")
      .addOrSetFieldQuery("user_id", "SELECT id FROM users WHERE name='John'");

    std::string query = qb.build(ez::sqlite::QueryType::INSERT);
    CTEST_ASSERT(!query.empty());
    CTEST_ASSERT(query.find("SELECT id FROM users") != std::string::npos);
    return true;
}

bool TestEzSqlite_QueryBuilder_MultipleWhereConditions() {
    ez::sqlite::QueryBuilder qb;
    qb.setTable("users")
      .addOrSetField("status", "active")
      .addWhere("age > 18")
      .addWhere("country = 'US'");

    std::string query = qb.build(ez::sqlite::QueryType::UPDATE);
    CTEST_ASSERT(!query.empty());
    CTEST_ASSERT(query.find("WHERE") != std::string::npos);
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
    else IfTestExist(TestEzSqlite_QueryBuilder_InsertQuery);
    else IfTestExist(TestEzSqlite_QueryBuilder_UpdateQuery);
    else IfTestExist(TestEzSqlite_QueryBuilder_InsertIfNotExist);
    else IfTestExist(TestEzSqlite_QueryBuilder_WithSubQuery);
    else IfTestExist(TestEzSqlite_QueryBuilder_MultipleWhereConditions);
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
