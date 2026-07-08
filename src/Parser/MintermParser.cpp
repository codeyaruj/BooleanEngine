#include "Parser/MintermParser.hpp"

#include "Core/Exceptions.hpp"
#include "Parser/Tokenizer.hpp"

#include <algorithm>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>

namespace BooleanEngine
{
namespace
{

std::string describeToken(const Token& token)
{
    if (!token.lexeme().empty())
    {
        return "'" + token.lexeme() + "'";
    }

    return "end of input";
}

ParserException parseError(const std::string& message, std::size_t position, const Token& token)
{
    std::ostringstream output;
    output << message << " at token index " << position << " near " << describeToken(token);
    return ParserException(output.str());
}

std::uint64_t checkedMaxIndexFor(std::size_t variableCount)
{
    if (variableCount == 0)
    {
        throw ParserException("Variable count must be greater than zero");
    }

    if (variableCount > static_cast<std::size_t>(std::numeric_limits<int>::digits))
    {
        std::ostringstream message;
        message << "Variable count " << variableCount
                << " exceeds the BooleanFunction index range";
        throw ParserException(message.str());
    }

    const std::uint64_t stateCount = std::uint64_t{1} << variableCount;
    const std::uint64_t maxIndex = stateCount - 1u;

    if (maxIndex > static_cast<std::uint64_t>(std::numeric_limits<int>::max()))
    {
        std::ostringstream message;
        message << "Minterm indices for " << variableCount
                << " variables cannot be represented by BooleanFunction";
        throw ParserException(message.str());
    }

    return maxIndex;
}

void validateNoOverlap(const std::vector<int>& minterms, const std::vector<int>& dontCares)
{
    std::unordered_set<int> mintermSet(minterms.begin(), minterms.end());

    for (int value : dontCares)
    {
        if (mintermSet.find(value) != mintermSet.end())
        {
            std::ostringstream message;
            message << "Index " << value << " appears in both the minterm and don't-care lists";
            throw ParserException(message.str());
        }
    }
}

BooleanFunction buildBooleanFunction(std::size_t variableCount,
                                     std::vector<int> minterms,
                                     std::vector<int> dontCares)
{
    BooleanFunction function;
    function.variableCount = static_cast<int>(variableCount);
    function.minterms = std::move(minterms);
    function.dontCares = std::move(dontCares);
    return function;
}

} // namespace

BooleanFunction MintermParser::parse(const std::vector<Token>& tokens,
                                     std::size_t variableCount) const
{
    if (tokens.empty())
    {
        throw ParserException("Empty token stream");
    }

    const std::uint64_t maxIndex = checkedMaxIndexFor(variableCount);
    ParseState state(tokens);

    std::vector<int> minterms = parseMintermClause(state, maxIndex, variableCount);
    std::vector<int> dontCares;

    if (state.match(TokenType::Or))
    {
        dontCares = parseDontCareClause(state, maxIndex, variableCount);
    }

    if (!state.isAtEnd())
    {
        throw parseError("Unexpected trailing token after minterm specification",
                         state.position(),
                         state.current());
    }

    validateNoOverlap(minterms, dontCares);
    return buildBooleanFunction(variableCount, std::move(minterms), std::move(dontCares));
}

BooleanFunction MintermParser::parse(const std::string& input,
                                     std::size_t variableCount) const
{
    return parse(Tokenizer::tokenize(input), variableCount);
}

MintermParser::ParseState::ParseState(const std::vector<Token>& tokens) noexcept
    : tokens_(tokens)
{
}

const Token& MintermParser::ParseState::current() const
{
    if (tokens_.empty())
    {
        throw ParserException("Empty token stream");
    }

    if (currentIndex_ >= tokens_.size())
    {
        return tokens_.back();
    }

    return tokens_[currentIndex_];
}

const Token& MintermParser::ParseState::previous() const
{
    if (currentIndex_ == 0 || tokens_.empty())
    {
        throw ParserException("No previous token");
    }

    return tokens_[currentIndex_ - 1u];
}

bool MintermParser::ParseState::isAtEnd() const noexcept
{
    return currentIndex_ >= tokens_.size() ||
           tokens_[currentIndex_].type() == TokenType::EndOfInput;
}

bool MintermParser::ParseState::check(TokenType type) const noexcept
{
    return !isAtEnd() && tokens_[currentIndex_].type() == type;
}

bool MintermParser::ParseState::match(TokenType type)
{
    if (!check(type))
    {
        return false;
    }

    ++currentIndex_;
    return true;
}

const Token& MintermParser::ParseState::consume(TokenType type,
                                                const std::string& message)
{
    if (check(type))
    {
        ++currentIndex_;
        return previous();
    }

    throw parseError(message, position(), current());
}

std::size_t MintermParser::ParseState::position() const noexcept
{
    return currentIndex_;
}

std::vector<int> MintermParser::parseMintermClause(ParseState& state,
                                                  std::uint64_t maxIndex,
                                                  std::size_t variableCount) const
{
    state.consume(TokenType::Sigma, "Expected Sigma symbol at start of minterm specification");
    consumeIdentifier(state, "m", "Expected 'm' after Sigma");
    return parseParenthesizedIndexList(state, "minterm", maxIndex, variableCount);
}

std::vector<int> MintermParser::parseDontCareClause(ParseState& state,
                                                   std::uint64_t maxIndex,
                                                   std::size_t variableCount) const
{
    consumeIdentifier(state, "d", "Expected don't-care clause 'd' after '+'");
    return parseParenthesizedIndexList(state, "don't-care", maxIndex, variableCount);
}

std::vector<int> MintermParser::parseParenthesizedIndexList(ParseState& state,
                                                           const std::string& listName,
                                                           std::uint64_t maxIndex,
                                                           std::size_t variableCount) const
{
    state.consume(TokenType::LeftParenthesis, "Expected '(' before " + listName + " list");

    std::vector<int> values;
    std::unordered_set<int> seen;

    if (state.match(TokenType::RightParenthesis))
    {
        return values;
    }

    while (true)
    {
        const int value = parseIndex(state, listName, maxIndex, variableCount);
        if (!seen.insert(value).second)
        {
            std::ostringstream message;
            message << "Duplicate " << listName << " " << value;
            throw ParserException(message.str());
        }

        values.push_back(value);

        if (!state.match(TokenType::Comma))
        {
            break;
        }
    }

    state.consume(TokenType::RightParenthesis, "Expected ')' after " + listName + " list");

    std::sort(values.begin(), values.end());
    return values;
}

int MintermParser::parseIndex(ParseState& state,
                              const std::string& listName,
                              std::uint64_t maxIndex,
                              std::size_t variableCount) const
{
    const Token& token = state.consume(TokenType::DecimalNumber,
                                       "Expected integer " + listName + " index");

    if (!token.hasNumericValue())
    {
        throw parseError("Decimal token is missing its numeric payload",
                         state.position() - 1u,
                         token);
    }

    const std::uint32_t value = token.numericValue().value();
    if (value > maxIndex)
    {
        std::ostringstream message;
        message << "Index " << value << " is outside the valid range [0, " << maxIndex
                << "] for " << variableCount << " variables";
        throw ParserException(message.str());
    }

    return static_cast<int>(value);
}

void MintermParser::consumeIdentifier(ParseState& state,
                                      const std::string& expected,
                                      const std::string& message) const
{
    const Token& token = state.consume(TokenType::Variable, message);
    if (token.lexeme() != expected)
    {
        throw parseError(message, state.position() - 1u, token);
    }
}

} // namespace BooleanEngine
