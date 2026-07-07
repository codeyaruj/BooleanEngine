#pragma once

#include "Core/Types.hpp"

#include <string>

namespace BooleanEngine
{

/**
 * @brief Abstract base interface for all parsers that produce BooleanFunction.
 */
class Parser
{
public:
    virtual ~Parser() = default;

    /**
     * @brief Parses input and returns a normalized BooleanFunction.
     */
    [[nodiscard]] virtual BooleanFunction parse(const std::string& input) = 0;

    /**
     * @brief Validates input without returning a parsed value.
     */
    virtual void validate(const std::string& input) const = 0;

    /**
     * @brief Clears parser-specific state between parse operations.
     */
    virtual void reset() = 0;
};

} // namespace BooleanEngine
