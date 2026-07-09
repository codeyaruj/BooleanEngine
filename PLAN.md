# BooleanEngine Plan

Last updated: 2026-07-09

This file is the running project plan. Update it after every major change with:

- Completed work
- Remaining work
- Decisions made
- Known issues

## Status Checklist

- [x] Project Setup
- [x] CMake
- [x] Core
- [x] Utilities
- [x] GrayCode
- [x] Graph
- [x] Hypercube
- [x] KarnaughMap
- [x] Parser Infrastructure
- [x] Boolean Expression Parser
- [x] Minterm Parser
- [x] Truth Table Parser
- [x] Boolean Evaluation Core
- [ ] Boolean Evaluation Engine Completion
- [ ] Group Detection
- [ ] Group Expansion
- [ ] Group Reduction
- [ ] Prime Implicant Generation
- [ ] Essential Prime Implicants
- [ ] Petrick's Method
- [ ] Expression Generation
- [ ] JSON Exporter
- [ ] Integration Layer
- [ ] Final Polish & Optimization

## Completed

### Project Setup

- Established a modular C++17 repository layout under `include/`, `src/`, and `tests/`.
- Added top-level CMake configuration with warnings enabled.
- Added `.gitignore` coverage for build artifacts and generated CMake files.

### Core

- Added shared core types and exceptions.
- Updated supported Boolean function variable range to 2 through 4 variables.
- Extended `BooleanCell` with `variableAssignment` metadata for K-map use.

### Utilities

- Implemented binary conversion utilities.
- Implemented bit utilities including popcount, bit operations, and Hamming distance.
- Implemented math helpers.
- Hardened utility edge cases:
  - invalid binary widths now throw
  - invalid binary strings now throw
  - invalid bit positions now throw
  - unsafe powers of two now throw

### GrayCode

- Implemented binary-to-Gray conversion.
- Implemented Gray-to-binary conversion.
- Implemented reflected Gray-code sequence generation.
- Implemented Gray-code adjacency checks.
- Added bounds checking for oversized Gray-code sequences.

### Graph

- Implemented generic undirected graph module.
- Added `Vertex`, `Edge`, and `Graph`.
- Added vertex and edge insertion/removal/query APIs.
- Added adjacency-list storage.
- Added BFS, DFS, and connected component detection.
- Added `Graph::getVertex()` for safe read-only vertex lookup.
- Added comprehensive graph tests.

### Hypercube

- Implemented generic n-dimensional hypercube generation using Graph.
- Generates `2^n` vertices.
- Connects vertices whose Hamming distance is one.
- Uses bit-flipping neighbor generation instead of pairwise comparison.
- Added Hypercube wrapper APIs for dimension, counts, graph access, vertex lookup, and neighbors.
- Added tests for dimensions 0 through 5 while keeping Hypercube generic.

### KarnaughMap

- Implemented K-map layout generation for 2, 3, and 4 variables.
- Enforced project K-map scope of 2 through 4 variables.
- Generated standard Gray-code row/column layouts:
  - 2-variable: 2 x 2
  - 3-variable: 2 x 4
  - 4-variable: 4 x 4
- Added minterm, row/column, and cell-index lookup.
- Added wrap-around neighbor relationships.
- Added construction helpers:
  - blank map
  - from minterms
  - from maxterms
  - from truth table
  - with don't-cares
- Added cell metadata:
  - binary index
  - Gray index
  - row
  - column
  - value
  - don't-care flag
  - variable assignment
- Added K-map group detection using subcube coverage.
- Added prime implicant detection.
- Added essential prime implicant detection.
- Added SOP simplification.
- Added ASCII K-map rendering.
- Added tests for layout, lookup, wrap-around, grouping, simplification, don't-cares, rendering, and invalid inputs.

### Parser Infrastructure

- Added shared token definitions with `TokenType` and `Token`.
- Added `Tokenizer` for lexical analysis only.
- Added support for variables, NOT, AND, OR, parentheses, commas, Sigma notation, decimal numbers, and end-of-input tokens.
- Added whitespace skipping and invalid-character rejection.
- Added parenthesis balance validation during tokenization.
- Added abstract `Parser` interface that future parsers can derive from.
- Added reusable parser utilities:
  - whitespace detection
  - Boolean variable validation
  - identifier validation
  - decimal number parsing
  - invalid character validation
  - parenthesis balance checks
  - duplicate variable detection
  - token variable extraction
  - unique variable counting
- Added parser infrastructure tests for tokens, tokenizer examples, parser utilities, edge cases, and parser base behavior.

### Boolean Expression Parser

- Added Boolean expression AST node kinds for variables, unary operations, and binary operations.
- Added Boolean operator enum for NOT, AND, and OR.
- Added `ExpressionNode` factory functions to preserve AST invariants.
- Added `ParsedBooleanExpression` as an AST-owning wrapper without reusing the existing Core `BooleanExpression` SOP/POS struct.
- Added deterministic AST serialization for tests and debugging.
- Added `BooleanExpressionParser` using recursive descent over the existing token stream.
- Parser supports:
  - single-letter variables `A` through `D`
  - prefix NOT with `!` and `~`
  - postfix NOT via apostrophe
  - AND via tokenizer-supported AND tokens
  - OR via tokenizer-supported OR tokens
  - parentheses
- Parser intentionally does not support XOR or constants because the tokenizer does not define tokens for them.
- Parser performs parsing only. It does not evaluate expressions, generate truth tables, produce minterms, simplify expressions, build K-maps, or export JSON.
- Added `BooleanExpressionParserTest` covering AST structure, precedence, associativity, nested parentheses, malformed expressions, and tokenizer-to-parser integration.

### Minterm Parser

- Added `MintermParser` for canonical sum-of-minterms notation over the existing tokenizer output.
- Added token-stream and source-string parse APIs that require the caller to provide `variableCount`.
- Accepted notation:
  - `Σm()`
  - `Σm(0)`
  - `Σm(1,3,7)`
  - `Σm(1,3,7) + d(0,2)`
- The source-string overload delegates lexical analysis to `Tokenizer`; it does not scan input manually.
- The tokenizer represents `Σm` as separate `Sigma` and `Variable("m")` tokens.
- Don't-care notation is represented as `Variable("d")` followed by a parenthesized index list.
- Empty minterm lists and empty don't-care lists are accepted.
- Minterm and don't-care indices are normalized into ascending order in `BooleanFunction`.
- Duplicate minterms, duplicate don't-cares, and minterm/don't-care overlaps throw `ParserException`.
- Index range validation uses checked `uint64_t` arithmetic and rejects values outside `[0, 2^variableCount - 1]`.
- The parser does not enforce the KarnaughMap 2-through-4-variable limit; it supports the range that fits the Core `BooleanFunction` index storage.
- Result construction directly fills `BooleanFunction::variableCount`, `BooleanFunction::minterms`, and `BooleanFunction::dontCares`.
- Complexity is `O(t + k log k)` time because output indices are sorted, with `O(k)` auxiliary storage.
- Added `MintermParserTest` covering valid notation, tokenizer integration, manual token streams, malformed syntax, duplicates, overlaps, range errors, variable-count errors, reusable parser objects, and result lifetime.

### Truth Table Parser

- Added `TruthTableParser` for complete line-oriented truth-table notation.
- Accepted syntax:
  - header: `A B | F`
  - rows: `0 1 | 1`
  - don't-care outputs: `X` or `x`
- The parser infers `variableCount` from the header input variables.
- Header identifiers use the existing parser utility identifier rule: one alphabetic character.
- The parser accepts blank lines and flexible whitespace, but does not support comments.
- Rows may appear in any order.
- Complete truth tables are required: exactly `2^variableCount` unique input assignments must be present.
- Duplicate input assignments throw `ParserException`.
- Input values must be `0` or `1`.
- Output values must be `0`, `1`, `X`, or `x`.
- Output `1` becomes a minterm; output `X` or `x` becomes a don't-care; output `0` is omitted from `BooleanFunction`.
- Minterms and don't-cares are normalized into ascending order.
- Domain-size validation uses checked `uint64_t` arithmetic and rejects variable counts outside the Core `BooleanFunction` index range.
- The parser does not enforce the KarnaughMap 2-through-4-variable limit.
- Complexity is `O(t + r + k log k)` time, with `O(r + k)` auxiliary storage.
- Added `TruthTableParserTest` covering valid tables, don't-cares, row order, whitespace, 5-variable support, malformed headers, malformed rows, duplicate assignments, missing rows, invalid values, invalid identifiers, and oversized variable counts.

### Boolean Evaluation Core

- Added `BooleanEvaluator` for read-only evaluation of existing Boolean expression ASTs.
- Added `EvaluationException` under the shared Core exception hierarchy for runtime evaluation failures.
- Public evaluator APIs:
  - evaluate a `ParsedBooleanExpression`
  - evaluate an `ExpressionNode`
- Supported AST behavior:
  - `ExpressionNodeKind::Variable` looks up the variable name in the assignment map.
  - `BooleanOperator::Not` negates its operand.
  - `BooleanOperator::And` evaluates with natural short-circuit behavior.
  - `BooleanOperator::Or` evaluates with natural short-circuit behavior.
- Prefix and postfix NOT both evaluate through the same normalized `BooleanOperator::Not` AST operator.
- Missing variable assignments throw `EvaluationException`.
- Empty parsed expressions throw `EvaluationException`.
- The evaluator does not parse source text, generate truth tables, produce minterms, build K-maps, simplify, or export JSON.
- Complexity is `O(m)` time in the number of visited AST nodes, with `O(h)` recursion stack space for AST height. Short-circuiting can visit fewer than `m` nodes.
- Added `BooleanEvaluatorTest` covering variables, NOT, AND, OR, precedence, parentheses, mixed expressions, direct AST evaluation, reusable evaluator objects, short-circuiting, missing assignments, and empty parsed expressions.

### CMake

- Registered Graph, Hypercube, and KarnaughMap sources in the main executable target.
- Registered unit test targets for Utilities, GrayCode, Graph, Hypercube, and KarnaughMap.
- Included existing placeholder source files in the main target so CMake compiles every current source file.
- Registered Parser infrastructure sources in the main executable target.
- Added `ParserInfrastructureTest`.
- Registered Boolean expression parser sources in the main executable target.
- Added `BooleanExpressionParserTest`.
- Registered Minterm parser sources in the main executable target.
- Added `MintermParserTest`.
- Registered Truth Table parser sources in the main executable target.
- Added `TruthTableParserTest`.
- Registered Boolean evaluator sources in the main executable target.
- Added `BooleanEvaluatorTest`.

### Verification

- Preflight before Boolean Evaluation Core implementation:
  - `git status`
  - `git log --oneline -5`
  - `cmake --build build`
  - `ctest --test-dir build --output-on-failure`
  - Result: working tree was clean, current history included `3054f8e`, and 9 of 9 existing tests passed.
- Preflight before Truth Table Parser implementation:
  - `git status`
  - `git diff --check`
  - `cmake --build build`
  - `ctest --test-dir build --output-on-failure`
  - Result: working tree was clean, Minterm Parser was already committed as `f33ac94`, and 8 of 8 existing tests passed.
- Preflight before Minterm Parser implementation:
  - `cmake --build build`
  - `ctest --test-dir build --output-on-failure`
  - Result: 7 of 7 existing tests passed.
- Clean out-of-tree configure/build/test completed successfully.
- Latest verification command:
  - `cmake -S . -B /private/tmp/booleanengine-evaluator-build.N0Ih0H`
  - `cmake --build /private/tmp/booleanengine-evaluator-build.N0Ih0H`
  - `ctest --test-dir /private/tmp/booleanengine-evaluator-build.N0Ih0H --output-on-failure`
- Latest result: 10 of 10 tests passed.

## Remaining

### Boolean Evaluation Engine Completion

- Generate truth tables from expressions.
- Convert parsed expressions to `BooleanFunction`.
- Cross-check expression evaluation against K-map construction.
- Add variable collection from ASTs.

### Group Detection

- Decide whether to keep K-map-owned group detection or move it into the planned Grouping module.
- If moved, preserve KMap APIs as wrappers or adapters.
- Add more exhaustive tests for all legal group shapes.

### Group Expansion

- Implement expansion from seed cells into maximal valid groups.
- Support wrap-around expansion.

### Group Reduction

- Remove redundant groups.
- Preserve groups needed for essential coverage.

### Prime Implicant Generation

- Decide whether to keep prime implicant logic in KMap or move it into `PrimeImplicants`.
- Add module-level API and tests if moved.

### Essential Prime Implicants

- Complete module-level essential prime implicant API if logic is moved out of KMap.
- Add tests for multiple-cover and single-cover minterms.

### Petrick's Method

- Implement Petrick's Method for cases where essential prime implicants do not fully cover all minterms.
- Add tests for tie-breaking and equivalent minimal covers.

### Expression Generation

- Generate clean minimized SOP expressions from selected implicants.
- Define variable naming conventions.
- Add tests for constants, single literals, complements, and multi-term SOP.

### JSON Exporter

- Define JSON schema for maps, groups, implicants, and expressions.
- Implement exporter with escaping and deterministic ordering.
- Add tests for valid JSON output.

### Integration Layer

- Connect parsers, KMap, grouping/implicant selection, simplification, and export.
- Define a stable high-level API for users.

### Final Polish & Optimization

- Review const correctness, exception messages, and header hygiene.
- Add more tests for malicious or malformed input.
- Consider replacing ad hoc assert tests with a test framework if the project grows.
- Revisit performance only after module boundaries stabilize.

## Decisions Made

- K-map support is restricted to 2, 3, and 4 variables.
- Hypercube remains generic and is not restricted to K-map limits.
- Graph remains generic and contains no Boolean, hypercube, or K-map logic.
- K-map layout uses Gray-code row and column ordering.
- K-map wrap-around neighbors are stored as cell indices in `BooleanCell::neighbors`.
- Public `KarnaughMap::neighbors(minterm)` returns neighboring minterms.
- K-map group detection currently uses hypercube/subcube semantics to naturally support wrap-around groups.
- K-map grouping, prime implicants, essential prime implicants, and SOP simplification currently live in `KarnaughMap`.
- Later dedicated modules may take over grouping, prime implicants, Petrick's Method, and expression generation.
- Parser infrastructure is intentionally lexical/framework-only and does not parse expressions or minterm semantics yet.
- Tokenizer emits adjacent variables as separate tokens so future expression parsing can handle implicit AND.
- Sigma notation is tokenized but not semantically parsed yet.
- Boolean expression parsing uses recursive descent with precedence `NOT > AND > OR`.
- Binary AND and OR are left-associative.
- Prefix NOT is right-associative; postfix apostrophe is parsed as unary NOT on the preceding primary expression.
- XOR and Boolean constants remain unsupported until tokenizer support is added intentionally.
- Minterm notation receives `variableCount` from the caller because the notation cannot infer it unambiguously.
- Minterm parsing remains standalone instead of deriving from `Parser`, because the base parser API has no variable-count parameter.
- Minterm parsing uses `TokenType::Or` for the optional don't-care separator, so tokenizer-supported OR lexemes are accepted there.
- Minterm parser output uses ascending canonical order because listed minterm order has no Boolean meaning.
- Minterm parser bounds are based on the Core `BooleanFunction` integer storage, not KarnaughMap's 2-through-4-variable scope.
- Truth-table parsing uses line-oriented row handling because the shared tokenizer intentionally skips whitespace and does not preserve newlines.
- Truth-table parsing requires complete tables by default to avoid ambiguous missing assignments.
- Truth-table variable count is inferred from the header rather than supplied separately.
- Truth-table parser output uses ascending canonical order to match the Minterm Parser.
- Truth-table parser bounds are based on Core `BooleanFunction` integer storage, not KarnaughMap's dimensional scope.
- Boolean evaluation is AST-only and intentionally does not parse source text.
- Boolean evaluation uses `EvaluationException` for missing assignments, empty parsed expressions, and malformed AST state.
- Boolean evaluation depends only on Core and Expression; parser integration is used only in tests.
- Existing placeholder modules were not rewritten without a clear stable API.

## Known Issues

- Grouping, PrimeImplicants, Simplifier, Exporter, and several related test files are still placeholders.
- The Boolean expression parser builds an AST only; expression truth-table generation is still pending.
- Boolean AST evaluation for concrete assignments is implemented, but expression truth-table generation is still pending.
- The Minterm Parser creates `BooleanFunction` data only; it does not evaluate expressions, build truth tables, build K-maps, or simplify logic.
- The Truth Table Parser creates `BooleanFunction` data only; it does not evaluate expressions, build K-maps, or simplify logic.
- Truth Table Parser comments are not supported; non-empty stray lines are treated as malformed input.
- K-map simplification currently performs a small exhaustive cover search suitable for 2 through 4 variables, not a full standalone Petrick module.
- K-map group/implicant APIs return minterm sets, not rich implicant objects.
- ASCII rendering is intentionally simple and not a final visualization layer.
- Existing tests are assert-based executables rather than a full-featured unit test framework.
- The main executable is still a GrayCode-oriented demo and is not an integration CLI.
