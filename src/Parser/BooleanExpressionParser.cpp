#include "Parser/BooleanExpressionParser.hpp"

#include "Core/Exceptions.hpp"
#include "Parser/ParserUtilities.hpp"
#include "Parser/Tokenizer.hpp"

#include <sstream>
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

BooleanOperator binaryOperatorFor(const Token& token)
{
    switch (token.type())
    {
    case TokenType::And:
        return BooleanOperator::And;
    case TokenType::Or:
        return BooleanOperator::Or;
    default:
        break;
    }

    throw ParserException("Token is not a supported binary Boolean operator: " + token.lexeme());
}

} // namespace

ParsedBooleanExpression BooleanExpressionParser::parse(const std::vector<Token>& tokens) const
{
    if (tokens.empty())
    {
        throw ParserException("Empty token stream");
    }

    ParseState state(tokens);
    if (state.isAtEnd())
    {
        throw parseError("Expected expression", state.position(), state.current());
    }

    auto root = parseExpression(state);

    if (!state.isAtEnd())
    {
        throw parseError("Unexpected trailing token", state.position(), state.current());
    }

    return ParsedBooleanExpression(std::move(root));
}

ParsedBooleanExpression BooleanExpressionParser::parse(const std::string& input) const
{
    return parse(Tokenizer::tokenize(input));
}

BooleanExpressionParser::ParseState::ParseState(const std::vector<Token>& tokens) noexcept
    : tokens_(tokens)
{
}

const Token& BooleanExpressionParser::ParseState::current() const
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

const Token& BooleanExpressionParser::ParseState::previous() const
{
    if (currentIndex_ == 0 || tokens_.empty())
    {
        throw ParserException("No previous token");
    }

    return tokens_[currentIndex_ - 1];
}

bool BooleanExpressionParser::ParseState::isAtEnd() const noexcept
{
    return currentIndex_ >= tokens_.size() || tokens_[currentIndex_].type() == TokenType::EndOfInput;
}

bool BooleanExpressionParser::ParseState::check(TokenType type) const noexcept
{
    return !isAtEnd() && tokens_[currentIndex_].type() == type;
}

bool BooleanExpressionParser::ParseState::match(TokenType type)
{
    if (!check(type))
    {
        return false;
    }

    ++currentIndex_;
    return true;
}

const Token& BooleanExpressionParser::ParseState::consume(TokenType type, const std::string& message)
{
    if (check(type))
    {
        ++currentIndex_;
        return previous();
    }

    throw parseError(message, position(), current());
}

std::size_t BooleanExpressionParser::ParseState::position() const noexcept
{
    return currentIndex_;
}

std::unique_ptr<ExpressionNode> BooleanExpressionParser::parseExpression(ParseState& state) const
{
    return parseOrExpression(state);
}

std::unique_ptr<ExpressionNode> BooleanExpressionParser::parseOrExpression(ParseState& state) const
{
    auto node = parseAndExpression(state);

    while (state.match(TokenType::Or))
    {
        const Token& operatorToken = state.previous();
        auto right = parseAndExpression(state);
        node = ExpressionNode::makeBinary(binaryOperatorFor(operatorToken), std::move(node), std::move(right));
    }

    return node;
}

std::unique_ptr<ExpressionNode> BooleanExpressionParser::parseAndExpression(ParseState& state) const
{
    auto node = parseUnaryExpression(state);

    while (state.match(TokenType::And))
    {
        const Token& operatorToken = state.previous();
        auto right = parseUnaryExpression(state);
        node = ExpressionNode::makeBinary(binaryOperatorFor(operatorToken), std::move(node), std::move(right));
    }

    return node;
}

std::unique_ptr<ExpressionNode> BooleanExpressionParser::parseUnaryExpression(ParseState& state) const
{
    if (state.match(TokenType::Not))
    {
        auto operand = parseUnaryExpression(state);
        return ExpressionNode::makeUnary(BooleanOperator::Not, std::move(operand));
    }

    return parsePostfixNotExpression(state);
}

std::unique_ptr<ExpressionNode> BooleanExpressionParser::parsePostfixNotExpression(
    ParseState& state) const
{
    auto node = parsePrimaryExpression(state);

    while (state.match(TokenType::Not))
    {
        node = ExpressionNode::makeUnary(BooleanOperator::Not, std::move(node));
    }

    return node;
}

std::unique_ptr<ExpressionNode> BooleanExpressionParser::parsePrimaryExpression(
    ParseState& state) const
{
    if (state.match(TokenType::Variable))
    {
        const Token& token = state.previous();
        if (token.lexeme().size() != 1 || !ParserUtilities::isValidVariable(token.lexeme().front()))
        {
            throw parseError("Invalid Boolean variable", state.position() - 1, token);
        }

        return ExpressionNode::makeVariable(token.lexeme());
    }

    if (state.match(TokenType::LeftParenthesis))
    {
        if (state.check(TokenType::RightParenthesis))
        {
            throw parseError("Expected expression inside parentheses", state.position(), state.current());
        }

        auto expression = parseExpression(state);
        state.consume(TokenType::RightParenthesis, "Expected closing parenthesis");

        return expression;
    }

    if (state.check(TokenType::RightParenthesis))
    {
        throw parseError("Unexpected closing parenthesis", state.position(), state.current());
    }

    if (state.check(TokenType::And) || state.check(TokenType::Or))
    {
        throw parseError("Expected left operand before binary operator", state.position(), state.current());
    }

    throw parseError("Expected variable, NOT, or parenthesized expression", state.position(), state.current());
}

} // namespace BooleanEngine
