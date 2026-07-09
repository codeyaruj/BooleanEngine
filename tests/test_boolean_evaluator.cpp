#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>

#include "Core/Exceptions.hpp"
#include "Evaluation/BooleanEvaluator.hpp"
#include "Expression/AST.hpp"
#include "Expression/Expression.hpp"
#include "Parser/BooleanExpressionParser.hpp"

using namespace BooleanEngine;

namespace
{

using AssignmentMap = BooleanEvaluator::AssignmentMap;

template <typename Callable>
void assertThrowsEvaluation(Callable callable)
{
    bool threw = false;

    try
    {
        callable();
    }
    catch (const EvaluationException& exception)
    {
        threw = true;
        assert(!std::string(exception.what()).empty());
    }

    assert(threw);
}

ParsedBooleanExpression parseExpression(const std::string& input)
{
    const BooleanExpressionParser parser;
    return parser.parse(input);
}

bool evaluateExpression(const std::string& input, const AssignmentMap& variables)
{
    const BooleanEvaluator evaluator;
    const ParsedBooleanExpression expression = parseExpression(input);
    return evaluator.evaluate(expression, variables);
}

void testSingleVariable()
{
    assert(evaluateExpression("A", {{"A", true}}));
    assert(!evaluateExpression("A", {{"A", false}}));
}

void testPrefixPostfixAndRepeatedNot()
{
    assert(!evaluateExpression("!A", {{"A", true}}));
    assert(evaluateExpression("!A", {{"A", false}}));

    assert(!evaluateExpression("A'", {{"A", true}}));
    assert(evaluateExpression("A'", {{"A", false}}));

    assert(!evaluateExpression("!!!A", {{"A", true}}));
}

void testAnd()
{
    assert(!evaluateExpression("A & B", {{"A", false}, {"B", false}}));
    assert(!evaluateExpression("A & B", {{"A", false}, {"B", true}}));
    assert(!evaluateExpression("A & B", {{"A", true}, {"B", false}}));
    assert(evaluateExpression("A & B", {{"A", true}, {"B", true}}));
}

void testOr()
{
    assert(!evaluateExpression("A | B", {{"A", false}, {"B", false}}));
    assert(evaluateExpression("A | B", {{"A", false}, {"B", true}}));
    assert(evaluateExpression("A | B", {{"A", true}, {"B", false}}));
    assert(evaluateExpression("A | B", {{"A", true}, {"B", true}}));
}

void testPrecedence()
{
    assert(evaluateExpression("A | B & C", {{"A", true}, {"B", false}, {"C", false}}));
    assert(!evaluateExpression("A | B & C", {{"A", false}, {"B", true}, {"C", false}}));
    assert(evaluateExpression("A | B & C", {{"A", false}, {"B", true}, {"C", true}}));
}

void testParentheses()
{
    assert(!evaluateExpression("(A | B) & C", {{"A", true}, {"B", false}, {"C", false}}));
    assert(evaluateExpression("(A | B) & C", {{"A", false}, {"B", true}, {"C", true}}));
}

void testMixedExpression()
{
    assert(evaluateExpression("!A & (B | C)", {{"A", false}, {"B", true}, {"C", false}}));
    assert(evaluateExpression("!A & (B | C)", {{"A", false}, {"B", false}, {"C", true}}));
    assert(!evaluateExpression("!A & (B | C)", {{"A", true}, {"B", true}, {"C", true}}));
    assert(!evaluateExpression("!A & (B | C)", {{"A", false}, {"B", false}, {"C", false}}));
}

void testReusableEvaluator()
{
    const BooleanEvaluator evaluator;
    const ParsedBooleanExpression first = parseExpression("A & B");
    const ParsedBooleanExpression second = parseExpression("A | B");

    assert(evaluator.evaluate(first, {{"A", true}, {"B", true}}));
    assert(!evaluator.evaluate(first, {{"A", true}, {"B", false}}));
    assert(evaluator.evaluate(second, {{"A", false}, {"B", true}}));
}

void testDirectAstEvaluation()
{
    const BooleanEvaluator evaluator;

    const auto direct = ExpressionNode::makeBinary(BooleanOperator::Or,
                                                   ExpressionNode::makeVariable("enable"),
                                                   ExpressionNode::makeVariable("data_in"));
    assert(evaluator.evaluate(*direct, {{"enable", false}, {"data_in", true}}));
    assert(!evaluator.evaluate(*direct, {{"enable", false}, {"data_in", false}}));
}

void testShortCircuiting()
{
    assert(!evaluateExpression("A & B", {{"A", false}}));
    assert(evaluateExpression("A | B", {{"A", true}}));
}

void testMissingAssignments()
{
    const BooleanEvaluator evaluator;
    const ParsedBooleanExpression expression = parseExpression("A & B");

    assertThrowsEvaluation([&evaluator, &expression] {
        (void)evaluator.evaluate(expression, {{"A", true}});
    });

    assertThrowsEvaluation([&evaluator] {
        const ParsedBooleanExpression single = parseExpression("A");
        (void)evaluator.evaluate(single, {});
    });
}

void testEmptyParsedExpression()
{
    const BooleanEvaluator evaluator;
    const ParsedBooleanExpression empty;

    assert(empty.empty());
    assertThrowsEvaluation([&evaluator, &empty] {
        (void)evaluator.evaluate(empty, {});
    });
}

} // namespace

int main()
{
    testSingleVariable();
    testPrefixPostfixAndRepeatedNot();
    testAnd();
    testOr();
    testPrecedence();
    testParentheses();
    testMixedExpression();
    testReusableEvaluator();
    testDirectAstEvaluation();
    testShortCircuiting();
    testMissingAssignments();
    testEmptyParsedExpression();

    std::cout << "All Boolean Evaluator tests passed!\n";

    return 0;
}
