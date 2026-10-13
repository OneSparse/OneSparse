# Unit Testing Framework Plan

## Goal

Extend OneSparse's existing documentation-oriented PostgreSQL regression tests
with a focused semantic unit-test suite. The new suite must complement, not
replace, the existing doctest workflow.

The current doctests remain valuable because executable SQL examples become
published documentation. The new suite will provide concise, assertion-based
checks that make failures easier to diagnose and avoid coupling core behavior
tests to psql formatting, HTML, SVG, or Graphviz output.

## Current State

The active test path is PostgreSQL's PGXS regression framework:

```text
templates/tests/*.sql
  -> generate.py
  -> sql/*.sql
  -> make installcheck / pg_regress
  -> expected/*.out
  -> doctestify.py
  -> docs/*.md
```

`Makefile` automatically includes every top-level `sql/*.sql` file in
`REGRESS`. The complete output of those scripts is checked against golden
files in `expected/`, and those golden files are also source material for the
documentation site.

This system provides broad executable documentation coverage, but many output
files contain unstable details such as:

- psql Unicode tables, wrapping, and notices;
- Graphviz SVG coordinates, paths, fonts, and identifiers;
- HTML fragments used by the documentation layout;
- gnuplot and graph-tool output;
- GraphBLAS diagnostic text and formatted object printing.

The repository also has an inactive pgTAP prototype in `tests/`. It is not
part of the Makefile, test script, or CI, and it is not currently runnable:

- pgTAP is not installed in `Dockerfile-debug`;
- the entry script refers to `pggraphblas` rather than `onesparse`;
- `init.sql` includes misspelled `scalor.sql`;
- hard-coded test plans would drift as coverage changes.

The prototype demonstrates that pgTAP is an appropriate assertion style, but
it should not be adopted in its current form.

## Recommended Framework

Add a dedicated pgTAP semantic test suite outside `sql/`:

```text
test/unit/sql/*.sql
  -> pg_prove / pgTAP
  -> TAP assertion diagnostics
```

The suite will exercise the public SQL extension API inside a real PostgreSQL
backend. This covers SQL bindings, PostgreSQL `Datum` conversion, expanded
objects, memory contexts, GraphBLAS initialization, serialization, and error
handling without requiring a separate C test harness.

A standalone C unit-test binary is not recommended initially. The extension's
implementation is tightly coupled to PostgreSQL backend allocation, error,
and expanded-object infrastructure, making server-backed tests substantially
more practical for public behavior.

## Test Boundary

The test suites have separate responsibilities.

| Suite | Location | Purpose | Output contract |
| --- | --- | --- | --- |
| Documentation regression | `sql/`, `expected/`, `templates/tests/` | Executable examples and rendered documentation | Full psql transcript snapshots |
| Semantic unit tests | `test/unit/` | Focused behavioral and error-contract checks | pgTAP assertions |

Semantic unit tests must not be placed in `sql/`, `templates/tests/`, or
`expected/`. Doing so would accidentally include them in the documentation
pipeline because of the current wildcard `REGRESS` configuration.

## Proposed Skeleton

```text
test/
  unit/
    README.md
    run.sh
    lib/
      onesparse_unit.sql
    sql/
      00_setup.sql
      10_scalar.sql
      20_vector.sql
      30_matrix.sql
      40_descriptors.sql
      50_io.sql
      60_graph.sql
      70_errors.sql
```

### `test/unit/README.md`

Document how to run the suite, the suite boundary, and conventions for adding
tests. It should explicitly state that doctests test documentation output,
whereas this suite tests semantic behavior.

### `test/unit/run.sh`

Run the ordered SQL files through `pg_prove`, using the connection parameters
already supplied by the test container. It should fail on the first unavailable
dependency or assertion failure and must not regenerate documentation or golden
files.

### `test/unit/sql/00_setup.sql`

Create `onesparse` and `pgtap` if required, set a predictable `search_path`,
and establish any deterministic session settings. Setup must avoid persistent
state that can affect doctests or another unit-test file.

### `test/unit/lib/onesparse_unit.sql`

Provide the initial shared helper skeleton. The first implementation should
only define the framework's intended assertion patterns; behavioral assertions
will be added later.

Future helpers should support:

- vector equality based on size, type, and sorted `(index, value)` elements;
- matrix equality based on dimensions, type, and sorted `(row, col, value)`
  elements;
- approximate scalar/vector comparisons with explicit NaN and infinity rules;
- connected-component partition checks that do not depend on arbitrary label
  values.

### Domain Files

Each domain file should be independently executable and use this shape:

```sql
BEGIN;
SELECT plan(0); -- Replace when concrete assertions are introduced.

-- Domain-specific assertion sections will be added here.

SELECT * FROM finish();
ROLLBACK;
```

The skeleton should use a zero-test plan until real assertions are added,
rather than inventing placeholder passing tests.

## Assertion Conventions

Prefer concise semantic assertions over rendered output:

- `is(...)` for scalar values, dimensions, counts, and types;
- `ok(...)` for predicates and invariants;
- `results_eq(...)` for stable, explicitly ordered row sets;
- `throws_ok(...)` for expected failures, matching SQLSTATE and only stable,
  intentional message text;
- tolerance-aware helpers for floating-point operations and ranking results.

Avoid `print()`, terminal tables, HTML, SVG, Graphviz output, and exact
GraphBLAS diagnostic text as unit-test assertions.

## Initial Domain Scope

The skeleton establishes these empty sections. Actual tests will be added in a
later phase.

| File | Future coverage |
| --- | --- |
| `10_scalar.sql` | Constructors, casts, lifecycle, special floating-point values, and error contracts |
| `20_vector.sql` | Construction, sparse coordinates, element access/mutation, ewise operations, reductions, assignment, extraction |
| `30_matrix.sql` | Construction, sparse coordinates, element access/mutation, transpose, ewise operations, multiplication, assignment, extraction |
| `40_descriptors.sql` | Value and structural masks, complement, replace, and accumulators |
| `50_io.sql` | In-memory serialization, file round trips, large-object save/load, missing/corrupt input errors |
| `60_graph.sql` | BFS, SSSP, PageRank invariants, connected components, and centrality algorithms |
| `70_errors.sql` | Bounds, invalid literals/types, incompatible dimensions, unsupported operations, invalid algorithm inputs |

## Build And Container Integration

1. Install a pinned PostgreSQL-18-compatible pgTAP release in
   `Dockerfile-debug`.
   - Build pgTAP from a pinned source release rather than relying on an
     unverified distribution package version.
   - Do not add pgTAP to production images.

2. Add a `unitcheck` target to `Makefile`:

   ```make
   unitcheck:
	./test/unit/run.sh
   ```

3. Preserve the existing `installcheck` target for documentation regression.

4. Add a combined target:

   ```make
   check:
	$(MAKE) installcheck
	$(MAKE) unitcheck
   ```

5. Extend `test.sh` with suite selection while keeping no-argument behavior:

   ```text
   ./test.sh          # documentation regression and unit tests
   ./test.sh doctest  # existing pg_regress suite only
   ./test.sh unit     # pgTAP semantic suite only
   ```

6. Update CI so doctest and unit-test failures are reported as distinct named
   steps. Initially they can share the existing container setup; splitting into
   separate jobs can be evaluated later.

## Test Design Priorities

The first real tests should target high-value behavior that is currently
embedded in broad documentation snapshots:

1. Small deterministic scalar, vector, and matrix construction/mutation
   cases.
2. Descriptor and mask behavior, especially structural, complement, and
   replace semantics.
3. Serialization, file I/O, and PostgreSQL large-object round trips and error
   contracts.
4. Graph algorithms using hand-verifiable graphs:
   - BFS levels and parent invariants;
   - exact SSSP distances;
   - PageRank ordering, non-negativity, and tolerance-based sum/value checks;
   - connected-component partitions independent of label numbering.
5. Explicit negative tests for NULLs, bounds, malformed input, incompatible
   shapes, invalid descriptors, and unsupported operations.

## Rollout Constraints

- Do not alter the existing generated documentation pipeline as part of the
  skeleton work.
- Do not copy `results/*.out` into `expected/` during normal test execution;
  that operation updates documentation snapshots.
- Do not move new semantic tests into `sql/` while `REGRESS` is derived from
  `$(wildcard sql/*.sql)`.
- Keep tests deterministic, small, and isolated. Use transactions and cleanup
  so each file can run alone.
- Treat expected errors as deliberate contracts with explicit assertions,
  rather than incidental `ERROR:` lines inside large examples.

## Success Criteria For The Skeleton

The skeleton phase is complete when:

1. The debug image includes a pinned, usable pgTAP installation.
2. `make unitcheck` runs the new suite independently of `make installcheck`.
3. `./test.sh`, `./test.sh doctest`, and `./test.sh unit` select the intended
   suite(s).
4. CI reports semantic assertion failures separately from documentation
   regression failures.
5. The unit-test files, helper scaffold, and README exist but contain no
   product-behavior tests beyond pgTAP's required zero-test skeleton structure.
