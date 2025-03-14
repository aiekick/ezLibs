#include <TestEzXml.h>
#include <ezlibs/ezXml.hpp>
#include <ezlibs/ezCTest.hpp>

#include <iostream>
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

bool TestEzXmlParsingOK() {
    const auto &doc =
            u8R"(
 < config > 
	<!-- Comment 1 -->
    <NumberOneLine>60</NumberOneLine>
    <Tests> 
	    <!-- Comment 2 -->
        <Test name="test1" number="50"/>
        <Test name ="test2" number="100"/>
        <Test name= "test3" number="150"/>
        <Test name = "test4" number="200">
            <SubTest name="subTest1" number="250"/>
            <SubTest name="subTest2" number="300"/>
            <bool>false</bool>
            <bool>true</bool>
        </Test>
    </Tests>
</config>
)";
    ez::Xml xml;
    if (!xml.parseString(doc))
        return false;
    std::cout << xml.dump() << std::endl;
    if (xml.getRoot().getChildren().empty())
        return false;
    const auto &rootChildrens = xml.getRoot().getChildren();
    if (rootChildrens.size() != 1U)
        return false;
    if (rootChildrens[0].getName() != "config")
        return false;
    const auto &configChildrens = rootChildrens[0].getChildren();
    if (configChildrens.size() != 3U)
        return false;
    if (configChildrens[0].getContent() != "<!-- Comment 1 -->")
        return false;
    if (configChildrens[1].getName() != "NumberOneLine")
        return false;
    if (configChildrens[1].getContent() != "60")
        return false;
    if (configChildrens[2].getName() != "Tests")
        return false;
    const auto &testsChildrens = configChildrens[2].getChildren();
    if (testsChildrens.size() != 5U)
        return false;
    if (testsChildrens[0].getContent() != "<!-- Comment 2 -->")
        return false;
    if (testsChildrens[1].getName() != "Test")
        return false;
    if (testsChildrens[1].getParentNodeName() != "Tests")
        return false;
    if (!testsChildrens[1].isAttributeExist("name"))
        return false;
    if (testsChildrens[1].getAttribute("name") != "test1")
        return false;
    if (testsChildrens[1].getAttribute("number") != "50")
        return false;
    if (testsChildrens[2].getAttribute("name") != "test2")
        return false;
    if (testsChildrens[2].getAttribute("number") != "100")
        return false;
    if (testsChildrens[3].getAttribute("name") != "test3")
        return false;
    if (testsChildrens[3].getAttribute("number") != "150")
        return false;
    if (testsChildrens[4].getAttribute("name") != "test4")
        return false;
    if (testsChildrens[4].getAttribute("number") != "200")
        return false;
    const auto &test4Childrens = testsChildrens[4].getChildren();
    if (test4Childrens.size() != 4U)
        return false;
    if (test4Childrens[0].getName() != "SubTest")
        return false;
    if (test4Childrens[0].getAttribute("name") != "subTest1")
        return false;
    if (test4Childrens[0].getAttribute("number") != "250")
        return false;
    if (test4Childrens[1].getName() != "SubTest")
        return false;
    if (test4Childrens[1].getAttribute("name") != "subTest2")
        return false;
    if (test4Childrens[1].getAttribute<int32_t>("number") != 300)
        return false;
    if (test4Childrens[2].getName() != "bool")
        return false;
    if (test4Childrens[2].getContent<bool>() != false)
        return false;
    if (test4Childrens[3].getName() != "bool")
        return false;
    if (test4Childrens[3].getContent<bool>() != true)
        return false;
    return true;
}

// all attributes value must be some strings
bool TestEzXmlParsingNOK_0() {
    const auto &doc =
            u8R"(
<config>
    <Test name="test1" number=5/>
</config>
)";
    ez::Xml xml;
    if (xml.parseString(doc))
        return false;
    std::cout << xml.dump() << std::endl;
    return true;
}

// to tag end
bool TestEzXmlParsingNOK_1() {
    const auto &doc =
            u8R"(
<config>
<config>
)";
    ez::Xml xml;
    if (!xml.parseString(doc))
        return false;
    std::cout << xml.dump() << std::endl;
    return true;
}

bool TestEzXmlWriting_1() {
    ez::Xml xml("test");
    auto &rootNode = xml.getRoot();
    rootNode.setName("config");
    rootNode.addComment("Comment 1");
    rootNode.addChild("NumberOneLine").setContent(60);
    auto &testsNode = rootNode.addChild("Tests");
    testsNode.addComment("Comment 2");
    testsNode.addChild("Test").addAttribute("name", "test1").addAttribute("number") << 50;
    testsNode.addChild("Test").addAttribute("name", "test2").addAttribute("number") << 100;
    testsNode.addChild("Test").addAttribute("name", "test3").addAttribute("number") << 150;
    auto &subNode = testsNode.addChild("Test");
    subNode.addAttribute("name", "test4").addAttribute("number") << 200;
    subNode.addChild("SubTest").addAttribute("name", "subTest1").addAttribute("number") << 250;
    subNode.addChild("SubTest").addAttribute("name", "subTest2").addAttribute("number") << 300;

    const auto result = xml.dump();
    const auto expected = u8R"(<config>
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

    CTEST_ASSERT(result == expected);
    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzXml(const std::string &vTest) {
    IfTestExist(TestEzXmlParsingOK);
    else IfTestExist(TestEzXmlParsingNOK_0);
    else IfTestExist(TestEzXmlParsingNOK_1);
    else IfTestExist(TestEzXmlWriting_1);
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
