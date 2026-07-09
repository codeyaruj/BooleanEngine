#include "Expression/ExpressionUtilities.hpp"

#include "Core/Exceptions.hpp"

#include <set>
#include <utility>

namespace BooleanEngine
{
namespace
{

void collectFromNode(const ExpressionNode& node, std::set<std::string>& variables)
{
    switch (node.kind())
    {
    case ExpressionNodeKind::Variable:
        if (node.variableName().empty())
        {
            throw ParserException("Variable node is missing a variable name");
        }

        variables.insert(node.variableName());
        return;

    case ExpressionNodeKind::UnaryOperation:
        if (!node.operation().has_value())
        {
            throw ParserException("Unary node is missing an operator");
        }

        if (node.operation().value() != BooleanOperator::Not)
        {
            throw ParserException("Unsupported unary operator while collecting variables");
        }

        if (node.right() == nullptr)
        {
            throw ParserException("Unary node is missing an operand");
        }

        collectFromNode(*node.right(), variables);
        return;

    case ExpressionNodeKind::BinaryOperation:
        if (!node.operation().has_value())
        {
            throw ParserException("Binary node is missing an operator");
        }

        if (node.operation().value() == BooleanOperator::Not)
        {
            throw ParserException("Binary node has unsupported NOT operator");
        }

        if (node.left() == nullptr || node.right() == nullptr)
        {
            throw ParserException("Binary node is missing a left or right operand");
        }

        collectFromNode(*node.left(), variables);
        collectFromNode(*node.right(), variables);
        return;
    }

    throw ParserException("Unsupported expression node kind while collecting variables");
}

std::vector<std::string> toVector(const std::set<std::string>& variables)
{
    return std::vector<std::string>(variables.begin(), variables.end());
}

} // namespace

std::vector<std::string> collectVariables(const ParsedBooleanExpression& expression)
{
    if (expression.empty())
    {
        throw ParserException("Parsed expression has no root node");
    }

    return collectVariables(expression.root());
}

std::vector<std::string> collectVariables(const ExpressionNode& root)
{
    std::set<std::string> variables;
    collectFromNode(root, variables);
    return toVector(variables);
}

} // namespace BooleanEngine
