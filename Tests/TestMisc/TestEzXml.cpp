#include <TestEzXml.h>
#include <ezlibs/ezXml.hpp>

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
    const auto& doc =
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
    const auto& rootChildrens = xml.getRoot().getChildren();
    if (rootChildrens.size() != 1U)
        return false;
    if (rootChildrens[0].getName() != "config")
        return false;
    const auto& configChildrens = rootChildrens[0].getChildren();
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
    const auto& testsChildrens = configChildrens[2].getChildren();
    if (testsChildrens.size() != 5U)
        return false;
    if (testsChildrens[0].getContent() != "<!-- Comment 2 -->")
        return false;
    if (testsChildrens[1].getName() != "Test")
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
    const auto& test4Childrens = testsChildrens[4].getChildren();
    if (test4Childrens.size() != 2U)
        return false;
    if (test4Childrens[0].getAttribute("name") != "subTest1")
        return false;
    if (test4Childrens[0].getAttribute("number") != "250")
        return false;
    if (test4Childrens[1].getAttribute("name") != "subTest2")
        return false;
    if (test4Childrens[1].getAttribute("number") != "300")
        return false;
    return true;
}

// all attributes value must be some strings
bool TestEzXmlParsingNOK_0() {
    const auto& doc =
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
    const auto& doc =
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

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzXml(const std::string& vTest) {
    IfTestExist(TestEzXmlParsingOK);
    else IfTestExist(TestEzXmlParsingNOK_0);
    else IfTestExist(TestEzXmlParsingNOK_1);
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
