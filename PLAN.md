# BooleanEngine Plan

Last updated: 2026-07-07

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
- [ ] Boolean Expression Parser
- [ ] Minterm & Truth Table Parsers
- [ ] Boolean Evaluation Engine
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

### CMake

- Registered Graph, Hypercube, and KarnaughMap sources in the main executable target.
- Registered unit test targets for Utilities, GrayCode, Graph, Hypercube, and KarnaughMap.
- Included existing placeholder source files in the main target so CMake compiles every current source file.
- Registered Parser infrastructure sources in the main executable target.
- Added `ParserInfrastructureTest`.

### Verification

- Clean out-of-tree configure/build/test completed successfully.
- Latest verification command:
  - `cmake -S . -B /private/tmp/booleanengine-parser-build.1cYKDH`
  - `cmake --build /private/tmp/booleanengine-parser-build.1cYKDH`
  - `ctest --test-dir /private/tmp/booleanengine-parser-build.1cYKDH --output-on-failure`
- Latest result: 6 of 6 tests passed.

## Remaining

### Boolean Expression Parser

- Parse Boolean expressions into an AST or equivalent internal form.
- Support operators and precedence consistently.
- Reject malformed expressions with informative exceptions.

### Minterm & Truth Table Parsers

- Parse minterm lists.
- Parse maxterm lists.
- Parse don't-care lists.
- Parse truth tables.
- Validate duplicates, overlaps, and out-of-range terms.

### Boolean Evaluation Engine

- Evaluate parsed Boolean expressions for assignments.
- Generate truth tables from expressions.
- Cross-check expression evaluation against K-map construction.

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
- Existing placeholder modules were not rewritten without a clear stable API.

## Known Issues

- Boolean expression parsing, minterm/truth-table semantic parsing, Expression, Grouping, PrimeImplicants, Simplifier, Exporter, and several related test files are still placeholders.
- K-map simplification currently performs a small exhaustive cover search suitable for 2 through 4 variables, not a full standalone Petrick module.
- K-map group/implicant APIs return minterm sets, not rich implicant objects.
- ASCII rendering is intentionally simple and not a final visualization layer.
- Existing tests are assert-based executables rather than a full-featured unit test framework.
- The main executable is still a GrayCode-oriented demo and is not an integration CLI.
