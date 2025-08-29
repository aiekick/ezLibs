#include <ezlibs/ezFile.hpp>
#include <ezlibs/ezCTest.hpp>
#include <string>

// Desactivation des warnings de conversion
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)  // Conversion from 'double' to 'float', possible loss of data
#pragma warning(disable : 4305)  // Truncation from 'double' to 'float'
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzFile_saveStringToFile() {
    const std::string file = "test_save.txt";
    const std::string content = "test content";
    CTEST_ASSERT(ez::file::saveStringToFile(content, file));
    CTEST_ASSERT(ez::file::isFileExist(file));
    CTEST_ASSERT(ez::file::loadFileToString(file) == content);
    ez::file::destroyFile(file);
    return true;
}

bool TestEzFile_saveBinToFile() {
    const std::string file = "test_save.bin";
    std::vector<uint8_t> bin = {42, 69, 0, 20, 255};
    bool ok = ez::file::saveBinToFile(bin, file);
    CTEST_ASSERT(ok);
    CTEST_ASSERT(ez::file::isFileExist(file));
    auto out = ez::file::loadFileToBin(file);
    CTEST_ASSERT(out == bin);
    ez::file::destroyFile(file);
    return true;
}

bool TestEzFile_parsePathFileName() {
    auto info = ez::file::parsePathFileName("dir/sub/file.ext");
    CTEST_ASSERT(info.isOk);
#ifdef WINDOWS_OS
    CTEST_ASSERT(info.path == "dir\\sub");
#else
    CTEST_ASSERT(info.path == "dir/sub");
#endif
    CTEST_ASSERT(info.name == "file");
    CTEST_ASSERT(info.ext == "ext");
    return true;
}

bool TestEzFile_simplifyFilePath() {
    std::string input = "folder/TOTO/../file.txt";
    std::string simplified = ez::file::simplifyFilePath(input);
    CTEST_ASSERT(simplified.find("..") == std::string::npos);
#ifdef WINDOWS_OS
    CTEST_ASSERT(simplified == "folder\\file.txt");
#else
    CTEST_ASSERT(simplified == "folder/file.txt");
#endif
    return true;
}

bool TestEzFile_composePath() {
    std::string composed = ez::file::composePath("dir", "name", "txt");
#ifdef WINDOWS_OS
    CTEST_ASSERT(composed == "dir\\name.txt");
#else
    CTEST_ASSERT(composed == "dir/name.txt");
#endif
    return true;
}

bool TestEzFile_correctSlashTypeForFilePathName() {
    std::string input = "a\\b/c\\d.txt";
    std::string corrected = ez::file::correctSlashTypeForFilePathName(input);
#ifdef WINDOWS_OS
    CTEST_ASSERT(corrected == "a\\b\\c\\d.txt");
#else
    CTEST_ASSERT(corrected == "a/b/c/d.txt");
#endif
    return true;
}

bool TestEzFile_createDirectoryIfNotExist() {
    const std::string dir = "test_temp_dir";
    ez::file::destroyFile(dir);  // just in case it's a file
    ez::file::destroyDir(dir);

    CTEST_ASSERT(!ez::file::isDirectoryExist(dir));
    CTEST_ASSERT(ez::file::createDirectoryIfNotExist(dir));
    CTEST_ASSERT(ez::file::isDirectoryExist(dir));
    CTEST_ASSERT(!ez::file::createDirectoryIfNotExist(dir));
    ez::file::destroyDir(dir);
    CTEST_ASSERT(ez::file::createDirectoryIfNotExist(dir));

    ez::file::destroyDir(dir);
    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzFile(const std::string& vTest) {
    IfTestExist(TestEzFile_saveStringToFile);
    else IfTestExist(TestEzFile_saveBinToFile);
    else IfTestExist(TestEzFile_parsePathFileName);
    else IfTestExist(TestEzFile_simplifyFilePath);
    else IfTestExist(TestEzFile_composePath);
    else IfTestExist(TestEzFile_correctSlashTypeForFilePathName);
    else IfTestExist(TestEzFile_createDirectoryIfNotExist);
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
