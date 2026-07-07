#include "Parser/Tokenizer.hpp"

#include "Core/Exceptions.hpp"
#include "Parser/ParserUtilities.hpp"

#include <cctype>
#include <sstream>
#include <utility>

namespace BooleanEngine
{
namespace
{

constexpr const char* SigmaLexeme = "\xCE\xA3";
constexpr std::size_t SigmaLength = 2;

ParserException invalidCharacter(char character, std::size_t position)
{
    std::ostringstream message;
    message << "Invalid character at position " << position << ": '" << character << "'";
    return ParserException(message.str());
}

} // namespace

Tokenizer::Tokenizer(std::string input)
    : input_(std::move(input))
{
}

std::vector<Token> Tokenizer::tokenize() const
{
    std::vector<Token> tokens;
    tokens.reserve(input_.size() + 1);

    for (std::size_t index = 0; index < input_.size();)
    {
        const unsigned char current = static_cast<unsigned char>(input_[index]);

        if (ParserUtilities::isWhitespace(static_cast<char>(current)))
        {
            ++index;
            continue;
        }

        if (input_.compare(index, SigmaLength, SigmaLexeme) == 0)
        {
            tokens.emplace_back(TokenType::Sigma, SigmaLexeme);
            index += SigmaLength;
            continue;
        }

        if (std::isalpha(current) != 0)
        {
            tokens.emplace_back(TokenType::Variable, std::string(1, static_cast<char>(current)));
            ++index;
            continue;
        }

        if (std::isdigit(current) != 0)
        {
            const std::size_t start = index;

            while (index < input_.size() &&
                   std::isdigit(static_cast<unsigned char>(input_[index])) != 0)
            {
                ++index;
            }

            const std::string lexeme = input_.substr(start, index - start);
            tokens.emplace_back(TokenType::DecimalNumber,
                                lexeme,
                                ParserUtilities::parseUnsignedInteger(lexeme));
            continue;
        }

        switch (input_[index])
        {
        case '\'':
        case '!':
        case '~':
            tokens.emplace_back(TokenType::Not, std::string(1, input_[index]));
            break;
        case '*':
        case '&':
        case '.':
            tokens.emplace_back(TokenType::And, std::string(1, input_[index]));
            break;
        case '+':
        case '|':
            tokens.emplace_back(TokenType::Or, std::string(1, input_[index]));
            break;
        case '(':
            tokens.emplace_back(TokenType::LeftParenthesis, "(");
            break;
        case ')':
            tokens.emplace_back(TokenType::RightParenthesis, ")");
            break;
        case ',':
            tokens.emplace_back(TokenType::Comma, ",");
            break;
        default:
            throw invalidCharacter(input_[index], index);
        }

        ++index;
    }

    ParserUtilities::validateBalancedParentheses(tokens);
    tokens.emplace_back(TokenType::EndOfInput, "");

    return tokens;
}

std::vector<Token> Tokenizer::tokenize(const std::string& input)
{
    return Tokenizer(input).tokenize();
}

} // namespace BooleanEngine
