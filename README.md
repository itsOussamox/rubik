# Rubik FMC Solver – Starter Plan

## 1. Project vision and success criteria
- Accept any valid 3×3×3 scramble sequence in WCA face notation plus modifiers (`, 2).
- Produce a solution sequence measured in Half-Turn Metric (HTM) that actually solves the scrambled cube.
- Prefer solutions ≤ 25 HTM when possible; always verify by reapplying moves.
- Provide clear CLI ergonomics, deterministic behavior, and robust error reporting.

## 2. Incremental milestones
1. **Core infrastructure**
   - Implement cube state representations (facelets + cubie permutations).
   - Support move application, inversion, and composition.
   - Add scramble parser, validator, and canonical formatter.
2. **Search scaffolding**
   - Build move tables and pruning heuristics for Kociemba-style two-phase search.
   - Implement Phase 1 reducer (to subgroup H) with IDDFS and admissible heuristics.
   - Implement Phase 2 search and solution assembly with verification.
3. **Optimization and quality**
   - Symmetry reductions, better heuristics, and memoized pruning to tighten move counts.
   - Solution simplifier (e.g., cancellation, commutator insertions).
   - Benchmark harness plus regression suite.
4. **Bonus features (time-permitting)**
   - Multiple algorithm portfolios (e.g., Thistlethwaite fallback, optimal IDA* mode).
   - Scramble generator, statistics output, or visualization frontend.

## 3. Technical architecture preview
- `include/` – public headers (Cube, Solver, Notation, Tables, etc.).
- `src/` – implementation files organized by subsystem.
- `resources/` – optional precomputed pruning tables (.bin) with version tags.
- `tests/` – unit and integration tests built via CTest/GoogleTest.
- `docs/` – documentation (subject summary, developer notes).

Key components:
- **CubeModel**: Represents cube state (corners, edges, orientation). Responsible for move application and hashing.
- **Notation**: Parsing, canonical formatting, HTM counting, random scramble generation.
- **Search::Phase1 / Phase2**: Two-phase solver engines exposing clear configuration options (depth limits, heuristic toggles).
- **SolverFacade**: High-level orchestrator used by CLI to run search, handle timeouts, and produce human-readable output.

## 4. Quality gates and tooling
- Compiler: `c++` with `-std=c++11 -Wall -Wextra -Wpedantic -O2`.
- Static analysis: optional `clang-tidy` profiles; treat warnings as actionable.
- Tests: unit (GoogleTest) + integration (golden scramble/solution pairs); run via `ctest`.
- Continuous integration idea: GitHub Actions matrix covering Windows/Linux builds.

## 5. Immediate next steps
1. Flesh out Cube data structures and move tables.
2. Implement placeholder solver pipeline returning `NotImplemented` status for now.
3. Write CLI to accept scramble, validate, and print forthcoming status messages.
4. Bootstrap unit-test harness for parser and cube manipulations.
5. Start Phase 1 heuristic generation scripts.

## 6. Risk register
- **Combinatorial explosion**: Mitigate via pruning tables and symmetry reductions.
- **Table generation time**: Cache binary snapshots; ship reseeding script.
- **Validation complexity**: Create deterministic replay tests ensuring solutions are correct.
- **Performance portability**: Keep portable C++11 code, avoid platform-specific APIs.

## 7. Documentation expectations
- Update README with usage, notation primer, and troubleshooting.
- Provide developer guide for table regeneration and algorithm explanations.
- Maintain changelog capturing solver improvements and metrics.
