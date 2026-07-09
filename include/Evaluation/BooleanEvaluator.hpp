#pragma once

#include "Expression/AST.hpp"
#include "Expression/Expression.hpp"

#include <string>
#include <unordered_map>

namespace BooleanEngine
{

/**
 * @brief Evaluates parsed Boolean expression ASTs for concrete assignments.
 *
 * The evaluator does not own, parse, simplify, or mutate expressions. It only
 * traverses an existing AST and reads variable values from the provided
 * assignment map.
 */
class BooleanEvaluator final
{
public:
    using AssignmentMap = std::unordered_map<std::string, bool>;

    /**
     * @brief Evaluates a parsed expression wrapper.
     *
     * @throws EvaluationException when the parsed expression is empty or cannot
     * be evaluated for the provided assignments.
     */
    [[nodiscard]] bool evaluate(const ParsedBooleanExpression& expression,
                                const AssignmentMap& variables) const;

    /**
     * @brief Evaluates an expression tree root.
     *
     * @throws EvaluationException when an assignment is missing or the AST node
     * state is invalid.
     */
    [[nodiscard]] bool evaluate(const ExpressionNode& root,
                                const AssignmentMap& variables) const;
};

} // namespace BooleanEngine
