#pragma once

#include <memory>
#include <optional>
#include <string>

namespace BooleanEngine
{

/**
 * @brief Classifies Boolean expression AST nodes.
 */
enum class ExpressionNodeKind
{
    Variable,
    UnaryOperation,
    BinaryOperation
};

/**
 * @brief Operators supported by the current Boolean expression parser.
 */
enum class BooleanOperator
{
    Not,
    And,
    Or
};

/**
 * @brief AST node for parsed Boolean expressions.
 *
 * Nodes own their children exclusively. They do not evaluate expressions and do
 * not store references into tokenizer-owned data.
 */
class ExpressionNode
{
public:
    [[nodiscard]] static std::unique_ptr<ExpressionNode> makeVariable(std::string variableName);

    [[nodiscard]] static std::unique_ptr<ExpressionNode> makeUnary(
        BooleanOperator operation,
        std::unique_ptr<ExpressionNode> operand);

    [[nodiscard]] static std::unique_ptr<ExpressionNode> makeBinary(
        BooleanOperator operation,
        std::unique_ptr<ExpressionNode> left,
        std::unique_ptr<ExpressionNode> right);

    [[nodiscard]] ExpressionNodeKind kind() const noexcept;
    [[nodiscard]] const std::string& variableName() const noexcept;
    [[nodiscard]] std::optional<BooleanOperator> operation() const noexcept;
    [[nodiscard]] const ExpressionNode* left() const noexcept;
    [[nodiscard]] const ExpressionNode* right() const noexcept;

private:
    explicit ExpressionNode(ExpressionNodeKind kind) noexcept;

    ExpressionNodeKind kind_;
    std::string variableName_;
    std::optional<BooleanOperator> operation_;
    std::unique_ptr<ExpressionNode> left_;
    std::unique_ptr<ExpressionNode> right_;
};

/**
 * @brief Deterministic prefix serialization for tests and debugging.
 */
[[nodiscard]] std::string serializeExpressionTree(const ExpressionNode& node);

} // namespace BooleanEngine
