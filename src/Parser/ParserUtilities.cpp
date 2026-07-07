#include "Parser/ParserUtilities.hpp"

#include "Core/Exceptions.hpp"

#include <algorithm>
#include <cctype>
#include <limits>
#include <sstream>
#include <unordered_set>

namespace BooleanEngine
{
namespace ParserUtilities
{

bool isWhitespace(char character) noexcept
{
    return character == ' ' || character == '\t' || character == '\n' || character == '\r' ||
           character == '\f' || character == '\v';
}

bool isValidVariable(char variable) noexcept
{
    return variable >= 'A' && variable <= 'D';
}

bool isValidIdentifier(const std::string& identifier) noexcept
{
    return identifier.size() == 1 &&
           std::isalpha(static_cast<unsigned char>(identifier.front())) != 0;
}

uint32_t parseUnsignedInteger(const std::string& text)
{
    if (text.empty())
    {
        throw ParserException("Expected decimal number, got empty input");
    }

    uint64_t value = 0;

    for (char character : text)
    {
        if (std::isdigit(static_cast<unsigned char>(character)) == 0)
        {
            throw ParserException("Invalid decimal number: " + text);
        }

        value = (value * 10u) + static_cast<uint64_t>(character - '0');
        if (value > std::numeric_limits<uint32_t>::max())
        {
            throw ParserException("Decimal number is too large: " + text);
        }
    }

    return static_cast<uint32_t>(value);
}

void validateNoInvalidCharacters(const std::string& input)
{
    for (std::size_t index = 0; index < input.size(); ++index)
    {
        const unsigned char character = static_cast<unsigned char>(input[index]);

        if (isWhitespace(static_cast<char>(character)) || std::isalpha(character) != 0 ||
            std::isdigit(character) != 0)
        {
            continue;
        }

        if (input.compare(index, 2, "\xCE\xA3") == 0)
        {
            ++index;
            continue;
        }

        switch (input[index])
        {
        case '\'':
        case '!':
        case '~':
        case '*':
        case '&':
        case '.':
        case '+':
        case '|':
        case '(':
        case ')':
        case ',':
            break;
        default:
            {
                std::ostringstream message;
                message << "Invalid character at position " << index << ": '" << input[index]
                        << "'";
                throw ParserException(message.str());
            }
        }
    }
}

bool hasBalancedParentheses(const std::string& input)
{
    int depth = 0;

    for (char character : input)
    {
        if (character == '(')
        {
            ++depth;
        }
        else if (character == ')')
        {
            --depth;
            if (depth < 0)
            {
                return false;
            }
        }
    }

    return depth == 0;
}

void validateBalancedParentheses(const std::vector<Token>& tokens)
{
    int depth = 0;

    for (const Token& token : tokens)
    {
        if (token.type() == TokenType::LeftParenthesis)
        {
            ++depth;
        }
        else if (token.type() == TokenType::RightParenthesis)
        {
            --depth;
            if (depth < 0)
            {
                throw ParserException("Unmatched right parenthesis");
            }
        }
    }

    if (depth != 0)
    {
        throw ParserException("Unmatched left parenthesis");
    }
}

bool hasDuplicateVariables(const std::vector<char>& variables)
{
    std::unordered_set<char> seen;

    for (char variable : variables)
    {
        if (!seen.insert(variable).second)
        {
            return true;
        }
    }

    return false;
}

std::vector<char> extractVariables(const std::vector<Token>& tokens)
{
    std::vector<char> variables;

    for (const Token& token : tokens)
    {
        if (token.type() != TokenType::Variable || token.lexeme().size() != 1)
        {
            continue;
        }

        const char variable = token.lexeme().front();
        if (isValidVariable(variable))
        {
            variables.push_back(variable);
        }
    }

    return variables;
}

std::size_t countUniqueVariables(const std::vector<Token>& tokens)
{
    const std::vector<char> variables = extractVariables(tokens);
    std::unordered_set<char> uniqueVariables(variables.begin(), variables.end());

    return uniqueVariables.size();
}

} // namespace ParserUtilities
} // namespace BooleanEngine
