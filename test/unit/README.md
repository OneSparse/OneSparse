# OneSparse Unit Tests

This directory contains pgTAP tests for OneSparse's SQL-visible behavior.
They complement the documentation regression tests in `sql/`; they do not
generate Markdown or compare formatted psql transcripts.

## Running Tests

Start the project test container through the normal test runner:

```sh
./test.sh unit
```

Run only the documentation regression suite:

```sh
./test.sh doctest
```

Run both suites:

```sh
./test.sh
```

With a compatible PostgreSQL server already running, run the unit suite
directly:

```sh
make unitcheck
```

## Conventions

- Keep a test file independent: it must create its own data and roll it back.
- Use `is`, `ok`, `results_eq`, and `throws_ok` instead of formatted object
  output.
- Compare vectors and matrices through sorted `elements()` rows, dimensions,
  and type metadata rather than `print()` output.
- Use explicit tolerances for floating-point behavior.
- Match an error SQLSTATE and only stable, intentional message text.
- Do not add unit test files to `sql/`, `templates/tests/`, or `expected/`.
  Those paths are part of the documentation regression pipeline.

## Layout

`sql/00_setup.sql` verifies the required extensions and session settings.
The numbered files reserve focused domains for future assertions. Each file
currently contains one harness-only passing assertion because pgTAP rejects a
zero-test plan when `finish()` is called. These placeholders prove that the
files load and complete successfully; product-behavior assertions will replace
them as coverage is added. `lib/` holds shared test helpers as they become
necessary. The test image installs pgTAP and its `pg_prove` TAP source handler.
