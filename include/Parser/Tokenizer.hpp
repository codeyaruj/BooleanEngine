#pragma once

#include "Parser/Token.hpp"

#include <string>
#include <vector>

namespace BooleanEngine
{

/**
 * @brief Converts parser input text into a token stream.
 *
 * The tokenizer performs lexical analysis only. It does not evaluate
 * expressions, build ASTs, or apply Boolean operator precedence.
 */
class Tokenizer
{
public:
    explicit Tokenizer(std::string input);

    /**
     * @brief Tokenizes the input supplied to the constructor.
     */
    [[nodiscard]] std::vector<Token> tokenize() const;

    /**
     * @brief Convenience tokenizer for one-shot use.
     */
    [[nodiscard]] static std::vector<Token> tokenize(const std::string& input);

private:
    std::string input_;
};

} // namespace BooleanEngine
