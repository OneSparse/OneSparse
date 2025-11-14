# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

OneSparse is a PostgreSQL extension that provides sparse linear algebra and graph algorithm capabilities by binding the SuiteSparse:GraphBLAS library. It exposes GraphBLAS functionality as PostgreSQL types, functions, and operators.

## Build and Test Commands

### Build and Install
```bash
make clean
make install
```

### Code Formatting
```bash
# Format code using pgindent (PostgreSQL code style)
pgindent <files>
```

### Running Tests
```bash
# Full test suite in Docker (preferred method)
./test.sh

# Direct PostgreSQL regression tests (requires PostgreSQL running)
make installcheck
```

The `test.sh` script:
- Removes any previous test container
- Builds the `onesparse/test` Docker image from `Dockerfile-debug`
- Starts a PostgreSQL container with OneSparse installed
- Runs `make installcheck` inside the container
- Cleans up the container

Individual test files are located in `sql/` with expected outputs in `expected/`.

### Running Individual Tests
```bash
# Run a specific test file
make installcheck REGRESS=test_name

# Example: run only the matrix I/O header tests
make installcheck REGRESS=test_io_header
```

### Building Documentation
```bash
# Generate documentation (uses doctestify.py to convert SQL tests to markdown)
python generate.py

# Build and serve documentation site
mkdocs build
mkdocs serve
```

## Architecture

### Core Data Types

OneSparse implements PostgreSQL "expanded object" types for GraphBLAS objects:

- **Matrix** (`os_Matrix`): Sparse 2D arrays backed by `GrB_Matrix`
- **Vector** (`os_Vector`): Sparse 1D arrays backed by `GrB_Vector`
- **Scalar** (`os_Scalar`): Single values backed by `GrB_Scalar`
- **Type** (`os_Type`): Type descriptors wrapping `GrB_Type`
- **Descriptor** (`os_Descriptor`): Operation modifiers wrapping `GrB_Descriptor`
- **Graph** (`os_Graph`): Graph structures with optional adjacency matrix

Each type has:
- **Flat representation**: Serialized form for storage (TOAST-able)
- **Expanded representation**: In-memory form holding GraphBLAS objects
- Conversion functions between representations

### Code Organization

- `src/onesparse.h`: Main header with macros, common definitions, GraphBLAS includes
- `src/onesparse.c`: Extension initialization, type promotion, lookup tables
- `src/guc.c`: PostgreSQL configuration parameters (GUCs)
- `src/*/`: Per-type subdirectories (matrix, vector, scalar, etc.)
  - `*_in.c`, `*_out.c`: Text I/O functions
  - `*.h`: Type-specific headers and function declarations
  - `*_*.c`: Type-specific operations
- `src/graph/`: LAGraph algorithm wrappers (BFS, PageRank, betweenness, etc.)
- `src/jit/`: JIT type system for dynamic code generation

### Code Generation System

OneSparse uses template-based code generation via `generate.py`:

1. Templates in `templates/` define SQL and C code patterns
2. `generate.py` reads GraphBLAS metadata using pyclibrary
3. Generates SQL files in `sql/` and `onesparse/` directories
4. `doctestify.py` converts SQL tests to markdown documentation

To regenerate code after template changes:
```bash
python generate.py
```

### Key Macros and Conventions

**Error Handling:**
- `OS_CHECK(method, obj, msg)`: Check GraphBLAS calls, report errors
- `LA_CHECK(method)`: Check LAGraph calls
- `ERRORIF(condition, msg)`: Conditional error reporting
- `ERRORNULL(arg)`: Check for NULL arguments

**Object Property Extraction:**
- `OS_MTYPE(type, matrix)`: Get matrix type
- `OS_MNROWS(nrows, matrix)`: Get row count
- `OS_MNCOLS(ncols, matrix)`: Get column count
- `OS_MNVALS(nvals, matrix)`: Get number of values
- Similar macros exist for vectors (`OS_V*`) and scalars (`OS_S*`)

**Debugging:**
- `OS_DEBUG` enables debug output
- `OS_START_BENCH()` / `OS_END_BENCH()`: Performance timing
- `LOGF()`: Function entry logging

**Debugging Commands:**
```bash
# Enable debug output (set before building)
export OS_DEBUG=1

# Run tests in debug Docker container with interactive access
docker run -it --entrypoint /bin/bash onesparse/test

# Attach to running test container for debugging
docker exec -it onesparse-test-db bash
```

### Extension Loading

`_PG_init()` in `src/onesparse.c` initializes:
1. GraphBLAS library (`GrB_init()`)
2. Type lookup tables
3. Descriptor, operator, monoid, semiring tables
4. GUC parameters
5. Custom burble handler for GraphBLAS diagnostics

### Memory Management

OneSparse uses PostgreSQL's expanded object framework:
- Flat objects are stored in TOAST when large
- Expanded objects live in PostgreSQL memory contexts
- `DatumGetMatrix()`, `DatumGetVector()`, etc. handle expansion
- Support functions enable in-place modification for performance

### GraphBLAS Integration

- Links against libgraphblas, liblagraph, liblagraphx
- Header: `/usr/local/include/suitesparse/GraphBLAS.h`
- Libraries: `-lgraphblas -llagraph -llagraphx -lpq`
- Compiler flags: `-std=c11 -Wfatal-errors`

## Development Workflow

1. **Modify templates** in `templates/` for generated code changes
2. **Run `generate.py`** to regenerate SQL and C files
3. **Rebuild extension**: `make clean && make install`
4. **Run tests**: `./test.sh` or `make installcheck`
5. **Update documentation**: `python generate.py` regenerates markdown from SQL tests
6. **Build docs site**: `mkdocs build` to verify documentation

## Docker Images

Multiple Dockerfiles for different purposes:
- `Dockerfile`: Basic build
- `Dockerfile-debug`: Development with debugging tools
- `Dockerfile-demo`: Demo container with sample data
- `Dockerfile-slim`: Minimal production image
- `Dockerfile-tests`: Test harness
- `Dockerfile-pgrouting`: Integration with pgRouting

## Extension Files

- `onesparse.control`: Extension metadata
- `onesparse/*.sql`: Schema migration files (version-specific)
- Generated by `generate.py` from templates

## Release Process

### Automated Release Workflow

The repository uses GitHub Actions to automate releases when version tags are pushed.

**Workflow:** `.github/workflows/release.yml`

**Trigger:** Push tags matching `v*.*.*` (e.g., v1.2.3, v2.0.0-beta.1)

**Process:**
1. **Test Job**: Runs `./test.sh` to verify all tests pass
2. **Docker Job**: Builds and pushes `onesparse/onesparse-slim` image to Docker Hub
3. **Release Job**: Creates GitHub release with auto-generated release notes

**Creating a Release:**
```bash
# Ensure main branch is up to date
git checkout main
git pull

# Create version tag
git tag v1.2.3

# Push tag to trigger workflow
git push origin v1.2.3
```

**Monitor:** Check GitHub Actions tab for workflow progress

**Verify:** Check Releases page for new release and Docker Hub for published image

**Requirements:** Repository must have `DOCKERHUB_USERNAME` and `DOCKERHUB_TOKEN` secrets configured

See `docs/plans/2025-11-13-github-release-workflow-design.md` for detailed design and usage.
