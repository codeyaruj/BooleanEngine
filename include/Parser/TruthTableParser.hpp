#pragma once

#include "Core/Types.hpp"

#include <string_view>

namespace BooleanEngine
{

/**
 * @brief Parses complete textual truth tables into BooleanFunction values.
 *
 * Supported syntax:
 *
 * A B | F
 * 0 0 | 0
 * 0 1 | 1
 * 1 0 | X
 * 1 1 | 0
 *
 * The header defines the variable count. Each non-empty row must provide one
 * binary input assignment and one output value. Output X or x marks a
 * don't-care state.
 */
class TruthTableParser final
{
public:
    /**
     * @brief Parses a complete truth table.
     *
     * @throws ParserException for malformed headers, rows, duplicates, missing
     * assignments, or unrepresentable domain sizes.
     */
    [[nodiscard]] BooleanFunction parse(std::string_view source) const;
};

} // namespace BooleanEngine
