#include "Expression/AST.hpp"

#include "Core/Exceptions.hpp"

#include <sstream>
#include <utility>

namespace BooleanEngine
{
namespace
{

std::string operatorSymbol(BooleanOperator operation)
{
    switch (operation)
    {
    case BooleanOperator::Not:
        return "!";
    case BooleanOperator::And:
        return "&";
    case BooleanOperator::Or:
        return "|";
    }

    return "?";
}

} // namespace

std::unique_ptr<ExpressionNode> ExpressionNode::makeVariable(std::string variableName)
{
    if (variableName.empty())
    {
        throw ParserException("Variable node requires a non-empty variable name");
    }

    auto node = std::unique_ptr<ExpressionNode>(new ExpressionNode(ExpressionNodeKind::Variable));
    node->variableName_ = std::move(variableName);

    return node;
}

std::unique_ptr<ExpressionNode> ExpressionNode::makeUnary(BooleanOperator operation,
                                                          std::unique_ptr<ExpressionNode> operand)
{
    if (operation != BooleanOperator::Not)
    {
        throw ParserException("Unary expression supports only NOT");
    }

    if (!operand)
    {
        throw ParserException("Unary expression requires an operand");
    }

    auto node =
        std::unique_ptr<ExpressionNode>(new ExpressionNode(ExpressionNodeKind::UnaryOperation));
    node->operation_ = operation;
    node->right_ = std::move(operand);

    return node;
}

std::unique_ptr<ExpressionNode> ExpressionNode::makeBinary(BooleanOperator operation,
                                                           std::unique_ptr<ExpressionNode> left,
                                                           std::unique_ptr<ExpressionNode> right)
{
    if (operation == BooleanOperator::Not)
    {
        throw ParserException("Binary expression does not support NOT");
    }

    if (!left || !right)
    {
        throw ParserException("Binary expression requires left and right operands");
    }

    auto node =
        std::unique_ptr<ExpressionNode>(new ExpressionNode(ExpressionNodeKind::BinaryOperation));
    node->operation_ = operation;
    node->left_ = std::move(left);
    node->right_ = std::move(right);

    return node;
}

ExpressionNodeKind ExpressionNode::kind() const noexcept
{
    return kind_;
}

const std::string& ExpressionNode::variableName() const noexcept
{
    return variableName_;
}

std::optional<BooleanOperator> ExpressionNode::operation() const noexcept
{
    return operation_;
}

const ExpressionNode* ExpressionNode::left() const noexcept
{
    return left_.get();
}

const ExpressionNode* ExpressionNode::right() const noexcept
{
    return right_.get();
}

ExpressionNode::ExpressionNode(ExpressionNodeKind kind) noexcept
    : kind_(kind)
{
}

std::string serializeExpressionTree(const ExpressionNode& node)
{
    if (node.kind() == ExpressionNodeKind::Variable)
    {
        return node.variableName();
    }

    if (!node.operation().has_value())
    {
        throw ParserException("Operation node is missing an operator");
    }

    if (node.kind() == ExpressionNodeKind::UnaryOperation)
    {
        if (node.right() == nullptr)
        {
            throw ParserException("Unary node is missing an operand");
        }

        return "(" + operatorSymbol(node.operation().value()) + " " +
               serializeExpressionTree(*node.right()) + ")";
    }

    if (node.left() == nullptr || node.right() == nullptr)
    {
        throw ParserException("Binary node is missing an operand");
    }

    return "(" + operatorSymbol(node.operation().value()) + " " +
           serializeExpressionTree(*node.left()) + " " + serializeExpressionTree(*node.right()) +
           ")";
}

} // namespace BooleanEngine
