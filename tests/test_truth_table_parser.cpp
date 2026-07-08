#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Core/Exceptions.hpp"
#include "Core/Types.hpp"
#include "Parser/TruthTableParser.hpp"

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

std::string twoVariableHeader()
{
    return "A B | F\n";
}

std::string generatedFiveVariableTable()
{
    std::ostringstream source;
    source << "A B C D E | F\n";

    for (int index = 0; index < 32; ++index)
    {
        for (int bit = 4; bit >= 0; --bit)
        {
            source << (((index >> bit) & 1) != 0 ? '1' : '0');
            if (bit > 0)
            {
                source << ' ';
            }
        }

        source << " | ";
        if (index == 0 || index == 31)
        {
            source << '1';
        }
        else if (index == 7 || index == 18)
        {
            source << 'X';
        }
        else
        {
            source << '0';
        }

        source << '\n';
    }

    return source.str();
}

std::string oversizedHeader()
{
    std::ostringstream source;

    for (char variable = 'A'; variable <= 'Z'; ++variable)
    {
        source << variable << ' ';
    }

    for (char variable = 'a'; variable <= 'f'; ++variable)
    {
        source << variable << ' ';
    }

    source << "| g\n";
    return source.str();
}

void testOneVariableTable()
{
    const TruthTableParser parser;
    assertFunction(parser.parse("A | F\n0 | 0\n1 | 1\n"), 1, {1}, {});
}

void testTwoVariableTable()
{
    const TruthTableParser parser;

    assertFunction(parser.parse(twoVariableHeader() +
                                "0 0 | 0\n"
                                "0 1 | 1\n"
                                "1 0 | 1\n"
                                "1 1 | 0\n"),
                   2,
                   {1, 2},
                   {});
}

void testThreeVariableTableWithDontCares()
{
    const TruthTableParser parser;

    assertFunction(parser.parse("A B C | F\n"
                                "0 0 0 | 0\n"
                                "0 0 1 | 1\n"
                                "0 1 0 | X\n"
                                "0 1 1 | 1\n"
                                "1 0 0 | 0\n"
                                "1 0 1 | 1\n"
                                "1 1 0 | 0\n"
                                "1 1 1 | X\n"),
                   3,
                   {1, 3, 5},
                   {2, 7});
}

void testRowsOutOfOrder()
{
    const TruthTableParser parser;

    assertFunction(parser.parse(twoVariableHeader() +
                                "1 1 | 0\n"
                                "0 0 | 1\n"
                                "1 0 | 1\n"
                                "0 1 | X\n"),
                   2,
                   {0, 2},
                   {1});
}

void testWhitespaceAndLowercaseDontCare()
{
    const TruthTableParser parser;

    assertFunction(parser.parse("\n"
                                "   A   B   |   F   \n"
                                "\n"
                                " 0   0  |  0 \n"
                                " 0   1  |  x \n"
                                " 1   0  |  1 \n"
                                " 1   1  |  0 \n"
                                "\n"),
                   2,
                   {2},
                   {1});
}

void testSupportedVariableNames()
{
    const TruthTableParser parser;

    assertFunction(parser.parse("A Z | F\n"
                                "0 0 | 0\n"
                                "0 1 | 1\n"
                                "1 0 | X\n"
                                "1 1 | 0\n"),
                   2,
                   {1},
                   {2});

    assertFunction(parser.parse("a b | f\n"
                                "0 0 | 0\n"
                                "0 1 | 0\n"
                                "1 0 | 0\n"
                                "1 1 | 1\n"),
                   2,
                   {3},
                   {});
}

void testFiveVariableTable()
{
    const TruthTableParser parser;
    assertFunction(parser.parse(generatedFiveVariableTable()), 5, {0, 31}, {7, 18});
}

void testReusableParserObject()
{
    const TruthTableParser parser;

    assertFunction(parser.parse("A | F\n0 | 0\n1 | 1\n"), 1, {1}, {});
    assertFunction(parser.parse("A | F\n0 | X\n1 | 0\n"), 1, {}, {0});
}

void testResultOwnsData()
{
    const TruthTableParser parser;
    BooleanFunction function;

    {
        const std::string source = twoVariableHeader() +
                                   "0 0 | 0\n"
                                   "0 1 | 1\n"
                                   "1 0 | X\n"
                                   "1 1 | 0\n";
        function = parser.parse(source);
    }

    assertFunction(function, 2, {1}, {2});
}

void testHeaderErrors()
{
    const TruthTableParser parser;

    assertThrowsParser([&parser] { (void)parser.parse(""); });
    assertThrowsParser([&parser] { (void)parser.parse("| F\n"); });
    assertThrowsParser([&parser] { (void)parser.parse("A B |\n"); });
    assertThrowsParser([&parser] { (void)parser.parse("A A | F\n"); });
    assertThrowsParser([&parser] { (void)parser.parse("A F | F\n"); });
    assertThrowsParser([&parser] { (void)parser.parse("A B F\n"); });
    assertThrowsParser([&parser] { (void)parser.parse("A B || F\n"); });
    assertThrowsParser([&parser] { (void)parser.parse("A B | F G\n"); });
    assertThrowsParser([&parser] { (void)parser.parse("AA | F\n"); });
    assertThrowsParser([&parser] { (void)parser.parse("A1 | F\n"); });
    assertThrowsParser([&parser] { (void)parser.parse("data_in | F\n"); });
}

void testRowShapeErrors()
{
    const TruthTableParser parser;

    assertThrowsParser([&parser] { (void)parser.parse(twoVariableHeader() + "0 | 1\n"); });
    assertThrowsParser([&parser] { (void)parser.parse(twoVariableHeader() + "0 1 1 | 0\n"); });
    assertThrowsParser([&parser] { (void)parser.parse(twoVariableHeader() + "0 1 |\n"); });
    assertThrowsParser([&parser] { (void)parser.parse(twoVariableHeader() + "0 1 | 0 1\n"); });
    assertThrowsParser([&parser] { (void)parser.parse(twoVariableHeader() + "0 1 0\n"); });
}

void testInvalidInputValues()
{
    const TruthTableParser parser;

    assertThrowsParser([&parser] {
        (void)parser.parse(twoVariableHeader() +
                           "0 2 | 1\n"
                           "0 1 | 0\n"
                           "1 0 | 1\n"
                           "1 1 | 0\n");
    });

    assertThrowsParser([&parser] {
        (void)parser.parse(twoVariableHeader() +
                           "A 1 | 0\n"
                           "0 1 | 0\n"
                           "1 0 | 1\n"
                           "1 1 | 0\n");
    });
}

void testInvalidOutputValues()
{
    const TruthTableParser parser;

    assertThrowsParser([&parser] {
        (void)parser.parse(twoVariableHeader() +
                           "0 0 | Q\n"
                           "0 1 | 1\n"
                           "1 0 | 0\n"
                           "1 1 | 1\n");
    });

    assertThrowsParser([&parser] {
        (void)parser.parse(twoVariableHeader() +
                           "0 0 | -\n"
                           "0 1 | 1\n"
                           "1 0 | 0\n"
                           "1 1 | 1\n");
    });
}

void testDuplicateMissingAndTooManyRows()
{
    const TruthTableParser parser;

    assertThrowsParser([&parser] {
        (void)parser.parse(twoVariableHeader() +
                           "0 0 | 0\n"
                           "0 0 | 1\n"
                           "0 1 | 1\n"
                           "1 0 | 0\n"
                           "1 1 | 1\n");
    });

    assertThrowsParser([&parser] {
        (void)parser.parse(twoVariableHeader() +
                           "0 0 | 0\n"
                           "0 1 | 1\n"
                           "1 0 | 1\n");
    });

    assertThrowsParser([&parser] {
        (void)parser.parse("A | F\n"
                           "0 | 0\n"
                           "1 | 1\n"
                           "1 | 0\n");
    });
}

void testStrayLineAndInvalidVariableCount()
{
    const TruthTableParser parser;

    assertThrowsParser([&parser] {
        (void)parser.parse(twoVariableHeader() +
                           "0 0 | 0\n"
                           "# comment\n"
                           "0 1 | 1\n"
                           "1 0 | 0\n"
                           "1 1 | 1\n");
    });

    assertThrowsParser([&parser] { (void)parser.parse(oversizedHeader()); });
}

} // namespace

int main()
{
    testOneVariableTable();
    testTwoVariableTable();
    testThreeVariableTableWithDontCares();
    testRowsOutOfOrder();
    testWhitespaceAndLowercaseDontCare();
    testSupportedVariableNames();
    testFiveVariableTable();
    testReusableParserObject();
    testResultOwnsData();
    testHeaderErrors();
    testRowShapeErrors();
    testInvalidInputValues();
    testInvalidOutputValues();
    testDuplicateMissingAndTooManyRows();
    testStrayLineAndInvalidVariableCount();

    std::cout << "All Truth Table Parser tests passed!\n";

    return 0;
}
