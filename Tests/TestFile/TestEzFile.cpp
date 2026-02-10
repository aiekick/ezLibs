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
    CTEST_ASSERT(ez::file::destroyFile(file));
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
    CTEST_ASSERT(ez::file::destroyFile(file));
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
    CTEST_ASSERT(ez::file::destroyFile(dir));  // just in case it's a file
    CTEST_ASSERT(ez::file::destroyDir(dir,false));

    CTEST_ASSERT(!ez::file::isDirectoryExist(dir));
    CTEST_ASSERT(ez::file::createDirectoryIfNotExist(dir));
    CTEST_ASSERT(ez::file::isDirectoryExist(dir));
    CTEST_ASSERT(!ez::file::createDirectoryIfNotExist(dir));
    CTEST_ASSERT(ez::file::destroyDir(dir, false));
    CTEST_ASSERT(ez::file::createDirectoryIfNotExist(dir));

    CTEST_ASSERT(ez::file::destroyDir(dir, false));
    return true;
}

bool TestEzFile_loadFileToString() {
    const std::string file = "test_load.txt";
    const std::string content = "Hello World!\nLine 2";

    // Save and load
    CTEST_ASSERT(ez::file::saveStringToFile(content, file));
    std::string loaded = ez::file::loadFileToString(file);
    CTEST_ASSERT(loaded == content);

    // Test nonexistent file
    std::string nonexist = ez::file::loadFileToString("nonexistent_12345.txt");
    CTEST_ASSERT(nonexist.empty());

    CTEST_ASSERT(ez::file::destroyFile(file));
    return true;
}

bool TestEzFile_loadFileToBin() {
    const std::string file = "test_load.bin";
    std::vector<uint8_t> data = {1, 2, 3, 4, 5, 255, 0};

    // Save and load
    CTEST_ASSERT(ez::file::saveBinToFile(data, file));
    auto loaded = ez::file::loadFileToBin(file);
    CTEST_ASSERT(loaded == data);

    // Test nonexistent file
    auto nonexist = ez::file::loadFileToBin("nonexistent_12345.bin");
    CTEST_ASSERT(nonexist.empty());

    CTEST_ASSERT(ez::file::destroyFile(file));
    return true;
}

bool TestEzFile_isFileExist() {
    const std::string file = "test_exist.txt";

    // File should not exist initially
    CTEST_ASSERT(ez::file::destroyFile(file));
    CTEST_ASSERT(!ez::file::isFileExist(file));

    // Create file and check
    CTEST_ASSERT(ez::file::saveStringToFile("test", file));
    CTEST_ASSERT(ez::file::isFileExist(file));

    // Cleanup
    CTEST_ASSERT(ez::file::destroyFile(file));
    CTEST_ASSERT(!ez::file::isFileExist(file));

    return true;
}

bool TestEzFile_isDirectoryExist() {
    const std::string dir = "test_dir_exist";

    // Directory should not exist initially
    CTEST_ASSERT(ez::file::destroyDir(dir, false));
    CTEST_ASSERT(!ez::file::isDirectoryExist(dir));

    // Create directory and check
    CTEST_ASSERT(ez::file::createDirectoryIfNotExist(dir));
    CTEST_ASSERT(ez::file::isDirectoryExist(dir));

    // Cleanup
    CTEST_ASSERT(ez::file::destroyDir(dir, false));
    CTEST_ASSERT(!ez::file::isDirectoryExist(dir));

    return true;
}

bool TestEzFile_destroyFile() {
    const std::string file = "test_destroy.txt";

    // Create and destroy
    CTEST_ASSERT(ez::file::saveStringToFile("test", file));
    CTEST_ASSERT(ez::file::isFileExist(file));
    CTEST_ASSERT(ez::file::destroyFile(file));
    CTEST_ASSERT(!ez::file::isFileExist(file));

    // Destroying nonexistent file, should not fail
    CTEST_ASSERT(ez::file::destroyFile("nonexistent_12345.txt"));

    return true;
}

bool TestEzFile_destroyDir() {
    const std::string dir = "test_destroy_dir";

    // Create and destroy
    CTEST_ASSERT(ez::file::createDirectoryIfNotExist(dir));
    CTEST_ASSERT(ez::file::isDirectoryExist(dir));
    CTEST_ASSERT(ez::file::destroyDir(dir, false));
    CTEST_ASSERT(!ez::file::isDirectoryExist(dir));

    // Destroying nonexistent dir, should not fail
    CTEST_ASSERT(ez::file::destroyDir("nonexistent_dir_12345", false));

    return true;
}

bool TestEzFile_createPathIfNotExist() {
    const std::string path = "test_dir1/test_dir2/test_dir3";

    // Clean up first
    CTEST_ASSERT(ez::file::destroyDir("test_dir1", true));

    // Create nested path
    CTEST_ASSERT(ez::file::createPathIfNotExist(path));
    CTEST_ASSERT(ez::file::isDirectoryExist(path));

    // Cleanup
    CTEST_ASSERT(ez::file::destroyDir("test_dir1", true));
    CTEST_ASSERT(!ez::file::isDirectoryExist(path));

    return true;
}

bool TestEzFile_composePathEdgeCases() {
    // Test with empty strings
    std::string result = ez::file::composePath("", "file", "txt");
    CTEST_ASSERT(!result.empty());

    // Test with various combinations
    result = ez::file::composePath("dir", "", "txt");
    CTEST_ASSERT(!result.empty());

    result = ez::file::composePath("dir", "file", "");
    CTEST_ASSERT(!result.empty());

    return true;
}

bool TestEzFile_parsePathFileNameEdgeCases() {
    // Test with just filename
    auto info = ez::file::parsePathFileName("file.txt");
    CTEST_ASSERT(info.isOk);
    CTEST_ASSERT(info.name == "file");
    CTEST_ASSERT(info.ext == "txt");

    // Test with no extension
    info = ez::file::parsePathFileName("dir/file");
    CTEST_ASSERT(info.isOk);
    CTEST_ASSERT(info.name == "file");

    // Test with multiple extensions
    info = ez::file::parsePathFileName("dir/file.tar.gz");
    CTEST_ASSERT(info.isOk);
    CTEST_ASSERT(info.name == "file.tar");
    CTEST_ASSERT(info.ext == "gz");

    return true;
}

bool TestEzFile_simplifyFilePathEdgeCases() {
    // Test with no .. in path
    std::string result = ez::file::simplifyFilePath("folder/file.txt");
    CTEST_ASSERT(!result.empty());

    // Test with multiple ..
    result = ez::file::simplifyFilePath("a/b/../c/../d.txt");
    CTEST_ASSERT(result.find("..") == std::string::npos);

    // Test empty path
    result = ez::file::simplifyFilePath("");
    CTEST_ASSERT(result.empty() || result == ".");

    return true;
}

bool TestEzFile_correctSlashTypeEdgeCases() {
    // Test with all forward slashes
    std::string result = ez::file::correctSlashTypeForFilePathName("a/b/c/d.txt");
    CTEST_ASSERT(!result.empty());

    // Test with all backslashes
    result = ez::file::correctSlashTypeForFilePathName("a\\b\\c\\d.txt");
    CTEST_ASSERT(!result.empty());

    // Test empty string
    result = ez::file::correctSlashTypeForFilePathName("");
    CTEST_ASSERT(result.empty());

    return true;
}

bool TestEzFile_saveStringToFileWithTimestamp() {
    const std::string file = "test_timestamp";
    const std::string content = "test";

    // Save with timestamp
    CTEST_ASSERT(ez::file::saveStringToFile(content, file, true));

    // Check that at least one file was created
    // Note: exact filename has timestamp, so we just verify no error occurred

    return true;
}

bool TestEzFile_saveBinToFileWithTimestamp() {
    const std::string file = "test_timestamp_bin";
    std::vector<uint8_t> data = {1, 2, 3};

    // Save with timestamp
    CTEST_ASSERT(ez::file::saveBinToFile(data, file, true));

    // Check that at least one file was created
    // Note: exact filename has timestamp, so we just verify no error occurred

    return true;
}

bool TestEzFile_PathInfos_GetFPNE() {
    ez::file::PathInfos info("mypath", "myfile", "txt");
    CTEST_ASSERT(info.isOk);
    std::string fpne = info.GetFPNE();
    CTEST_ASSERT(fpne.find("myfile") != std::string::npos);
    CTEST_ASSERT(fpne.find(".txt") != std::string::npos);
    return true;
}

bool TestEzFile_PathInfos_GetFPNE_WithPath() {
    ez::file::PathInfos info("oldpath", "file", "txt");
    std::string fpne = info.GetFPNE_WithPath("newpath");
    CTEST_ASSERT(fpne.find("newpath") != std::string::npos);
    CTEST_ASSERT(fpne.find("file") != std::string::npos);
    return true;
}

bool TestEzFile_PathInfos_GetFPNE_WithName() {
    ez::file::PathInfos info("path", "oldname", "txt");
    std::string fpne = info.GetFPNE_WithName("newname");
    CTEST_ASSERT(fpne.find("newname") != std::string::npos);
    CTEST_ASSERT(fpne.find(".txt") != std::string::npos);
    return true;
}

bool TestEzFile_PathInfos_GetFPNE_WithExt() {
    ez::file::PathInfos info("path", "file", "oldext");
    std::string fpne = info.GetFPNE_WithExt("newext");
    CTEST_ASSERT(fpne.find("file") != std::string::npos);
    CTEST_ASSERT(fpne.find(".newext") != std::string::npos);
    return true;
}

bool TestEzFile_PathInfos_GetFPNE_WithPathName() {
    ez::file::PathInfos info("oldpath", "oldname", "txt");
    std::string fpne = info.GetFPNE_WithPathName("newpath", "newname");
    CTEST_ASSERT(fpne.find("newpath") != std::string::npos);
    CTEST_ASSERT(fpne.find("newname") != std::string::npos);
    return true;
}

bool TestEzFile_PathInfos_GetFPNE_WithNameExt() {
    ez::file::PathInfos info("path", "oldname", "oldext");
    std::string fpne = info.GetFPNE_WithNameExt("newname", "newext");
    CTEST_ASSERT(fpne.find("newname") != std::string::npos);
    CTEST_ASSERT(fpne.find(".newext") != std::string::npos);
    return true;
}

bool TestEzFile_PathInfos_ExtensionInName() {
    ez::file::PathInfos info("path", "file.tar", "");
    CTEST_ASSERT(info.ext == "tar");
    CTEST_ASSERT(info.name == "file");
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
    else IfTestExist(TestEzFile_loadFileToString);
    else IfTestExist(TestEzFile_loadFileToBin);
    else IfTestExist(TestEzFile_isFileExist);
    else IfTestExist(TestEzFile_isDirectoryExist);
    else IfTestExist(TestEzFile_destroyFile);
    else IfTestExist(TestEzFile_destroyDir);
    else IfTestExist(TestEzFile_createPathIfNotExist);
    else IfTestExist(TestEzFile_composePathEdgeCases);
    else IfTestExist(TestEzFile_parsePathFileNameEdgeCases);
    else IfTestExist(TestEzFile_simplifyFilePathEdgeCases);
    else IfTestExist(TestEzFile_correctSlashTypeEdgeCases);
    else IfTestExist(TestEzFile_saveStringToFileWithTimestamp);
    else IfTestExist(TestEzFile_saveBinToFileWithTimestamp);
    else IfTestExist(TestEzFile_PathInfos_GetFPNE);
    else IfTestExist(TestEzFile_PathInfos_GetFPNE_WithPath);
    else IfTestExist(TestEzFile_PathInfos_GetFPNE_WithName);
    else IfTestExist(TestEzFile_PathInfos_GetFPNE_WithExt);
    else IfTestExist(TestEzFile_PathInfos_GetFPNE_WithPathName);
    else IfTestExist(TestEzFile_PathInfos_GetFPNE_WithNameExt);
    else IfTestExist(TestEzFile_PathInfos_ExtensionInName);
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
