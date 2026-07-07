#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "Core/Exceptions.hpp"
#include "Parser/Parser.hpp"
#include "Parser/ParserUtilities.hpp"
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
    catch (const ParserException&)
    {
        threw = true;
    }

    assert(threw);
}

std::vector<TokenType> tokenTypes(const std::vector<Token>& tokens)
{
    std::vector<TokenType> types;
    types.reserve(tokens.size());

    for (const Token& token : tokens)
    {
        types.push_back(token.type());
    }

    return types;
}

class DummyParser final : public Parser
{
public:
    BooleanFunction parse(const std::string& input) override
    {
        validate(input);
        parsed_ = true;

        BooleanFunction function;
        function.variableCount =
            static_cast<int>(ParserUtilities::countUniqueVariables(Tokenizer::tokenize(input)));
        return function;
    }

    void validate(const std::string& input) const override
    {
        (void)Tokenizer::tokenize(input);
    }

    void reset() override
    {
        parsed_ = false;
    }

    [[nodiscard]] bool parsed() const noexcept
    {
        return parsed_;
    }

private:
    bool parsed_ = false;
};

void testToken()
{
    const Token defaultToken;
    assert(defaultToken.type() == TokenType::EndOfInput);
    assert(defaultToken.lexeme().empty());
    assert(!defaultToken.hasNumericValue());

    const Token number(TokenType::DecimalNumber, "42", 42);
    assert(number.type() == TokenType::DecimalNumber);
    assert(number.lexeme() == "42");
    assert(number.hasNumericValue());
    assert(number.numericValue().value() == 42);
}

void testTokenizerVariablesAndOperators()
{
    const auto tokens = Tokenizer::tokenize("AB + A'C");
    const std::vector<TokenType> expected{
        TokenType::Variable,
        TokenType::Variable,
        TokenType::Or,
        TokenType::Variable,
        TokenType::Not,
        TokenType::Variable,
        TokenType::EndOfInput};

    assert(tokenTypes(tokens) == expected);
    assert(tokens[0].lexeme() == "A");
    assert(tokens[1].lexeme() == "B");
    assert(tokens[2].lexeme() == "+");
    assert(tokens[4].lexeme() == "'");
}

void testTokenizerParenthesesAndWhitespace()
{
    const Tokenizer tokenizer("  (A+B')C  ");
    const auto tokens = tokenizer.tokenize();
    const std::vector<TokenType> expected{
        TokenType::LeftParenthesis,
        TokenType::Variable,
        TokenType::Or,
        TokenType::Variable,
        TokenType::Not,
        TokenType::RightParenthesis,
        TokenType::Variable,
        TokenType::EndOfInput};

    assert(tokenTypes(tokens) == expected);
}

void testTokenizerNumbersAndSigmaNotation()
{
    const auto tokens = Tokenizer::tokenize("Σm(1,3,5)d(2,7)");
    const std::vector<TokenType> expected{
        TokenType::Sigma,
        TokenType::Variable,
        TokenType::LeftParenthesis,
        TokenType::DecimalNumber,
        TokenType::Comma,
        TokenType::DecimalNumber,
        TokenType::Comma,
        TokenType::DecimalNumber,
        TokenType::RightParenthesis,
        TokenType::Variable,
        TokenType::LeftParenthesis,
        TokenType::DecimalNumber,
        TokenType::Comma,
        TokenType::DecimalNumber,
        TokenType::RightParenthesis,
        TokenType::EndOfInput};

    assert(tokenTypes(tokens) == expected);
    assert(tokens[0].lexeme() == "Σ");
    assert(tokens[1].lexeme() == "m");
    assert(tokens[3].numericValue().value() == 1);
    assert(tokens[5].numericValue().value() == 3);
    assert(tokens[7].numericValue().value() == 5);
    assert(tokens[9].lexeme() == "d");
    assert(tokens[11].numericValue().value() == 2);
    assert(tokens[13].numericValue().value() == 7);
}

void testTokenizerEdgeCases()
{
    assert(Tokenizer::tokenize("").size() == 1);
    assert(Tokenizer::tokenize(" \t\n ").size() == 1);

    assertThrowsParser([] { (void)Tokenizer::tokenize("A @ B"); });
    assertThrowsParser([] { (void)Tokenizer::tokenize("(A+B"); });
    assertThrowsParser([] { (void)Tokenizer::tokenize("A+B)"); });
    assertThrowsParser([] { (void)Tokenizer::tokenize("4294967296"); });
}

void testParserUtilities()
{
    assert(ParserUtilities::isWhitespace(' '));
    assert(ParserUtilities::isWhitespace('\n'));
    assert(!ParserUtilities::isWhitespace('A'));

    assert(ParserUtilities::isValidVariable('A'));
    assert(ParserUtilities::isValidVariable('D'));
    assert(!ParserUtilities::isValidVariable('E'));
    assert(!ParserUtilities::isValidVariable('a'));

    assert(ParserUtilities::isValidIdentifier("A"));
    assert(ParserUtilities::isValidIdentifier("m"));
    assert(!ParserUtilities::isValidIdentifier(""));
    assert(!ParserUtilities::isValidIdentifier("AB"));
    assert(!ParserUtilities::isValidIdentifier("A1"));

    assert(ParserUtilities::parseUnsignedInteger("0") == 0);
    assert(ParserUtilities::parseUnsignedInteger("12345") == 12345);
    assertThrowsParser([] { (void)ParserUtilities::parseUnsignedInteger(""); });
    assertThrowsParser([] { (void)ParserUtilities::parseUnsignedInteger("12A"); });
    assertThrowsParser([] { (void)ParserUtilities::parseUnsignedInteger("4294967296"); });

    ParserUtilities::validateNoInvalidCharacters("Σm(1,3,5)d(2,7)");
    assertThrowsParser([] { ParserUtilities::validateNoInvalidCharacters("A ? B"); });

    assert(ParserUtilities::hasBalancedParentheses("(A+B)C"));
    assert(!ParserUtilities::hasBalancedParentheses("(A+B"));
    assert(!ParserUtilities::hasBalancedParentheses("A+B)"));

    assert(ParserUtilities::hasDuplicateVariables({'A', 'B', 'A'}));
    assert(!ParserUtilities::hasDuplicateVariables({'A', 'B', 'C'}));
}

void testTokenUtilities()
{
    const auto tokens = Tokenizer::tokenize("AB + A'C + E");
    const auto variables = ParserUtilities::extractVariables(tokens);

    assert(variables == std::vector<char>({'A', 'B', 'A', 'C'}));
    assert(ParserUtilities::countUniqueVariables(tokens) == 3);

    ParserUtilities::validateBalancedParentheses(Tokenizer::tokenize("(A+B)"));

    const std::vector<Token> badTokens{
        Token(TokenType::RightParenthesis, ")"),
        Token(TokenType::EndOfInput, "")};
    assertThrowsParser([&badTokens] { ParserUtilities::validateBalancedParentheses(badTokens); });
}

void testParserBaseInterface()
{
    DummyParser parser;
    assert(!parser.parsed());

    const BooleanFunction function = parser.parse("A+B");
    assert(parser.parsed());
    assert(function.variableCount == 2);

    parser.validate("(A+B')C");
    parser.reset();
    assert(!parser.parsed());

    assertThrowsParser([&parser] { parser.validate("(A+B"); });
}

} // namespace

int main()
{
    testToken();
    testTokenizerVariablesAndOperators();
    testTokenizerParenthesesAndWhitespace();
    testTokenizerNumbersAndSigmaNotation();
    testTokenizerEdgeCases();
    testParserUtilities();
    testTokenUtilities();
    testParserBaseInterface();

    std::cout << "All Parser infrastructure tests passed!\n";

    return 0;
}
