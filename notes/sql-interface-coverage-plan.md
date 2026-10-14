# SQL Interface Coverage Plan

## Scope and Baseline

This report reviews `coverage/coverage.functions.html`, generated after the
current unit suite. It reports 363 of 475 functions executed (76.4%), 70.0%
line coverage, and 28.7% branch coverage.

The goal of the first effort is **SQL-interface coverage**, not 100% source or
branch coverage. A SQL interface function is an implementation symbol named in
`onesparse/onesparse--1.0.1.sql` through:

```sql
AS '$libdir/onesparse', '<symbol>'
```

The test suite should execute every such implementation at least once, with
representative successful behavior and meaningful SQL-visible error cases.
Generated `pg_finfo_*` registration symbols are not test targets. Functions
that are reached only as a consequence of the public SQL API are separated in
the internal-support section and are deliberately out of scope for this pass.

The coverage report includes a few versioned duplicate HTML names for recently
changed sources. The function table, source path, and generated extension SQL
are the authoritative mapping used here.

## Findings

The following SQL-exposed implementations have no direct execution in the
current report:

| Area | Unexecuted SQL implementation | Planned test location |
| --- | --- | --- |
| Type/JIT | `type_name`, `jit_type` | New `15_types.sql` |
| JIT row conversion | `row_scalar`, `scalar_row` | New `15_jit.sql` |
| Operator/type output | `descriptor_out`, `unaryop_out`, `indexunaryop_out`, `binaryop_out`, `monoid_out`, `semiring_out` | Existing type/operator suites or new focused file |
| Operator/type names | `unaryop_name`, `indexunaryop_name`, `monoid_name`, `semiring_name` | Existing type/operator suites or new focused file |
| Vector | `vector_out`, `vector_new`, `vector_info`, `vector_vxv`, `vector_norm` | `20_vector.sql` |
| Matrix | `matrix_out`, `matrix_new`, `matrix_norm` | `30_matrix.sql` |
| Graph I/O | `graph_in`, `graph_out` | `60_graph.sql` |

Several public functions are already called but have low block coverage. The
plan below adds a small set of behavior-oriented cases per operation family,
rather than chasing each compiler branch individually.

## Test Design Rules

- Use pgTAP semantic assertions. Test returned values through `elements`,
  `nvals`, dimensions, `type`, and equality, rather than relying on GraphBLAS
  implementation details or formatting that is not contractual.
- Test error paths with `throws_ok` only when the extension's observable error
  is stable enough to assert. At minimum cover invalid dimensions, indexes,
  incompatible shapes/types, and invalid external data where supported.
- For `INOUT`-style GraphBLAS operations, add one success case for default
  optional arguments, one case with an output object, and selected mask,
  accumulator, and descriptor cases. Do not create a Cartesian-product suite.
- Execute PostgreSQL type-output functions deliberately with `value::text`;
  printing values through pgTAP does not reliably force every output function.
- Keep all tests transactional, update each file's `plan(...)` count exactly,
  and finish with `finish()` and `ROLLBACK` following existing conventions.

## Implementation Phases

### 1. Type, Operator, and SQL I/O Completeness

Add a compact dedicated unit file, `test/unit/sql/15_types.sql`.

1. Assert `name('int32'::type)` and representative built-in type names to
   execute `type_name`.
2. Define a representative composite type and assert that `jit_type` emits its
   C layout, plus its non-composite-type failure.
3. For each descriptor, unary operator, index unary operator, binary operator,
   monoid, and semiring, force `::text` output and round-trip it through its
   type input function. Assert `name(...)` for the currently missed operator
   families.
4. Add scalar type-output cases to `10_scalar.sql`: each supported family,
   empty scalar, zero, signed value, and special floating values if their text
   representation is part of the accepted syntax.
5. Add vector, matrix, and graph input/output round trips in their respective
   test files. Include empty and populated values. `graph_in` and `graph_out`
   require explicit coercions rather than only `graph(matrix)` construction.

`indexbinaryop` is excluded from this SQL-interface phase: it has C source but
no type or function declaration in the generated extension SQL. It belongs in
the deferred internal-support review unless a SQL binding is added later.

Expected result: all generated public operator/type symbols and every
PostgreSQL type I/O path have at least one SQL-level execution.

### 2. Scalar and JIT Row Interfaces

Extend `10_scalar.sql` and add `15_jit.sql` for the record-dependent API.

1. For `scalar_type`, `scalar_nvals`, `print`, `wait`, `dup`, and `clear`, use
   empty and populated scalars and prove duplicate independence by clearing or
   changing only the duplicate.
2. Test scalar casts across signed, unsigned, floating-point, and boolean
   families, including a narrowing/sign-changing conversion.
3. Define a temporary composite type or table row with mixed fixed-width,
   floating, boolean, text/bit, and nullable fields. Round-trip it through
   `row_scalar(record)` and `scalar_row(bytea, anyelement)`.
4. Cover deterministic representation using two equal records and the current
   null-field rejection behavior. `row_scalar` presently requires each packed
   field to be non-null, so nullable-field round trips require a future
   production implementation change rather than a coverage-only test.

### 3. Vector Public Operations

Extend `20_vector.sql`; keep random generation cases in `38_random.sql`.

1. Add direct constructor cases for `vector(type, size)`: bounded, default
   size, and invalid size. Verify type, size, and empty state.
2. Add literal/output round trips for vector input variants, sparse gaps, and
   malformed or out-of-range input.
3. Cover basic lifecycle and querying: `type`, `nvals`, `size`, `elements`,
   `contains`, `get_element`, `set_element`, `remove_element`, `dup`, `clear`,
   `resize`, `wait`, `info`, and equality. Use present/absent positions and
   empty/populated inputs.
4. Add vector algebra cases for `eadd`, `emult`, `eunion`, scalar reduction,
   `apply`, scalar-first and scalar-second apply, and `choose`. Use disjoint
   and intersecting sparsity patterns, then a selected output/mask/accumulator/
   descriptor case per operation family.
5. Add index-array cases for `assign` and `extract_vector`: all indexes,
   reordered subset, absent source entries, and invalid indexes.
6. Add direct `vxv` and `norm` tests. Use small hand-checkable inputs and both
   default and explicit semiring/output forms where exposed.
7. Extend cast semantics beyond dispatch coverage: identity, empty vector,
   sign/precision change, and boolean conversion.
8. In `38_random.sql`, verify seeded repeatability, density zero/one, and
   validation for vector size and density. Add a valid `lsh` case and assert
   its documented rejection behavior for unsupported inputs.

### 4. Matrix Public Operations

Extend `30_matrix.sql`, `36_matrix_ops.sql`, `37_matrix_query.sql`,
`38_random.sql`, and `50_io.sql` according to responsibility.

1. Add direct `matrix(type, nrows, ncols)` constructor coverage, plus matrix
   text output/input round trips for empty, rectangular, and sparse matrices.
2. Cover lifecycle/query methods: dimensions, type, `nvals`, `elements`,
   `irows`, `icols`, `contains`, get/set/remove, `dup`, `clear`, `resize`,
   `wait`, `info`, and equality. Include empty, present, absent, and invalid
   coordinate cases.
3. Add aggregate coverage for zero-row, one-row, multiple compatible rows,
   and incompatible matrix groups for `matrix_agg` and `vector_agg`.
4. In `36_matrix_ops.sql`, cover `eadd`, `emult`, `eunion`, matrix/scalar/
   row-vector/column-vector assignment, matrix/vector extraction, `mxm`,
   `mxv`, `vxm`, `kronecker`, reductions, `choose`, transpose, and unary/
   binary applies. Each operation gets a basic exact-result assertion plus
   targeted optional-argument coverage. Include an incompatible-shape case
   for multiplication and assignment/extraction index validation.
5. Add direct `norm(matrix)` coverage with known values, supplied output, and
   an empty matrix. Add matrix cast semantic cases for identity, empty,
   precision/sign changes, and boolean conversion.
6. In `37_matrix_query.sql`, add zero-row, explicit versus inferred bounds,
   batching, and malformed-query/error coverage for `matrix_query`.
7. In `38_random.sql`, add seeded repeatability, density endpoints, supported
   types, and invalid dimension/density cases for `random_matrix`.
8. In `50_io.sql`, add bytea and file serialize/deserialize round trips for
   empty and populated matrices; corrupted bytes, missing paths, and a valid
   Matrix Market fixture plus malformed/missing-file `mmread` cases. Cover
   large-object `save`/`load` with a valid object and invalid OID behavior.

### 5. Graph Public Operations

Extend `60_graph.sql` with direct `graph` overloads, not just matrix wrappers.

1. Test graph construction for the default kind, directed/undirected kinds,
   invalid kind, and non-square input. Force graph `::text` output and test
   any supported text-input round trip.
2. For BFS and SSSP, use a graph with a nonzero source and an unreachable
   vertex; cover invalid source and directed traversal where supported.
3. For PageRank, use a dangling/disconnected graph and non-default damping,
   tolerance, and iteration settings.
4. For connected components, triangle count/centrality, square clustering,
   FGLT, and betweenness, add empty/singleton/disconnected and known small
   graph fixtures. Include direct graph overload invocation.
5. For `argminmax`, test both min/max selection, both dimensions, ties, and
   sparse rows or columns without values.

### 6. Verification and Completion Check

1. Run `./test.sh unit` after each logical phase; keep the report generated
   from a clean test container.
2. Run `./test.sh` after all phases because documentation regressions exercise
   a distinct portion of the generated SQL surface.
3. Cross-check every `AS '$libdir/onesparse', '<symbol>'` entry in the
   generated extension SQL against the function report. The target is no
   SQL-exposed implementation reported as `not called`.
4. Review remaining partial functions manually. A function may remain below
   100% block coverage only when the remaining paths are PostgreSQL NULL
   guards, GraphBLAS/library failure handling that cannot be reliably induced,
   or deliberately deferred internal support behavior. Record the reason next
   to the final coverage result.

## Deferred: Internal Support Coverage

The following is intentionally a separate later effort. These functions are
not direct SQL entry points, even though public SQL tests can incidentally
exercise them.

| Internal group | Examples | Future testing concern |
| --- | --- | --- |
| Expanded-object conversion | `DatumGet*`, `expand_*`, `flatten_*`, `new_*`, `*_get_flat_size` | Force flat/expanded transitions, TOAST storage, copy/detoast behavior, and memory-context cleanup. |
| PostgreSQL cleanup callbacks | `context_callback_*_free` | Exercise resource owner and memory-context lifetimes without relying on process exit. |
| Support-function entrypoints | `*_support`, especially matrix operation support functions | Decide whether these are actually reachable from SQL planner support hooks; add EXPLAIN/planner integration tests only if registered. |
| Core dispatch/default helpers | `default_binaryop`, `default_monoid`, `default_semiring`, `code_type`, `code_size`, `error_name`, array conversion helpers | Build a dispatch/error matrix around default resolution and type combinations. |
| GUC and diagnostics | `burble_notice_func`, `burble_assign_hook`, `check_os_jit_control`, `initialize_gucs` | Use session `SET`, invalid values, and capture NOTICE behavior where stable. |
| Parser/printer internals | `_parse_matrix`, `_print_matrix`, `_scalar_in` | Systematically enumerate accepted and rejected literal grammar, escaping, special FP values, and round trips. |
| JIT layout helpers | `emit_field`, `map_type`, `get_field_layout`, `get_notnull_attr`, `bytes_to_bit_cstr`, `cstring_to_typed_datum` | Complete only after public row-scalar tests establish supported datum/composite behavior. |
| Graph flat representation | `graph_get_flat_size`, `flatten_graph`, `expand_graph` | Require persisted graph values or explicit type I/O round trips; validate graph storage semantics. |

The `*_support` functions should not be counted as uncovered SQL operations
until their registrations and planner invocation paths have been established.
They have separate C symbols and `PG_FUNCTION_INFO_V1` declarations, but are
not normal user-callable functions in the generated extension SQL.

## Suggested Order

1. Type/operator I/O and names.
2. Constructors, output coercions, and lifecycle/query functions.
3. Vector and matrix algebra optional arguments and validation paths.
4. Matrix external I/O and query paths.
5. Direct graph overloads and graph I/O.
6. JIT row conversion, after defining the expected composite-record contract.
7. Recompute coverage and start the deferred internal-support plan from the
   remaining report.

## Phase 6 Audit Result

Completed on 2026-10-13 using the combined `./test.sh` workflow. All 33
documentation regressions and all 354 pgTAP assertions passed. The generated
coverage report records 4,422/5,106 lines (86.6%), 400/475 functions (84.2%),
and 1,983/5,322 branches (37.3%).

The generated extension SQL contains 231 distinct active C bindings. After
excluding `pg_finfo_*` registration shims and planner support declarations,
every bound implementation represented in `coverage.functions.html` has a
positive call count. The generated macro-instantiation families below do not
have individual function-table rows, so their suffixed bindings cannot be
directly reconciled by name:

- scalar constructors, casts, mutation, and native arithmetic in
  `src/scalar/scalar_ops.c`;
- array/vector casts and typed vector aggregate transitions in
  `src/vector/vector_ops.c`;
- typed matrix aggregate transitions in `src/matrix/matrix_ops.c`.

They are exercised by the documentation and semantic tests; their absence from
the function table is a gcov reporting limitation, not evidence of an
unexecuted SQL binding.

The remaining zero-call functions are deferred internal work:

- planner `*_support` entry points, which need a separate registration and
  planner-invocation audit before EXPLAIN-level coverage is meaningful;
- `indexbinaryop` expanded-object and type-I/O functions, which have source
  code and a metadata table but no generated `CREATE TYPE` or callable function
  binding;
- scalar typmod input/output functions, which are not installed by the current
  `CREATE TYPE scalar` declaration;
- expanded-object conversion, cleanup callbacks, GUC diagnostics, and parser
  branches listed in the internal-support section above.

This completes the SQL-interface coverage plan. The next test-planning effort
should use the deferred internal-support inventory rather than extending public
SQL cases solely to increase aggregate coverage.
