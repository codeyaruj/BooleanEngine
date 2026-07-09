#include "Evaluation/BooleanEvaluator.hpp"

#include "Core/Exceptions.hpp"

#include <sstream>

namespace BooleanEngine
{
namespace
{

const char* operatorName(BooleanOperator operation) noexcept
{
    switch (operation)
    {
    case BooleanOperator::Not:
        return "NOT";
    case BooleanOperator::And:
        return "AND";
    case BooleanOperator::Or:
        return "OR";
    }

    return "unknown";
}

EvaluationException missingOperator(ExpressionNodeKind kind)
{
    const char* nodeKind = "operation";
    if (kind == ExpressionNodeKind::UnaryOperation)
    {
        nodeKind = "unary";
    }
    else if (kind == ExpressionNodeKind::BinaryOperation)
    {
        nodeKind = "binary";
    }

    std::ostringstream message;
    message << "Malformed " << nodeKind << " node is missing an operator";
    return EvaluationException(message.str());
}

} // namespace

bool BooleanEvaluator::evaluate(const ParsedBooleanExpression& expression,
                                const AssignmentMap& variables) const
{
    if (expression.empty())
    {
        throw EvaluationException("Parsed expression has no root node");
    }

    return evaluate(expression.root(), variables);
}

bool BooleanEvaluator::evaluate(const ExpressionNode& root,
                                const AssignmentMap& variables) const
{
    switch (root.kind())
    {
    case ExpressionNodeKind::Variable:
        {
            const auto variable = variables.find(root.variableName());
            if (variable == variables.end())
            {
                throw EvaluationException("Missing assignment for variable '" +
                                          root.variableName() + "'");
            }

            return variable->second;
        }

    case ExpressionNodeKind::UnaryOperation:
        {
            if (!root.operation().has_value())
            {
                throw missingOperator(root.kind());
            }

            if (root.operation().value() != BooleanOperator::Not)
            {
                throw EvaluationException("Unsupported unary operator " +
                                          std::string(operatorName(root.operation().value())));
            }

            if (root.right() == nullptr)
            {
                throw EvaluationException("Unary NOT node is missing an operand");
            }

            return !evaluate(*root.right(), variables);
        }

    case ExpressionNodeKind::BinaryOperation:
        {
            if (!root.operation().has_value())
            {
                throw missingOperator(root.kind());
            }

            if (root.left() == nullptr || root.right() == nullptr)
            {
                throw EvaluationException("Binary " +
                                          std::string(operatorName(root.operation().value())) +
                                          " node is missing a left or right operand");
            }

            switch (root.operation().value())
            {
            case BooleanOperator::And:
                return evaluate(*root.left(), variables) && evaluate(*root.right(), variables);
            case BooleanOperator::Or:
                return evaluate(*root.left(), variables) || evaluate(*root.right(), variables);
            case BooleanOperator::Not:
                throw EvaluationException("Unsupported binary operator NOT");
            }

            throw EvaluationException("Unsupported binary operator");
        }
    }

    throw EvaluationException("Unsupported expression node kind");
}

} // namespace BooleanEngine
