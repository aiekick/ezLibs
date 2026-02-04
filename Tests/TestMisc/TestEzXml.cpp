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

bool TestEzXml_ParsingOK() {
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
        <Test/>
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
    if (testsChildrens.size() != 6U)
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
    if (testsChildrens[5].getName() != "Test")
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
bool TestEzXml_ParsingNOK_0() {
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
bool TestEzXml_ParsingNOK_1() {
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

bool TestEzXml_Writing_1() {
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

bool TestEzXml_EscapeUnescapeXml() {
    std::string input = "<tag>Value&\"'</tag>";
    std::string escaped = ez::xml::Node::escapeXml(input);
    CTEST_ASSERT(escaped.find("&lt;") != std::string::npos);
    CTEST_ASSERT(escaped.find("&gt;") != std::string::npos);
    CTEST_ASSERT(escaped.find("&amp;") != std::string::npos);
    CTEST_ASSERT(escaped.find("&quot;") != std::string::npos);
    CTEST_ASSERT(escaped.find("&apos;") != std::string::npos);

    std::string unescaped = ez::xml::Node::unEscapeXml(escaped);
    CTEST_ASSERT(unescaped == input);
    return true;
}

bool TestEzXml_NodeOperations() {
    ez::xml::Node node("testNode");
    node.setContent("TestContent");
    CTEST_ASSERT(node.getName() == "testNode");
    CTEST_ASSERT(node.getContent() == "TestContent");

    node.addAttribute("attr1", "value1");
    CTEST_ASSERT(node.isAttributeExist("attr1"));
    CTEST_ASSERT(node.getAttribute("attr1") == "value1");
    CTEST_ASSERT(!node.isAttributeExist("nonexistent"));

    return true;
}

bool TestEzXml_GetOrAddChild() {
    ez::xml::Node parent("parent");
    auto& child1 = parent.addChild("child");
    auto& child2 = parent.getOrAddChild("child");

    // Should return the existing child
    CTEST_ASSERT(&child1 == &child2);

    // Should create a new child
    auto& newChild = parent.getOrAddChild("newChild");
    CTEST_ASSERT(newChild.getName() == "newChild");

    return true;
}

bool TestEzXml_GetChildNull() {
    ez::xml::Node parent("parent");
    parent.addChild("child1");

    auto* found = parent.getChild("child1");
    CTEST_ASSERT(found != nullptr);

    auto* notFound = parent.getChild("nonexistent");
    CTEST_ASSERT(notFound == nullptr);

    return true;
}

bool TestEzXml_AddChilds() {
    ez::xml::Node parent("parent");
    ez::xml::Nodes children;
    children.push_back(ez::xml::Node("child1"));
    children.push_back(ez::xml::Node("child2"));
    children.push_back(ez::xml::Node("child3"));

    parent.addChilds(children);
    CTEST_ASSERT(parent.getChildren().size() == 3);

    return true;
}

bool TestEzXml_AttributeWithTemplateTypes() {
    ez::xml::Node node("test");
    node.addAttribute("intAttr", 42);
    node.addAttribute("floatAttr", 3.14f);
    node.addAttribute("doubleAttr", 2.718);

    CTEST_ASSERT(node.getAttribute<int>("intAttr") == 42);
    float f = node.getAttribute<float>("floatAttr");
    CTEST_ASSERT(f > 3.13f && f < 3.15f);

    return true;
}

bool TestEzXml_ReplaceAll() {
    std::string str = "hello world hello";
    ez::xml::Node::replaceAll(str, "hello", "hi");
    CTEST_ASSERT(str == "hi world hi");

    // Test with empty string
    std::string str2 = "test";
    ez::xml::Node::replaceAll(str2, "", "x");
    CTEST_ASSERT(str2 == "test");

    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzXml(const std::string &vTest) {
    IfTestExist(TestEzXml_ParsingOK);
    else IfTestExist(TestEzXml_ParsingNOK_0);
    else IfTestExist(TestEzXml_ParsingNOK_1);
    else IfTestExist(TestEzXml_Writing_1);
    else IfTestExist(TestEzXml_EscapeUnescapeXml);
    else IfTestExist(TestEzXml_NodeOperations);
    else IfTestExist(TestEzXml_GetOrAddChild);
    else IfTestExist(TestEzXml_GetChildNull);
    else IfTestExist(TestEzXml_AddChilds);
    else IfTestExist(TestEzXml_AttributeWithTemplateTypes);
    else IfTestExist(TestEzXml_ReplaceAll);
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
