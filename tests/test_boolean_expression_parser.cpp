#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "Core/Exceptions.hpp"
#include "Expression/AST.hpp"
#include "Expression/Expression.hpp"
#include "Parser/BooleanExpressionParser.hpp"
#include "Parser/Token.hpp"
#include "Parser/Tokenizer.hpp"

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

std::string parseAndSerialize(const std::string& input)
{
    const BooleanExpressionParser parser;
    const ParsedBooleanExpression expression = parser.parse(Tokenizer::tokenize(input));

    assert(!expression.empty());
    return serializeExpressionTree(expression.root());
}

void testAstFactoriesAndWrapper()
{
    auto variable = ExpressionNode::makeVariable("A");
    assert(variable->kind() == ExpressionNodeKind::Variable);
    assert(variable->variableName() == "A");
    assert(!variable->operation().has_value());
    assert(variable->left() == nullptr);
    assert(variable->right() == nullptr);

    auto unary = ExpressionNode::makeUnary(BooleanOperator::Not, ExpressionNode::makeVariable("B"));
    assert(unary->kind() == ExpressionNodeKind::UnaryOperation);
    assert(unary->operation().value() == BooleanOperator::Not);
    assert(unary->left() == nullptr);
    assert(unary->right() != nullptr);

    auto binary = ExpressionNode::makeBinary(BooleanOperator::And,
                                             ExpressionNode::makeVariable("A"),
                                             ExpressionNode::makeVariable("B"));
    assert(binary->kind() == ExpressionNodeKind::BinaryOperation);
    assert(binary->operation().value() == BooleanOperator::And);
    assert(binary->left() != nullptr);
    assert(binary->right() != nullptr);

    ParsedBooleanExpression empty;
    assert(empty.empty());
    assertThrowsParser([&empty] { (void)empty.root(); });

    ParsedBooleanExpression expression(std::move(binary));
    assert(!expression.empty());
    assert(serializeExpressionTree(expression.root()) == "(& A B)");

    assertThrowsParser([] { (void)ExpressionNode::makeVariable(""); });
    assertThrowsParser([] {
        (void)ExpressionNode::makeUnary(BooleanOperator::And, ExpressionNode::makeVariable("A"));
    });
    assertThrowsParser([] {
        (void)ExpressionNode::makeBinary(BooleanOperator::Not,
                                         ExpressionNode::makeVariable("A"),
                                         ExpressionNode::makeVariable("B"));
    });
}

void testValidExpressions()
{
    assert(parseAndSerialize("A") == "A");
    assert(parseAndSerialize("(A)") == "A");
    assert(parseAndSerialize("!A") == "(! A)");
    assert(parseAndSerialize("~A") == "(! A)");
    assert(parseAndSerialize("A'") == "(! A)");
    assert(parseAndSerialize("!!!A") == "(! (! (! A)))");
    assert(parseAndSerialize("A & B") == "(& A B)");
    assert(parseAndSerialize("A * B") == "(& A B)");
    assert(parseAndSerialize("A.B") == "(& A B)");
    assert(parseAndSerialize("A | B") == "(| A B)");
    assert(parseAndSerialize("A + B") == "(| A B)");
    assert(parseAndSerialize("A | B & C") == "(| A (& B C))");
    assert(parseAndSerialize("!A & B") == "(& (! A) B)");
    assert(parseAndSerialize("(A | B) & C") == "(& (| A B) C)");
    assert(parseAndSerialize("A | B | C") == "(| (| A B) C)");
    assert(parseAndSerialize("!A & (B | C) | D") == "(| (& (! A) (| B C)) D)");
    assert(parseAndSerialize("(((A)))") == "A");
}

void testInvalidExpressionsFromTokenizerIntegration()
{
    assertThrowsParser([] { (void)parseAndSerialize(""); });
    assertThrowsParser([] { (void)parseAndSerialize("   "); });
    assertThrowsParser([] { (void)parseAndSerialize("& A"); });
    assertThrowsParser([] { (void)parseAndSerialize("| A"); });
    assertThrowsParser([] { (void)parseAndSerialize("A &"); });
    assertThrowsParser([] { (void)parseAndSerialize("A |"); });
    assertThrowsParser([] { (void)parseAndSerialize("!"); });
    assertThrowsParser([] { (void)parseAndSerialize("A B"); });
    assertThrowsParser([] { (void)parseAndSerialize("()"); });
    assertThrowsParser([] { (void)parseAndSerialize("A & | B"); });
    assertThrowsParser([] { (void)parseAndSerialize("(| A)"); });
    assertThrowsParser([] { (void)parseAndSerialize("A ^ B"); });
    assertThrowsParser([] { (void)parseAndSerialize("E"); });
    assertThrowsParser([] { (void)parseAndSerialize("1"); });
    assertThrowsParser([] { (void)parseAndSerialize("Σm(1,3)"); });
    assertThrowsParser([] { (void)parseAndSerialize("(A | B"); });
    assertThrowsParser([] { (void)parseAndSerialize("A | B)"); });
}

void testInvalidManualTokenStreams()
{
    const BooleanExpressionParser parser;

    assertThrowsParser([&parser] {
        (void)parser.parse(std::vector<Token>{
            Token(TokenType::LeftParenthesis, "("),
            Token(TokenType::Variable, "A"),
            Token(TokenType::Or, "|"),
            Token(TokenType::Variable, "B"),
            Token(TokenType::EndOfInput, "")});
    });

    assertThrowsParser([&parser] {
        (void)parser.parse(std::vector<Token>{
            Token(TokenType::Variable, "A"),
            Token(TokenType::Or, "|"),
            Token(TokenType::Variable, "B"),
            Token(TokenType::RightParenthesis, ")"),
            Token(TokenType::EndOfInput, "")});
    });

    assertThrowsParser([&parser] {
        (void)parser.parse(std::vector<Token>{
            Token(TokenType::And, "&"),
            Token(TokenType::Variable, "A"),
            Token(TokenType::EndOfInput, "")});
    });

    assertThrowsParser([&parser] { (void)parser.parse(std::vector<Token>{}); });
}

void testParseStringConvenience()
{
    const BooleanExpressionParser parser;
    const ParsedBooleanExpression expression = parser.parse("A | B & C");

    assert(serializeExpressionTree(expression.root()) == "(| A (& B C))");
}

} // namespace

int main()
{
    testAstFactoriesAndWrapper();
    testValidExpressions();
    testInvalidExpressionsFromTokenizerIntegration();
    testInvalidManualTokenStreams();
    testParseStringConvenience();

    std::cout << "All Boolean Expression Parser tests passed!\n";

    return 0;
}
