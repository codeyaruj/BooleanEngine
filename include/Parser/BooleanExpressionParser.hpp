#pragma once

#include "Expression/Expression.hpp"
#include "Parser/Token.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace BooleanEngine
{

/**
 * @brief Recursive-descent parser for Boolean expressions.
 *
 * Supported grammar:
 *
 * expression    -> orExpression
 * orExpression  -> andExpression (OR andExpression)*
 * andExpression -> unaryExpression (AND unaryExpression)*
 * unaryExpression -> NOT unaryExpression | postfixNotExpression
 * postfixNotExpression -> primaryExpression (NOT)*
 * primaryExpression -> Variable | '(' expression ')'
 *
 * OR and AND are left-associative. Prefix NOT is right-associative. Postfix NOT
 * is supported because the tokenizer maps apostrophe to TokenType::Not.
 */
class BooleanExpressionParser
{
public:
    [[nodiscard]] ParsedBooleanExpression parse(const std::vector<Token>& tokens) const;
    [[nodiscard]] ParsedBooleanExpression parse(const std::string& input) const;

private:
    class ParseState
    {
    public:
        explicit ParseState(const std::vector<Token>& tokens) noexcept;

        [[nodiscard]] const Token& current() const;
        [[nodiscard]] const Token& previous() const;
        [[nodiscard]] bool isAtEnd() const noexcept;
        [[nodiscard]] bool check(TokenType type) const noexcept;
        bool match(TokenType type);
        const Token& consume(TokenType type, const std::string& message);
        [[nodiscard]] std::size_t position() const noexcept;

    private:
        const std::vector<Token>& tokens_;
        std::size_t currentIndex_ = 0;
    };

    [[nodiscard]] std::unique_ptr<ExpressionNode> parseExpression(ParseState& state) const;
    [[nodiscard]] std::unique_ptr<ExpressionNode> parseOrExpression(ParseState& state) const;
    [[nodiscard]] std::unique_ptr<ExpressionNode> parseAndExpression(ParseState& state) const;
    [[nodiscard]] std::unique_ptr<ExpressionNode> parseUnaryExpression(ParseState& state) const;
    [[nodiscard]] std::unique_ptr<ExpressionNode> parsePostfixNotExpression(ParseState& state) const;
    [[nodiscard]] std::unique_ptr<ExpressionNode> parsePrimaryExpression(ParseState& state) const;
};

} // namespace BooleanEngine
