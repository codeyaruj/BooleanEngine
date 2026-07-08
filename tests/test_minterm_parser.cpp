#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "Core/Exceptions.hpp"
#include "Core/Types.hpp"
#include "Parser/MintermParser.hpp"
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

void assertFunction(const BooleanFunction& function,
                    int variableCount,
                    const std::vector<int>& minterms,
                    const std::vector<int>& dontCares)
{
    assert(function.variableCount == variableCount);
    assert(function.minterms == minterms);
    assert(function.dontCares == dontCares);
}

void testEmptyMintermList()
{
    const MintermParser parser;
    assertFunction(parser.parse("Σm()", 3), 3, {}, {});
}

void testSingleMinterms()
{
    const MintermParser parser;
    assertFunction(parser.parse("Σm(0)", 1), 1, {0}, {});
    assertFunction(parser.parse("Σm(1)", 1), 1, {1}, {});
}

void testMultipleMintermsAndWhitespace()
{
    const MintermParser parser;
    assertFunction(parser.parse("Σm(1,3,7)", 3), 3, {1, 3, 7}, {});
    assertFunction(parser.parse(" Σ m ( 1 , 3 , 7 ) ", 3), 3, {1, 3, 7}, {});
}

void testDontCareClauses()
{
    const MintermParser parser;

    assertFunction(parser.parse("Σm(1,3) + d()", 2), 2, {1, 3}, {});
    assertFunction(parser.parse("Σm() + d(0,1)", 2), 2, {}, {0, 1});
    assertFunction(parser.parse("Σm(1,3,7) + d(0,2)", 3), 3, {1, 3, 7}, {0, 2});
    assertFunction(parser.parse("Σm(1)|d(2)", 2), 2, {1}, {2});
}

void testBoundaryValuesAndGenericDimensions()
{
    const MintermParser parser;

    assertFunction(parser.parse("Σm(0,15)", 4), 4, {0, 15}, {});
    assertFunction(parser.parse("Σm(0,31)", 5), 5, {0, 31}, {});

    const std::string maxInt = std::to_string(std::numeric_limits<int>::max());
    const BooleanFunction function = parser.parse("Σm(" + maxInt + ")", 31);
    assertFunction(function, 31, {std::numeric_limits<int>::max()}, {});
}

void testCanonicalOrdering()
{
    const MintermParser parser;
    assertFunction(parser.parse("Σm(7,1,3)+d(2,0)", 3), 3, {1, 3, 7}, {0, 2});
}

void testReusableParserObject()
{
    const MintermParser parser;

    assertFunction(parser.parse("Σm(0)", 1), 1, {0}, {});
    assertFunction(parser.parse("Σm(1)", 1), 1, {1}, {});
    assertFunction(parser.parse("Σm() + d(0)", 1), 1, {}, {0});
}

void testResultOwnsData()
{
    const MintermParser parser;
    BooleanFunction function;

    {
        const std::vector<Token> tokens = Tokenizer::tokenize("Σm(1,3) + d(0)");
        function = parser.parse(tokens, 2);
    }

    assertFunction(function, 2, {1, 3}, {0});
}

void testManualTokenSequences()
{
    const MintermParser parser;

    const std::vector<Token> tokens{
        Token(TokenType::Sigma, "Σ"),
        Token(TokenType::Variable, "m"),
        Token(TokenType::LeftParenthesis, "("),
        Token(TokenType::DecimalNumber, "3", std::uint32_t{3}),
        Token(TokenType::RightParenthesis, ")")};

    assertFunction(parser.parse(tokens, 2), 2, {3}, {});

    assertThrowsParser([&parser] {
        (void)parser.parse(
            std::vector<Token>{
                Token(TokenType::Sigma, "Σ"),
                Token(TokenType::Variable, "m"),
                Token(TokenType::LeftParenthesis, "("),
                Token(TokenType::DecimalNumber, "1"),
                Token(TokenType::RightParenthesis, ")"),
                Token(TokenType::EndOfInput, "")},
            1);
    });

    assertThrowsParser([&parser] { (void)parser.parse(std::vector<Token>{}, 1); });
}

void testEmptyAndIncompleteInput()
{
    const MintermParser parser;

    assertThrowsParser([&parser] { (void)parser.parse("", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σ", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1,", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1,)", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(,1)", 2); });
}

void testParenthesisAndCommaErrors()
{
    const MintermParser parser;

    assertThrowsParser([&parser] { (void)parser.parse("Σm)", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm()(", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm((1))", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1))", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm((1)", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(,)", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1,,2)", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1 2)", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1, ,2)", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1,)", 2); });
}

void testWrongTokenTypes()
{
    const MintermParser parser;

    assertThrowsParser([&parser] { (void)parser.parse("Σm(A)", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(!)", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1|2)", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(-1)", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1.5)", 2); });
}

void testDontCareSyntaxErrors()
{
    const MintermParser parser;

    assertThrowsParser([&parser] { (void)parser.parse("Σm(1) +", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1) + d", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1) + d(", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1) + d(2", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1) + d(2,)", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1) d(2)", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1) + + d(2)", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1) + d(2) + d(3)", 3); });
    assertThrowsParser([&parser] { (void)parser.parse("d(1)", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("+ d(1)", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1) + x(2)", 2); });
}

void testDuplicateOverlapAndRangeErrors()
{
    const MintermParser parser;

    assertThrowsParser([&parser] { (void)parser.parse("Σm(1,1)", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1,2,1)", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1) + d(2,2)", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1,2) + d(2,3)", 2); });

    assertThrowsParser([&parser] { (void)parser.parse("Σm(2)", 1); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(4)", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1) + d(4)", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(8)", 3); });
}

void testTrailingInputAndVariableCountErrors()
{
    const MintermParser parser;

    assertThrowsParser([&parser] { (void)parser.parse("Σm(1) A", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1) )", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1) + d(2) A", 2); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(1) Σm(2)", 2); });

    assertThrowsParser([&parser] { (void)parser.parse("Σm(0)", 0); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(0)", 32); });
    assertThrowsParser([&parser] { (void)parser.parse("Σm(4294967296)", 2); });
}

} // namespace

int main()
{
    testEmptyMintermList();
    testSingleMinterms();
    testMultipleMintermsAndWhitespace();
    testDontCareClauses();
    testBoundaryValuesAndGenericDimensions();
    testCanonicalOrdering();
    testReusableParserObject();
    testResultOwnsData();
    testManualTokenSequences();
    testEmptyAndIncompleteInput();
    testParenthesisAndCommaErrors();
    testWrongTokenTypes();
    testDontCareSyntaxErrors();
    testDuplicateOverlapAndRangeErrors();
    testTrailingInputAndVariableCountErrors();

    std::cout << "All Minterm Parser tests passed!\n";

    return 0;
}
