# Code Coverage Plan

## Goal

Add source-level code coverage reporting for OneSparse so every test run shows
which code under `src/` was executed and which lines, functions, and branches
remain uncovered. The report should guide new PostgreSQL-level tests without
instrumenting third-party libraries or changing the production build.

Coverage is diagnostic rather than a release-quality gate initially. The first
rollout should establish a trustworthy baseline and make uncovered code easy
to inspect. A coverage threshold or ratchet can be considered after the
baseline is stable.

## Recommended Approach

Use GCC coverage instrumentation and `gcovr` against the extension shared
library. Run the existing SQL tests through a real PostgreSQL backend, then
stop PostgreSQL cleanly before collecting counters.

This approach measures the behavior that matters for a PostgreSQL extension:

- SQL function bindings and PostgreSQL `Datum` conversion;
- expanded-object and memory-context behavior;
- extension initialization and shared-library loading;
- GraphBLAS and LAGraph calls made by OneSparse;
- PostgreSQL error handling and transaction behavior;
- serialization, TOAST, SPI, and large-object paths exercised through SQL.

A standalone C test binary is not the primary approach because much of the
implementation depends on PostgreSQL backend infrastructure, including
`ereport`, memory contexts, expanded objects, FMGR calling conventions, and
SPI.

## Coverage Boundary

The coverage denominator should contain only OneSparse implementation files:

```text
src/**/*.c
```

Exclude the following from the report:

- PostgreSQL server source and libraries;
- SuiteSparse:GraphBLAS;
- LAGraph and LAGraphX;
- system headers;
- generated SQL and documentation files;
- test harnesses and helper scripts.

The report should include line, function, and branch coverage. Line coverage
alone will hide important extension behavior such as `PG_ARGISNULL` checks,
type dispatch, GraphBLAS status handling, and error branches.

## Current Constraints

The existing test flow has several properties that coverage support must
preserve:

- `test.sh` builds `Dockerfile-debug` and starts a disposable PostgreSQL
  container;
- the repository is bind-mounted read-only in practice, so the script copies
  it to `/home/postgres/test-work` before running tests;
- the extension is currently compiled and installed while the image is built;
- `make installcheck` runs the documentation regression suite;
- `make unitcheck` runs the pgTAP suite through `test/unit/run.sh`;
- the container is removed after the selected suite finishes;
- the debug image currently retains debug symbols but does not enable coverage
  instrumentation or install a coverage report generator.

Coverage artifacts must therefore be generated in the writable test copy and
exported before the container is removed.

## Build Instrumentation

Add an opt-in coverage mode to the project `Makefile` rather than changing the
default compiler flags.

Suggested interface:

```sh
make COVERAGE=1
```

When `COVERAGE=1`, add GCC coverage flags to OneSparse compilation and linking,
such as `--coverage` or the equivalent `-fprofile-arcs -ftest-coverage` flags.
Keep debug information and low optimization in the coverage build so source
locations and branch behavior remain useful. Do not instrument production
builds or third-party libraries.

The coverage build must preserve `.gcno` files next to the corresponding
object files. Runtime `.gcda` files are written when instrumented processes
exit and must be collected from the same build tree.

There are two viable build layouts:

1. Build and install the coverage-instrumented extension from the writable
   `/home/postgres/test-work` copy after the container starts. This is the
   preferred option because both `.gcno` and `.gcda` files remain available in
   one predictable location.
2. Build during the image build and explicitly retain all coverage metadata in
   the image. This is less desirable because runtime counters and source
   metadata are separated from the checkout and can become stale between runs.

The implementation should use the first layout unless a measured build-time
constraint requires otherwise.

## Runtime and Collection Lifecycle

Coverage collection must follow this order:

1. Build the extension with coverage instrumentation.
2. Remove stale `.gcda` files from the coverage build tree.
3. Install the instrumented extension.
4. Initialize and start PostgreSQL with OneSparse loaded as usual.
5. Run the selected documentation and/or semantic test suite.
6. Stop PostgreSQL cleanly with `pg_ctl stop`.
7. Run `gcovr` against the instrumented OneSparse source tree.
8. Copy reports from the container to the host `coverage/` directory.
9. Remove the test container.

The explicit database shutdown is required. GCC generally flushes execution
counters when the backend process exits; collecting before PostgreSQL stops
can produce incomplete or missing `.gcda` data.

Cleanup must be safe when tests fail or the script is interrupted. The trap
should attempt, in order, to stop PostgreSQL, collect coverage where possible,
copy artifacts, and remove the container. A failed test must not prevent the
coverage report from being retained for diagnosis.

## Report Formats

Generate reports under the disposable test workspace and export them to a
gitignored host directory:

```text
coverage/
  coverage.txt
  coverage.html
  coverage.xml
```

The reports should provide:

- a terminal summary in `coverage.txt` for local and CI logs;
- an HTML report with annotated source and uncovered lines;
- Cobertura XML for GitHub Actions artifacts and future coverage tooling.

The `gcovr` filters should include only the repository's `src/` tree. The
report should retain zero-coverage source files so entirely untested modules
are visible rather than silently omitted.

The host `coverage/` directory must be added to `.gitignore`. Coverage output
is generated data and should not be committed.

## Test Command Behavior

The existing commands should retain their meanings:

```sh
./test.sh          # documentation and semantic tests, combined coverage
./test.sh doctest  # documentation tests and their coverage
./test.sh unit     # semantic tests and their coverage
```

Direct commands should remain unchanged:

```sh
make installcheck
make unitcheck
```

They may run against an externally managed PostgreSQL instance and therefore
cannot reliably control backend shutdown or report artifact collection. They
should not generate coverage unless a separate, explicitly documented target
is added later.

The selected suite's test result must remain the command's primary exit status.
Coverage generation failures should be reported clearly and should normally
fail the coverage-enabled container workflow, but cleanup must still preserve
any partial reports. The exact policy should be decided during implementation
based on whether the default test command is expected to work on systems
without coverage tools.

## Container Changes

Update `Dockerfile-debug` only; coverage tooling must not be added to
production images.

The debug image should install:

- `gcovr`, preferably at a pinned version or through a reproducible package
  installation;
- any required GCC/gcov tooling if it is not already provided by
  `build-essential`.

The image should continue to provide the existing pinned PostgreSQL and pgTAP
environment. Do not mix PostgreSQL server coverage into the first OneSparse
coverage report.

## CI Integration

Update the test workflow to retain coverage artifacts even when tests fail:

```yaml
- name: Upload coverage report
  if: always()
  uses: actions/upload-artifact@v4
  with:
    name: onesparse-coverage
    path: coverage/
```

The gcovr text summary should also appear in the workflow log. The HTML report
is the primary artifact for developers investigating uncovered code.

Release validation should use the same coverage-capable test path only if the
additional runtime and artifact handling are acceptable. Coverage should not
become a release gate during the initial rollout.

## PostgreSQL Extension Testing Practices

Coverage should be used to find missing tests, not as a substitute for
behavioral assertions. For every uncovered region, determine whether it is:

- a public SQL behavior that needs a pgTAP assertion;
- an integration path better represented by a regression test;
- an error path requiring `throws_ok` with stable SQLSTATE/message matching;
- a type, descriptor, mask, sparse/dense, or scalar branch needing a focused
  input;
- unreachable defensive code that should be documented or removed.

Prefer tests through the public SQL API and a real PostgreSQL backend. Use
small deterministic fixtures, explicit transactions, and isolated test files.
Exercise both successful and failing paths, including NULL arguments, bounds,
incompatible dimensions, malformed input, type dispatch, serialization, and
large-object behavior.

Keep the existing separation of responsibilities:

- `sql/` and `expected/` cover executable documentation and full psql output;
- `test/unit/` covers semantic behavior, invariants, and stable error
  contracts;
- coverage reports identify which `src/` paths need additional tests.

For floating-point and graph algorithms, assert invariants, tolerances,
ordering, partitions, and dimensions rather than unstable formatted output.
For GraphBLAS diagnostics, match only stable intentional error contracts.

## Rollout Stages

### Stage 1: Local Instrumented Run

- Add conditional coverage flags to `Makefile`.
- Add `gcovr` to `Dockerfile-debug`.
- Build the extension from the writable test copy.
- Generate a source-filtered text and HTML report after clean shutdown.
- Add `coverage/` to `.gitignore`.

### Stage 2: Test Harness Integration

- Update `test.sh` cleanup and artifact-copy logic.
- Support coverage for all three test-suite selections.
- Verify reports are produced on passing and failing test runs.
- Verify stale counters do not affect subsequent runs.

### Stage 3: CI Publication

- Print the summary in CI logs.
- Upload HTML, text, and XML reports as artifacts with `if: always()`.
- Document how to inspect uncovered lines locally and in CI.

### Stage 4: Baseline and Optional Ratchet

- Record the initial line, function, and branch baseline.
- Add targeted tests based on high-value uncovered paths.
- Consider enforcing that coverage does not decrease from the main-branch
  baseline only after report generation is stable.

## Verification Plan

The implementation is complete when all of the following are verified:

1. A normal non-coverage build remains unchanged and succeeds.
2. `./test.sh`, `./test.sh doctest`, and `./test.sh unit` retain their existing
   test-selection behavior.
3. Each selected suite produces a report containing only OneSparse `src/`
   files.
4. The report includes uncovered files and uncovered source lines.
5. Branch and function coverage are present in the text or HTML output.
6. PostgreSQL is stopped before counters are collected.
7. Repeated runs start from clean counters and produce independent results.
8. Reports are copied out before the container is removed.
9. Reports remain available when a test assertion fails.
10. The CI workflow uploads the report artifact even on test failure.
11. Production Dockerfiles and ordinary installs do not receive coverage
    flags or runtime coverage dependencies.

## Open Implementation Choices

The implementation should resolve these details explicitly:

- exact `gcovr` version and installation method;
- whether coverage generation failure fails `./test.sh` or is advisory;
- whether release workflows publish coverage artifacts;
- whether the report should include branch exclusions for defensive macros;
- whether a future coverage-specific command is needed for externally managed
  PostgreSQL instances.

The recommended initial policy is to publish reports without a coverage
threshold, keep coverage enabled in the container-based `test.sh` path, and
use the resulting uncovered-source report to drive the next semantic test
phase.
