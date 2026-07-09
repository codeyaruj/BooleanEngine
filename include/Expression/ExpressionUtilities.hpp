#pragma once

#include "Expression/AST.hpp"
#include "Expression/Expression.hpp"

#include <string>
#include <vector>

namespace BooleanEngine
{

/**
 * @brief Collects unique variable names from a parsed expression.
 *
 * Variables are returned in deterministic ascending lexicographic order.
 *
 * @throws ParserException when the parsed expression is empty or the AST is
 * structurally malformed.
 */
[[nodiscard]] std::vector<std::string> collectVariables(
    const ParsedBooleanExpression& expression);

/**
 * @brief Collects unique variable names from an expression tree root.
 *
 * Variables are returned in deterministic ascending lexicographic order.
 *
 * @throws ParserException when the AST is structurally malformed.
 */
[[nodiscard]] std::vector<std::string> collectVariables(const ExpressionNode& root);

} // namespace BooleanEngine
