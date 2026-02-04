#include <TestEzXmlConfig.h>
#include <ezlibs/ezXmlConfig.hpp>

#include <iostream>
#include <string>
#include <sstream>

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

class TestConfig : public ez::xml::Config {
public:
    int testValue = 0;
    std::string testName;

    virtual ez::xml::Nodes getXmlNodes(const std::string& vUserDatas = "") final {
        ez::xml::Node node("root");
        node.addChild("TestValue").setContent(std::to_string(testValue));
        node.addChild("TestName").setContent(testName);
        return node.getChildren();
    }

    virtual bool setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) final {
        if (vNode.getName() == "TestValue") {
            testValue = vNode.getContent<int>();
        } else if (vNode.getName() == "TestName") {
            testName = vNode.getContent();
        }
        return true;
    }
};

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzXmlConfig_ParsingOK() {
    const auto& doc = u8R"(
<config>
	<!-- Comment 1 -->
    <NumberOneLine>60</NumberOneLine>
    <Tests>
	    <!-- Comment 2 -->
        <Test name="test1" number="50"/>
        <Test name="test2" number="100"/>
        <Test name="test3" number="150"/>
        <Test name="test4" number="200">
            <SubTest name="subTest1" number="250"/>
            <SubTest name="subTest2" number="300"/>
        </Test>
    </Tests>
</config>
)";
    TestConfig config;
    bool result = config.LoadConfigString(doc, "");
    if (!result) {
        return false;
    }
    return true;
}

bool TestEzXmlConfig_SaveLoadString() {
    TestConfig config;
    config.testValue = 42;
    config.testName = "TestName";

    // Save to string
    std::string savedString = config.SaveConfigString("", "config");

    // Check that string is not empty
    if (savedString.empty()) {
        return false;
    }

    // Load back
    TestConfig loadedConfig;
    bool result = loadedConfig.LoadConfigString(savedString, "");
    if (!result) {
        return false;
    }

    // Verify values
    if (loadedConfig.testValue != 42) {
        return false;
    }
    if (loadedConfig.testName != "TestName") {
        return false;
    }

    return true;
}

bool TestEzXmlConfig_SaveLoadFile() {
    TestConfig config;
    config.testValue = 123;
    config.testName = "FileTest";

    // Save to file
    std::string testFile = std::string(RESULTS_PATH) + "test_config.xml";
    bool saveResult = config.SaveConfigFile(testFile, "", "config");
    if (!saveResult) {
        return false;
    }

    // Load back
    TestConfig loadedConfig;
    bool loadResult = loadedConfig.LoadConfigFile(testFile, "");
    if (!loadResult) {
        return false;
    }

    // Verify values
    if (loadedConfig.testValue != 123) {
        return false;
    }
    if (loadedConfig.testName != "FileTest") {
        return false;
    }

    return true;
}

bool TestEzXmlConfig_LoadNonExistentFile() {
    TestConfig config;

    // Try to load a file that doesn't exist - should succeed (no file found is ok)
    bool result = config.LoadConfigFile("nonexistent_file_12345.xml", "");
    if (!result) {
        return false;
    }

    return true;
}

bool TestEzXmlConfig_UserDatas() {
    class UserDataConfig : public ez::xml::Config {
    public:
        std::string receivedUserData;

        virtual ez::xml::Nodes getXmlNodes(const std::string& vUserDatas = "") final {
            ez::xml::Node node("root");
            return node.getChildren();
        }

        virtual bool setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) final {
            receivedUserData = vUserDatas;
            return true;
        }
    };

    UserDataConfig config;
    const std::string testUserData = "TestUserData123";
    const std::string xml = "<config><item/></config>";

    bool result = config.LoadConfigString(xml, testUserData);
    if (!result) {
        return false;
    }

    // Check that user data was passed through
    if (config.receivedUserData != testUserData) {
        return false;
    }

    return true;
}

bool TestEzXmlConfig_RecursiveParsing() {
    class CountingConfig : public ez::xml::Config {
    public:
        int nodeCount = 0;

        virtual ez::xml::Nodes getXmlNodes(const std::string& vUserDatas = "") final {
            ez::xml::Node node("root");
            return node.getChildren();
        }

        virtual bool setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) final {
            nodeCount++;
            return true;  // Continue parsing children
        }
    };

    CountingConfig config;
    const std::string xml = R"(
<config>
    <level1>
        <level2>
            <level3/>
        </level2>
    </level1>
</config>
)";

    bool result = config.LoadConfigString(xml, "");
    if (!result) {
        return false;
    }

    // Should have parsed multiple nodes (config, level1, level2, level3)
    if (config.nodeCount < 3) {
        return false;
    }

    return true;
}

bool TestEzXmlConfig_StopChildParsing() {
    class StopParsingConfig : public ez::xml::Config {
    public:
        int nodeCount = 0;

        virtual ez::xml::Nodes getXmlNodes(const std::string& vUserDatas = "") final {
            ez::xml::Node node("root");
            return node.getChildren();
        }

        virtual bool setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) final {
            nodeCount++;
            if (vNode.getName() == "level1") {
                return false;  // Stop parsing children
            }
            return true;
        }
    };

    StopParsingConfig config;
    const std::string xml = R"(
<config>
    <level1>
        <level2>
            <level3/>
        </level2>
    </level1>
</config>
)";

    bool result = config.LoadConfigString(xml, "");
    if (!result) {
        return false;
    }

    // Should have parsed config and level1, but not level2 and level3
    // 3 mean, root, config, level1
    if (config.nodeCount > 3) {
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzXmlConfig(const std::string& vTest) {
    IfTestExist(TestEzXmlConfig_ParsingOK);
    else IfTestExist(TestEzXmlConfig_SaveLoadString);
    else IfTestExist(TestEzXmlConfig_SaveLoadFile);
    else IfTestExist(TestEzXmlConfig_LoadNonExistentFile);
    else IfTestExist(TestEzXmlConfig_UserDatas);
    else IfTestExist(TestEzXmlConfig_RecursiveParsing);
    else IfTestExist(TestEzXmlConfig_StopChildParsing);
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
