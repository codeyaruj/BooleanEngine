#include "Expression/Expression.hpp"

#include "Core/Exceptions.hpp"

#include <utility>

namespace BooleanEngine
{

ParsedBooleanExpression::ParsedBooleanExpression(std::unique_ptr<ExpressionNode> root) noexcept
    : root_(std::move(root))
{
}

bool ParsedBooleanExpression::empty() const noexcept
{
    return root_ == nullptr;
}

const ExpressionNode& ParsedBooleanExpression::root() const
{
    if (root_ == nullptr)
    {
        throw ParserException("Parsed expression has no root node");
    }

    return *root_;
}

} // namespace BooleanEngine
