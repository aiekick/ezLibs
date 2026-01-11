#include <TestEzXmlConfig.h>
#include <ezlibs/ezXmlConfig.hpp>

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

class TestConfig : public ez::xml::Config {
public:
    virtual ez::xml::Nodes getXmlNodes(const std::string& vUserDatas = "") final {
        ez::xml::Node node("root");
        return node.getChildren();
    }
    // return true for continue xml parsing of childs in this node or false for interrupt the child exploration (if we want explore child ourselves)
    virtual bool setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) final { return true; }
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
    // todo
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
