#include <cassert>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Core/Exceptions.hpp"
#include "Evaluation/TruthTableGenerator.hpp"
#include "Expression/AST.hpp"
#include "Expression/Expression.hpp"
#include "Parser/BooleanExpressionParser.hpp"

using namespace BooleanEngine;

namespace
{

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

GeneratedTruthTable generateFromSource(const std::string& input)
{
    const BooleanExpressionParser parser;
    const ParsedBooleanExpression expression = parser.parse(input);
    const TruthTableGenerator generator;
    return generator.generate(expression);
}

void assertOutputs(const GeneratedTruthTable& table,
                   std::initializer_list<bool> expected)
{
    assert(table.rows.size() == expected.size());

    auto expectedOutput = expected.begin();
    for (const TruthTableRow& row : table.rows)
    {
        assert(row.output == *expectedOutput);
        ++expectedOutput;
    }
}

void assertAssignment(const TruthTableRow& row,
                      std::initializer_list<std::pair<std::string, bool>> expected)
{
    assert(row.assignment.size() == expected.size());

    for (const auto& variable : expected)
    {
        assert(row.assignment.at(variable.first) == variable.second);
    }
}

void testSingleVariable()
{
    const GeneratedTruthTable table = generateFromSource("A");

    assert(table.variables == std::vector<std::string>({"A"}));
    assertOutputs(table, {false, true});
    assertAssignment(table.rows[0], {{"A", false}});
    assertAssignment(table.rows[1], {{"A", true}});
}

void testPrefixAndPostfixNot()
{
    assertOutputs(generateFromSource("!A"), {true, false});
    assertOutputs(generateFromSource("A'"), {true, false});
}

void testAnd()
{
    assertOutputs(generateFromSource("A & B"), {false, false, false, true});
}

void testOr()
{
    assertOutputs(generateFromSource("A | B"), {false, true, true, true});
}

void testPrecedence()
{
    assertOutputs(generateFromSource("A | B & C"),
                  {false, false, false, true, true, true, true, true});
}

void testParentheses()
{
    assertOutputs(generateFromSource("(A | B) & C"),
                  {false, false, false, true, false, true, false, true});
}

void testSortedVariablesAndBinaryRowOrder()
{
    const GeneratedTruthTable table = generateFromSource("B | A");

    assert(table.variables == std::vector<std::string>({"A", "B"}));
    assertAssignment(table.rows[0], {{"A", false}, {"B", false}});
    assertAssignment(table.rows[1], {{"A", false}, {"B", true}});
    assertAssignment(table.rows[2], {{"A", true}, {"B", false}});
    assertAssignment(table.rows[3], {{"A", true}, {"B", true}});
}

void testDuplicateVariables()
{
    const GeneratedTruthTable table = generateFromSource("A & A");

    assert(table.variables == std::vector<std::string>({"A"}));
    assertOutputs(table, {false, true});
}

void testThreeVariableExpression()
{
    const GeneratedTruthTable table = generateFromSource("!A & (B | C)");

    assert(table.variables == std::vector<std::string>({"A", "B", "C"}));
    assertOutputs(table, {false, true, true, true, false, false, false, false});
}

void testReusableGenerator()
{
    const BooleanExpressionParser parser;
    const ParsedBooleanExpression andExpression = parser.parse("A & B");
    const ParsedBooleanExpression orExpression = parser.parse("A | B");
    const TruthTableGenerator generator;

    assertOutputs(generator.generate(andExpression), {false, false, false, true});
    assertOutputs(generator.generate(orExpression), {false, true, true, true});
}

void testDirectExpressionNodeAndMultiCharacterVariables()
{
    const auto root = ExpressionNode::makeBinary(
        BooleanOperator::And,
        ExpressionNode::makeVariable("enable"),
        ExpressionNode::makeVariable("data_in"));
    const TruthTableGenerator generator;
    const GeneratedTruthTable table = generator.generate(*root);

    assert(table.variables == std::vector<std::string>({"data_in", "enable"}));
    assertOutputs(table, {false, false, false, true});
}

void testEmptyParsedExpression()
{
    const ParsedBooleanExpression empty;
    const TruthTableGenerator generator;

    assertThrowsEvaluation([&generator, &empty] { (void)generator.generate(empty); });
}

void testOversizedDomain()
{
    std::unique_ptr<ExpressionNode> root = ExpressionNode::makeVariable("variable_00");

    for (std::size_t index = 1; index <= 16; ++index)
    {
        root = ExpressionNode::makeBinary(
            BooleanOperator::Or,
            std::move(root),
            ExpressionNode::makeVariable("variable_" + std::to_string(index)));
    }

    const TruthTableGenerator generator;
    assertThrowsEvaluation([&generator, &root] { (void)generator.generate(*root); });
}

} // namespace

int main()
{
    testSingleVariable();
    testPrefixAndPostfixNot();
    testAnd();
    testOr();
    testPrecedence();
    testParentheses();
    testSortedVariablesAndBinaryRowOrder();
    testDuplicateVariables();
    testThreeVariableExpression();
    testReusableGenerator();
    testDirectExpressionNodeAndMultiCharacterVariables();
    testEmptyParsedExpression();
    testOversizedDomain();

    std::cout << "All Truth Table Generator tests passed!\n";

    return 0;
}
