#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <utility>

namespace BooleanEngine
{

/**
 * @brief Token categories shared by all parser implementations.
 */
enum class TokenType
{
    Variable,
    Not,
    And,
    Or,
    LeftParenthesis,
    RightParenthesis,
    Comma,
    Sigma,
    DecimalNumber,
    EndOfInput
};

/**
 * @brief One lexical token produced by the tokenizer.
 */
class Token
{
public:
    Token() = default;

    Token(TokenType type, std::string lexeme, std::optional<uint32_t> numericValue = std::nullopt)
        : type_(type),
          lexeme_(std::move(lexeme)),
          numericValue_(numericValue)
    {
    }

    [[nodiscard]] TokenType type() const noexcept
    {
        return type_;
    }

    [[nodiscard]] const std::string& lexeme() const noexcept
    {
        return lexeme_;
    }

    [[nodiscard]] bool hasNumericValue() const noexcept
    {
        return numericValue_.has_value();
    }

    [[nodiscard]] std::optional<uint32_t> numericValue() const noexcept
    {
        return numericValue_;
    }

private:
    TokenType type_ = TokenType::EndOfInput;
    std::string lexeme_;
    std::optional<uint32_t> numericValue_;
};

} // namespace BooleanEngine
