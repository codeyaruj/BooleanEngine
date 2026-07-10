#pragma once

#include "Evaluation/BooleanEvaluator.hpp"
#include "Expression/AST.hpp"
#include "Expression/Expression.hpp"

#include <string>
#include <vector>

namespace BooleanEngine
{

/**
 * @brief One fully owned assignment and its evaluated Boolean output.
 */
struct TruthTableRow
{
    BooleanEvaluator::AssignmentMap assignment;
    bool output = false;
};

/**
 * @brief Complete generated truth table with deterministic variable ordering.
 */
struct GeneratedTruthTable
{
    std::vector<std::string> variables;
    std::vector<TruthTableRow> rows;
};

/**
 * @brief Evaluates an expression over every assignment in its Boolean domain.
 *
 * Variables are sorted lexicographically. Rows use binary-counting order with
 * the first variable as the most significant bit. To bound materialization,
 * generation supports at most 65,536 rows (16 unique variables).
 */
class TruthTableGenerator final
{
public:
    /**
     * @brief Generates a complete table from an AST-owning expression.
     *
     * @throws EvaluationException when the expression is empty, its domain is
     * too large to materialize, or evaluation fails.
     */
    [[nodiscard]] GeneratedTruthTable generate(
        const ParsedBooleanExpression& expression) const;

    /**
     * @brief Generates a complete table from an expression tree root.
     *
     * @throws EvaluationException when the expression domain is too large to
     * materialize or evaluation fails.
     * @throws ParserException when structural variable collection fails.
     */
    [[nodiscard]] GeneratedTruthTable generate(const ExpressionNode& root) const;
};

} // namespace BooleanEngine
