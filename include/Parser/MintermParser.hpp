#pragma once

#include "Core/Types.hpp"
#include "Parser/Token.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace BooleanEngine
{

/**
 * @brief Parses canonical sum-of-minterms notation into a BooleanFunction.
 *
 * Supported grammar:
 *
 * mintermSpecification -> Sigma 'm' '(' indexList? ')'
 *                         (OR 'd' '(' indexList? ')')? End
 * indexList            -> DecimalNumber (Comma DecimalNumber)*
 *
 * The variable count is supplied by the caller because minterm notation alone
 * is ambiguous, especially for empty or sparse minterm lists.
 */
class MintermParser final
{
public:
    /**
     * @brief Parses an existing token stream produced by Tokenizer.
     *
     * @throws ParserException for malformed notation or invalid indices.
     */
    [[nodiscard]] BooleanFunction parse(const std::vector<Token>& tokens,
                                        std::size_t variableCount) const;

    /**
     * @brief Tokenizes source text, then parses the resulting token stream.
     *
     * This overload delegates all lexical analysis to Tokenizer.
     */
    [[nodiscard]] BooleanFunction parse(const std::string& input,
                                        std::size_t variableCount) const;

private:
    class ParseState
    {
    public:
        explicit ParseState(const std::vector<Token>& tokens) noexcept;

        [[nodiscard]] const Token& current() const;
        [[nodiscard]] const Token& previous() const;
        [[nodiscard]] bool isAtEnd() const noexcept;
        [[nodiscard]] bool check(TokenType type) const noexcept;
        bool match(TokenType type);
        const Token& consume(TokenType type, const std::string& message);
        [[nodiscard]] std::size_t position() const noexcept;

    private:
        const std::vector<Token>& tokens_;
        std::size_t currentIndex_ = 0;
    };

    [[nodiscard]] std::vector<int> parseMintermClause(ParseState& state,
                                                      std::uint64_t maxIndex,
                                                      std::size_t variableCount) const;
    [[nodiscard]] std::vector<int> parseDontCareClause(ParseState& state,
                                                       std::uint64_t maxIndex,
                                                       std::size_t variableCount) const;
    [[nodiscard]] std::vector<int> parseParenthesizedIndexList(ParseState& state,
                                                               const std::string& listName,
                                                               std::uint64_t maxIndex,
                                                               std::size_t variableCount) const;
    [[nodiscard]] int parseIndex(ParseState& state,
                                 const std::string& listName,
                                 std::uint64_t maxIndex,
                                 std::size_t variableCount) const;
    void consumeIdentifier(ParseState& state,
                           const std::string& expected,
                           const std::string& message) const;
};

} // namespace BooleanEngine
