#include "Evaluation/TruthTableGenerator.hpp"

#include "Core/Exceptions.hpp"
#include "Expression/ExpressionUtilities.hpp"

#include <cstddef>
#include <sstream>
#include <utility>

namespace BooleanEngine
{
namespace
{

constexpr std::size_t MaximumRowCount = 65'536U;

std::size_t checkedRowCount(std::size_t variableCount)
{
    std::size_t rowCount = 1U;

    for (std::size_t index = 0; index < variableCount; ++index)
    {
        if (rowCount > MaximumRowCount / 2U)
        {
            std::ostringstream message;
            message << "Cannot generate truth table for " << variableCount
                    << " variables: row count exceeds supported limit of "
                    << MaximumRowCount;
            throw EvaluationException(message.str());
        }

        rowCount *= 2U;
    }

    return rowCount;
}

} // namespace

GeneratedTruthTable TruthTableGenerator::generate(
    const ParsedBooleanExpression& expression) const
{
    if (expression.empty())
    {
        throw EvaluationException("Cannot generate a truth table from an empty expression");
    }

    return generate(expression.root());
}

GeneratedTruthTable TruthTableGenerator::generate(const ExpressionNode& root) const
{
    GeneratedTruthTable table;
    table.variables = collectVariables(root);

    const std::size_t variableCount = table.variables.size();
    const std::size_t rowCount = checkedRowCount(variableCount);
    table.rows.reserve(rowCount);

    const BooleanEvaluator evaluator;

    for (std::size_t rowIndex = 0; rowIndex < rowCount; ++rowIndex)
    {
        BooleanEvaluator::AssignmentMap assignment;
        assignment.reserve(variableCount);

        for (std::size_t variableIndex = 0; variableIndex < variableCount;
             ++variableIndex)
        {
            const std::size_t bitIndex = variableCount - 1U - variableIndex;
            const bool value = ((rowIndex >> bitIndex) & 1U) != 0U;
            assignment.emplace(table.variables[variableIndex], value);
        }

        const bool output = evaluator.evaluate(root, assignment);
        table.rows.push_back(TruthTableRow{std::move(assignment), output});
    }

    return table;
}

} // namespace BooleanEngine
