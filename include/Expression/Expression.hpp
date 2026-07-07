#pragma once

#include "Expression/AST.hpp"

#include <memory>

namespace BooleanEngine
{

/**
 * @brief Owns the AST produced by BooleanExpressionParser.
 *
 * This intentionally does not duplicate the existing Core::BooleanExpression
 * struct, which stores generated SOP/POS strings for later simplification
 * stages.
 */
class ParsedBooleanExpression
{
public:
    ParsedBooleanExpression() = default;
    explicit ParsedBooleanExpression(std::unique_ptr<ExpressionNode> root) noexcept;

    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] const ExpressionNode& root() const;

private:
    std::unique_ptr<ExpressionNode> root_;
};

} // namespace BooleanEngine
