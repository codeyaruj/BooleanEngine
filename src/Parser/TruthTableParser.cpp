#include "Parser/TruthTableParser.hpp"

#include "Core/Exceptions.hpp"
#include "Parser/ParserUtilities.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace BooleanEngine
{
namespace
{

enum class TruthValue
{
    Zero,
    One,
    DontCare
};

struct LogicalLine
{
    std::string text;
    std::size_t number = 0;
};

struct Header
{
    std::vector<std::string> variableNames;
    std::string outputName;
};

std::string trim(const std::string& text)
{
    std::size_t first = 0;
    while (first < text.size() && ParserUtilities::isWhitespace(text[first]))
    {
        ++first;
    }

    std::size_t last = text.size();
    while (last > first && ParserUtilities::isWhitespace(text[last - 1u]))
    {
        --last;
    }

    return text.substr(first, last - first);
}

std::vector<std::string> splitWords(const std::string& text)
{
    std::istringstream input(text);
    std::vector<std::string> words;
    std::string word;

    while (input >> word)
    {
        words.push_back(word);
    }

    return words;
}

std::vector<LogicalLine> splitLogicalLines(std::string_view source)
{
    std::istringstream input{std::string(source)};
    std::vector<LogicalLine> lines;
    std::string line;
    std::size_t lineNumber = 1;

    while (std::getline(input, line))
    {
        const std::string cleaned = trim(line);
        if (!cleaned.empty())
        {
            lines.push_back(LogicalLine{cleaned, lineNumber});
        }

        ++lineNumber;
    }

    return lines;
}

ParserException lineError(std::size_t lineNumber, const std::string& message)
{
    std::ostringstream output;
    output << "Line " << lineNumber << ": " << message;
    return ParserException(output.str());
}

std::pair<std::string, std::string> splitAtSeparator(const LogicalLine& line)
{
    const std::size_t firstSeparator = line.text.find('|');
    if (firstSeparator == std::string::npos)
    {
        throw lineError(line.number, "Expected '|' separator");
    }

    if (line.text.find('|', firstSeparator + 1u) != std::string::npos)
    {
        throw lineError(line.number, "Expected exactly one '|' separator");
    }

    return {trim(line.text.substr(0, firstSeparator)),
            trim(line.text.substr(firstSeparator + 1u))};
}

void validateIdentifier(const std::string& identifier,
                        std::size_t lineNumber,
                        const std::string& label)
{
    if (!ParserUtilities::isValidIdentifier(identifier))
    {
        throw lineError(lineNumber, "Invalid " + label + " name '" + identifier + "'");
    }
}

Header parseHeader(const LogicalLine& line)
{
    const auto [inputSide, outputSide] = splitAtSeparator(line);
    const std::vector<std::string> variables = splitWords(inputSide);
    const std::vector<std::string> outputs = splitWords(outputSide);

    if (variables.empty())
    {
        throw lineError(line.number, "Header must contain at least one input variable");
    }

    if (outputs.size() != 1u)
    {
        throw lineError(line.number, "Header must contain exactly one output column after '|'");
    }

    std::unordered_set<std::string> seenVariables;
    for (const std::string& variable : variables)
    {
        validateIdentifier(variable, line.number, "input variable");

        if (!seenVariables.insert(variable).second)
        {
            throw lineError(line.number, "Duplicate variable name '" + variable + "' in header");
        }
    }

    validateIdentifier(outputs.front(), line.number, "output");
    if (seenVariables.find(outputs.front()) != seenVariables.end())
    {
        throw lineError(line.number,
                        "Output name '" + outputs.front() +
                            "' must not duplicate an input variable");
    }

    return Header{variables, outputs.front()};
}

std::uint64_t checkedDomainSizeFor(std::size_t variableCount)
{
    if (variableCount == 0)
    {
        throw ParserException("Truth table must contain at least one input variable");
    }

    if (variableCount > static_cast<std::size_t>(std::numeric_limits<int>::digits))
    {
        std::ostringstream message;
        message << "Variable count " << variableCount
                << " exceeds the BooleanFunction index range";
        throw ParserException(message.str());
    }

    const std::uint64_t domainSize = std::uint64_t{1} << variableCount;
    const std::uint64_t maxIndex = domainSize - 1u;

    if (maxIndex > static_cast<std::uint64_t>(std::numeric_limits<int>::max()))
    {
        std::ostringstream message;
        message << "Truth-table indices for " << variableCount
                << " variables cannot be represented by BooleanFunction";
        throw ParserException(message.str());
    }

    if (domainSize > static_cast<std::uint64_t>(std::numeric_limits<std::size_t>::max()))
    {
        std::ostringstream message;
        message << "Truth-table domain for " << variableCount
                << " variables is too large for this platform";
        throw ParserException(message.str());
    }

    return domainSize;
}

TruthValue parseOutputValue(const std::string& value, std::size_t lineNumber)
{
    if (value == "0")
    {
        return TruthValue::Zero;
    }

    if (value == "1")
    {
        return TruthValue::One;
    }

    if (value == "X" || value == "x")
    {
        return TruthValue::DontCare;
    }

    throw lineError(lineNumber,
                    "Invalid output value '" + value + "'; expected 0, 1, or X");
}

int assignmentIndex(const std::vector<std::string>& inputValues, std::size_t lineNumber)
{
    std::uint64_t index = 0;

    for (const std::string& value : inputValues)
    {
        if (value != "0" && value != "1")
        {
            throw lineError(lineNumber,
                            "Invalid input value '" + value + "'; expected 0 or 1");
        }

        index = (index << 1u) + static_cast<std::uint64_t>(value == "1" ? 1u : 0u);
    }

    return static_cast<int>(index);
}

std::string assignmentLabel(const std::vector<std::string>& inputValues)
{
    std::string label;
    label.reserve(inputValues.size());

    for (const std::string& value : inputValues)
    {
        label += value;
    }

    return label;
}

BooleanFunction buildFunction(std::size_t variableCount,
                              std::vector<int> minterms,
                              std::vector<int> dontCares)
{
    std::sort(minterms.begin(), minterms.end());
    std::sort(dontCares.begin(), dontCares.end());

    BooleanFunction function;
    function.variableCount = static_cast<int>(variableCount);
    function.minterms = std::move(minterms);
    function.dontCares = std::move(dontCares);
    return function;
}

} // namespace

BooleanFunction TruthTableParser::parse(std::string_view source) const
{
    const std::vector<LogicalLine> lines = splitLogicalLines(source);
    if (lines.empty())
    {
        throw ParserException("Expected header line");
    }

    const Header header = parseHeader(lines.front());
    const std::size_t variableCount = header.variableNames.size();
    const std::uint64_t domainSize = checkedDomainSizeFor(variableCount);

    std::vector<int> minterms;
    std::vector<int> dontCares;
    std::unordered_set<int> seenAssignments;

    for (std::size_t index = 1; index < lines.size(); ++index)
    {
        const LogicalLine& line = lines[index];
        const auto [inputSide, outputSide] = splitAtSeparator(line);
        const std::vector<std::string> inputValues = splitWords(inputSide);
        const std::vector<std::string> outputValues = splitWords(outputSide);

        if (inputValues.size() != variableCount)
        {
            std::ostringstream message;
            message << "Row has " << inputValues.size() << " input values, expected "
                    << variableCount;
            throw lineError(line.number, message.str());
        }

        if (outputValues.size() != 1u)
        {
            throw lineError(line.number, "Row must contain exactly one output value after '|'");
        }

        const int assignment = assignmentIndex(inputValues, line.number);
        if (!seenAssignments.insert(assignment).second)
        {
            throw lineError(line.number,
                            "Duplicate assignment " + assignmentLabel(inputValues));
        }

        const TruthValue output = parseOutputValue(outputValues.front(), line.number);
        if (output == TruthValue::One)
        {
            minterms.push_back(assignment);
        }
        else if (output == TruthValue::DontCare)
        {
            dontCares.push_back(assignment);
        }

        if (seenAssignments.size() > domainSize)
        {
            std::ostringstream message;
            message << "Too many rows for " << variableCount << " variables: expected "
                    << domainSize << " unique assignments";
            throw ParserException(message.str());
        }
    }

    if (seenAssignments.size() != domainSize)
    {
        std::ostringstream message;
        message << "Truth table is incomplete: expected " << domainSize << " rows, got "
                << seenAssignments.size();
        throw ParserException(message.str());
    }

    return buildFunction(variableCount, std::move(minterms), std::move(dontCares));
}

} // namespace BooleanEngine
