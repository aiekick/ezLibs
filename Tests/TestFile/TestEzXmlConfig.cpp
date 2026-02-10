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
            testValue = vNode.getContentInt(0);
        } else if (vNode.getName() == "TestName") {
            testName = vNode.getContent();
        }
        return true;
    }
};

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzXmlConfigParsingOK() {
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

bool TestEzXmlConfigSaveLoadString() {
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

bool TestEzXmlConfigSaveLoadFile() {
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

bool TestEzXmlConfigLoadNonExistentFile() {
    TestConfig config;

    // Try to load a file that doesn't exist - should succeed (no file found is ok)
    bool result = config.LoadConfigFile("nonexistent_file_12345.xml", "");
    if (!result) {
        return false;
    }

    return true;
}

bool TestEzXmlConfigParseInvalidXml() {
    TestConfig config;

    // Try to parse invalid XML
    const std::string invalidXml = "<invalid><unclosed>";
    bool result = config.LoadConfigString(invalidXml, "");

    // Should return false for invalid XML
    if (result) {
        return false;
    }

    return true;
}

bool TestEzXmlConfigUserDatas() {
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

bool TestEzXmlConfigRecursiveParsing() {
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

bool TestEzXmlConfigStopChildParsing() {
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
    if (config.nodeCount > 2) {
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
    IfTestExist(TestEzXmlConfigParsingOK);
    else IfTestExist(TestEzXmlConfigSaveLoadString);
    else IfTestExist(TestEzXmlConfigSaveLoadFile);
    else IfTestExist(TestEzXmlConfigLoadNonExistentFile);
    else IfTestExist(TestEzXmlConfigParseInvalidXml);
    else IfTestExist(TestEzXmlConfigUserDatas);
    else IfTestExist(TestEzXmlConfigRecursiveParsing);
    else IfTestExist(TestEzXmlConfigStopChildParsing);
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
