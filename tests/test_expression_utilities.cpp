#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "Core/Exceptions.hpp"
#include "Expression/AST.hpp"
#include "Expression/Expression.hpp"
#include "Expression/ExpressionUtilities.hpp"
#include "Parser/BooleanExpressionParser.hpp"

using namespace BooleanEngine;

namespace
{

template <typename Callable>
void assertThrowsParser(Callable callable)
{
    bool threw = false;

    try
    {
        callable();
    }
    catch (const ParserException& exception)
    {
        threw = true;
        assert(!std::string(exception.what()).empty());
    }

    assert(threw);
}

std::vector<std::string> variablesFromSource(const std::string& input)
{
    const BooleanExpressionParser parser;
    const ParsedBooleanExpression expression = parser.parse(input);
    return collectVariables(expression);
}

void assertVariables(const std::string& input, const std::vector<std::string>& expected)
{
    assert(variablesFromSource(input) == expected);
}

void testSingleVariable()
{
    assertVariables("A", {"A"});
}

void testDuplicateVariable()
{
    assertVariables("A & A", {"A"});
}

void testTwoVariables()
{
    assertVariables("A & B", {"A", "B"});
}

void testSortedDeterministicOrder()
{
    assertVariables("B | A", {"A", "B"});
}

void testUnaryExpressions()
{
    assertVariables("!A", {"A"});
    assertVariables("A'", {"A"});
}

void testNestedExpression()
{
    assertVariables("!(A & B) | C", {"A", "B", "C"});
}

void testComplexRepeatedVariables()
{
    assertVariables("C | A & B | A | !C", {"A", "B", "C"});
}

void testParentheses()
{
    assertVariables("(B | A) & C", {"A", "B", "C"});
}

void testReusableCollector()
{
    assertVariables("D | B", {"B", "D"});
    assertVariables("A & C", {"A", "C"});
    assertVariables("B | A & B", {"A", "B"});
}

void testDirectExpressionNodeApi()
{
    const auto direct = ExpressionNode::makeBinary(BooleanOperator::And,
                                                   ExpressionNode::makeVariable("enable"),
                                                   ExpressionNode::makeVariable("data_in"));

    assert(collectVariables(*direct) == std::vector<std::string>({"data_in", "enable"}));
}

void testEmptyParsedExpression()
{
    const ParsedBooleanExpression empty;

    assert(empty.empty());
    assertThrowsParser([&empty] { (void)collectVariables(empty); });
}

void testFactoryPreventsMalformedAstNodes()
{
    assertThrowsParser([] { (void)ExpressionNode::makeVariable(""); });
    assertThrowsParser([] {
        (void)ExpressionNode::makeUnary(BooleanOperator::Not, nullptr);
    });
    assertThrowsParser([] {
        (void)ExpressionNode::makeBinary(BooleanOperator::And,
                                         nullptr,
                                         ExpressionNode::makeVariable("A"));
    });
    assertThrowsParser([] {
        (void)ExpressionNode::makeBinary(BooleanOperator::And,
                                         ExpressionNode::makeVariable("A"),
                                         nullptr);
    });
}

} // namespace

int main()
{
    testSingleVariable();
    testDuplicateVariable();
    testTwoVariables();
    testSortedDeterministicOrder();
    testUnaryExpressions();
    testNestedExpression();
    testComplexRepeatedVariables();
    testParentheses();
    testReusableCollector();
    testDirectExpressionNodeApi();
    testEmptyParsedExpression();
    testFactoryPreventsMalformedAstNodes();

    std::cout << "All Expression Utilities tests passed!\n";

    return 0;
}
