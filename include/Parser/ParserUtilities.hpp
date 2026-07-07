#pragma once

#include "Parser/Token.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace BooleanEngine
{
namespace ParserUtilities
{

/**
 * @brief Returns true for ASCII whitespace characters accepted by parsers.
 */
[[nodiscard]] bool isWhitespace(char character) noexcept;

/**
 * @brief Returns true for supported Boolean variable names.
 */
[[nodiscard]] bool isValidVariable(char variable) noexcept;

/**
 * @brief Returns true when an identifier is one alphabetic character.
 */
[[nodiscard]] bool isValidIdentifier(const std::string& identifier) noexcept;

/**
 * @brief Parses a base-10 unsigned integer.
 *
 * @throws ParserException for empty, non-numeric, or overflowing input.
 */
[[nodiscard]] uint32_t parseUnsignedInteger(const std::string& text);

/**
 * @brief Throws ParserException when input contains an unsupported symbol.
 */
void validateNoInvalidCharacters(const std::string& input);

/**
 * @brief Checks whether parentheses are balanced in raw input text.
 */
[[nodiscard]] bool hasBalancedParentheses(const std::string& input);

/**
 * @brief Throws ParserException when the token stream has unbalanced parentheses.
 */
void validateBalancedParentheses(const std::vector<Token>& tokens);

/**
 * @brief Checks for duplicate variable names.
 */
[[nodiscard]] bool hasDuplicateVariables(const std::vector<char>& variables);

/**
 * @brief Extracts variable lexemes from a token stream.
 */
[[nodiscard]] std::vector<char> extractVariables(const std::vector<Token>& tokens);

/**
 * @brief Counts unique valid Boolean variables in a token stream.
 */
[[nodiscard]] std::size_t countUniqueVariables(const std::vector<Token>& tokens);

} // namespace ParserUtilities
} // namespace BooleanEngine
