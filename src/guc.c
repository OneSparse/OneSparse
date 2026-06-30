#include "onesparse.h"

static void burble_assign_hook(bool newvalue, void *extra);
static bool os_burble = false;

static char *os_jit_control = NULL;
static GxB_JIT_Control os_jit_control_mode = GxB_JIT_OFF;

static int	os_nthreads = 0;
static double os_chunk = 0.0;
static bool os_print_1based = false;
static double os_hyper_switch = 0.0625;

static char *os_jit_cache_path = NULL;
static char *os_jit_c_compiler_name = NULL;
static char *os_jit_c_compiler_flags = NULL;
static char *os_jit_c_linker_flags = NULL;
static char *os_jit_c_libraries = NULL;
static char *os_jit_c_preface = NULL;
static char *os_jit_error_log = NULL;
static bool os_jit_use_cmake = false;

static void
burble_assign_hook(bool newvalue, void *extra)
{
	OK_CHECK(GrB_set(GrB_GLOBAL, (int32_t) newvalue, GxB_BURBLE),
			 "Cannot set burble function");
}

void
burble_notice_func(const char *format,...)
{
	char		buffer[1024 * 8];
	size_t		bsize = sizeof(buffer);
	va_list		args;
	int			result;

	va_start(args, format);
	result = vsnprintf(buffer, bsize, format, args);
	/* Call vsnprintf */
	va_end(args);

	if (result >= 0)
	{
		ereport(NOTICE, (errmsg("%s", buffer)));
	}
	else
	{
		elog(ERROR, "Could not format burble message.");
	}
}

static GxB_JIT_Control
parse_mode(const char *s)
{
	if (pg_strcasecmp(s, "off") == 0)
	{
		return GxB_JIT_OFF;
	}
	if (pg_strcasecmp(s, "pause") == 0)
	{
		return GxB_JIT_PAUSE;
	}
	if (pg_strcasecmp(s, "run") == 0)
	{
		return GxB_JIT_RUN;
	}
	if (pg_strcasecmp(s, "load") == 0)
	{
		return GxB_JIT_LOAD;
	}
	if (pg_strcasecmp(s, "on") == 0)
	{
		return GxB_JIT_ON;
	}
	return -1;
}

static bool
check_os_jit_control(char **newval, void **extra, GucSource source)
{
	GxB_JIT_Control parsed;

	if (*newval == NULL)
	{
		GUC_check_errdetail("Value must not be NULL.");
		return false;
	}

	parsed = parse_mode(*newval);
	if (parsed < 0)
	{
		GUC_check_errdetail("Valid values are: off, pause, run, load, on.");
		return false;
	}

	os_jit_control_mode = parsed;

	switch (os_jit_control_mode)
	{
		case GxB_JIT_OFF:
			OK_CHECK(GrB_set(GrB_GLOBAL, GxB_JIT_OFF, GxB_JIT_C_CONTROL),
					 "Cannot set jit control mode");
			break;
		case GxB_JIT_PAUSE:
			OK_CHECK(GrB_set(GrB_GLOBAL, GxB_JIT_PAUSE, GxB_JIT_C_CONTROL),
					 "Cannot set jit control mode");
			break;
		case GxB_JIT_RUN:
			OK_CHECK(GrB_set(GrB_GLOBAL, GxB_JIT_RUN, GxB_JIT_C_CONTROL),
					 "Cannot set jit control mode");
			break;
		case GxB_JIT_LOAD:
			OK_CHECK(GrB_set(GrB_GLOBAL, GxB_JIT_LOAD, GxB_JIT_C_CONTROL),
					 "Cannot set jit control mode");
			break;
		case GxB_JIT_ON:
			OK_CHECK(GrB_set(GrB_GLOBAL, GxB_JIT_ON, GxB_JIT_C_CONTROL),
					 "Cannot set jit control mode");
			break;
	}
	return true;
}

static void
nthreads_assign_hook(int newvalue, void *extra)
{
	if (newvalue == 0) return;	/* 0 = let GraphBLAS/OpenMP decide */
	OK_CHECK(GrB_set(GrB_GLOBAL, (int32_t) newvalue, GxB_GLOBAL_NTHREADS),
			 "Cannot set nthreads");
}

static void
chunk_assign_hook(double newvalue, void *extra)
{
	if (newvalue == 0.0) return;	/* 0 = let GraphBLAS decide */
	OK_CHECK(GxB_Global_Option_set_FP64(GxB_CHUNK, newvalue),
			 "Cannot set chunk");
}

static void
print_1based_assign_hook(bool newvalue, void *extra)
{
	OK_CHECK(GrB_set(GrB_GLOBAL, (int32_t) newvalue, GxB_PRINT_1BASED),
			 "Cannot set print_1based");
}

static void
hyper_switch_assign_hook(double newvalue, void *extra)
{
	OK_CHECK(GxB_Global_Option_set_FP64(GxB_HYPER_SWITCH, newvalue),
			 "Cannot set hyper_switch");
}

static void
jit_cache_path_assign_hook(const char *newval, void *extra)
{
	if (newval == NULL) return;
	OK_CHECK(GxB_Global_Option_set_CHAR(GxB_JIT_CACHE_PATH, newval),
			 "Cannot set JIT cache path");
}

static void
jit_c_compiler_name_assign_hook(const char *newval, void *extra)
{
	if (newval == NULL) return;
	OK_CHECK(GxB_Global_Option_set_CHAR(GxB_JIT_C_COMPILER_NAME, newval),
			 "Cannot set JIT C compiler name");
}

static void
jit_c_compiler_flags_assign_hook(const char *newval, void *extra)
{
	if (newval == NULL) return;
	OK_CHECK(GxB_Global_Option_set_CHAR(GxB_JIT_C_COMPILER_FLAGS, newval),
			 "Cannot set JIT C compiler flags");
}

static void
jit_c_linker_flags_assign_hook(const char *newval, void *extra)
{
	if (newval == NULL) return;
	OK_CHECK(GxB_Global_Option_set_CHAR(GxB_JIT_C_LINKER_FLAGS, newval),
			 "Cannot set JIT C linker flags");
}

static void
jit_c_libraries_assign_hook(const char *newval, void *extra)
{
	if (newval == NULL) return;
	OK_CHECK(GxB_Global_Option_set_CHAR(GxB_JIT_C_LIBRARIES, newval),
			 "Cannot set JIT C libraries");
}

static void
jit_c_preface_assign_hook(const char *newval, void *extra)
{
	if (newval == NULL) return;
	OK_CHECK(GxB_Global_Option_set_CHAR(GxB_JIT_C_PREFACE, newval),
			 "Cannot set JIT C preface");
}

static void
jit_error_log_assign_hook(const char *newval, void *extra)
{
	if (newval == NULL) return;
	OK_CHECK(GxB_Global_Option_set_CHAR(GxB_JIT_ERROR_LOG, newval),
			 "Cannot set JIT error log");
}

static void
jit_use_cmake_assign_hook(bool newvalue, void *extra)
{
	OK_CHECK(GrB_set(GrB_GLOBAL, (int32_t) newvalue, GxB_JIT_USE_CMAKE),
			 "Cannot set JIT use cmake");
}

void
initialize_gucs()
{
	OK_CHECK(GrB_set(GrB_GLOBAL, (void *) burble_notice_func, GxB_PRINTF, sizeof(void *)),
			 "Cannot set burble print function");

	DefineCustomBoolVariable(
							 "onesparse.burble",
							 "Enable or disable the SuiteSparse burble feature.",
							 "Session-level parameter to control burble behavior.",
							 &os_burble,
							 false,
							 PGC_USERSET,
							 0,
							 NULL,
							 burble_assign_hook,
							 NULL);

	DefineCustomStringVariable(
							   "onesparse.jit_control",
							   "Control mode for OneSparse JIT.",
							   "Valid values: off, pause, run, load, on.",
							   &os_jit_control,
							   "off",
							   PGC_POSTMASTER,
							   0,
							   check_os_jit_control,
							   NULL,
							   NULL);

	DefineCustomIntVariable(
							"onesparse.nthreads",
							"Maximum number of threads for SuiteSparse:GraphBLAS operations.",
							"0 means use all available threads (OpenMP default).",
							&os_nthreads,
							0,
							0,
							65536,
							PGC_USERSET,
							0,
							NULL,
							nthreads_assign_hook,
							NULL);

	DefineCustomRealVariable(
							 "onesparse.chunk",
							 "Chunk size for small problems in SuiteSparse:GraphBLAS.",
							 "0 means automatic (GraphBLAS default). Typical values: 65536, 262144.",
							 &os_chunk,
							 0.0,
							 0.0,
							 1e15,
							 PGC_USERSET,
							 0,
							 NULL,
							 chunk_assign_hook,
							 NULL);

	DefineCustomBoolVariable(
							 "onesparse.print_1based",
							 "Print matrix/vector indices as 1-based instead of 0-based.",
							 NULL,
							 &os_print_1based,
							 false,
							 PGC_USERSET,
							 0,
							 NULL,
							 print_1based_assign_hook,
							 NULL);

	DefineCustomRealVariable(
							 "onesparse.hyper_switch",
							 "Threshold to switch between hypersparse and sparse format.",
							 "Use GxB_ALWAYS_HYPER (-1) or GxB_NEVER_HYPER (0). Default ~0.0625.",
							 &os_hyper_switch,
							 0.0625,
							 -1.0,
							 1.0,
							 PGC_USERSET,
							 0,
							 NULL,
							 hyper_switch_assign_hook,
							 NULL);

	DefineCustomStringVariable(
							   "onesparse.jit_cache_path",
							   "Cache directory for SuiteSparse JIT compiled kernels.",
							   NULL,
							   &os_jit_cache_path,
							   NULL,
							   PGC_POSTMASTER,
							   0,
							   NULL,
							   jit_cache_path_assign_hook,
							   NULL);

	DefineCustomStringVariable(
							   "onesparse.jit_c_compiler_name",
							   "C compiler for SuiteSparse JIT (e.g. \"gcc\").",
							   NULL,
							   &os_jit_c_compiler_name,
							   NULL,
							   PGC_POSTMASTER,
							   0,
							   NULL,
							   jit_c_compiler_name_assign_hook,
							   NULL);

	DefineCustomStringVariable(
							   "onesparse.jit_c_compiler_flags",
							   "Compiler flags for SuiteSparse JIT.",
							   NULL,
							   &os_jit_c_compiler_flags,
							   NULL,
							   PGC_POSTMASTER,
							   0,
							   NULL,
							   jit_c_compiler_flags_assign_hook,
							   NULL);

	DefineCustomStringVariable(
							   "onesparse.jit_c_linker_flags",
							   "Linker flags for SuiteSparse JIT.",
							   NULL,
							   &os_jit_c_linker_flags,
							   NULL,
							   PGC_POSTMASTER,
							   0,
							   NULL,
							   jit_c_linker_flags_assign_hook,
							   NULL);

	DefineCustomStringVariable(
							   "onesparse.jit_c_libraries",
							   "Libraries for SuiteSparse JIT.",
							   NULL,
							   &os_jit_c_libraries,
							   NULL,
							   PGC_POSTMASTER,
							   0,
							   NULL,
							   jit_c_libraries_assign_hook,
							   NULL);

	DefineCustomStringVariable(
							   "onesparse.jit_c_preface",
							   "C code preface inserted into every SuiteSparse JIT kernel.",
							   NULL,
							   &os_jit_c_preface,
							   NULL,
							   PGC_POSTMASTER,
							   0,
							   NULL,
							   jit_c_preface_assign_hook,
							   NULL);

	DefineCustomStringVariable(
							   "onesparse.jit_error_log",
							   "File path for SuiteSparse JIT compilation error log.",
							   NULL,
							   &os_jit_error_log,
							   NULL,
							   PGC_POSTMASTER,
							   0,
							   NULL,
							   jit_error_log_assign_hook,
							   NULL);

	DefineCustomBoolVariable(
							 "onesparse.jit_use_cmake",
							 "Use cmake instead of direct compile for SuiteSparse JIT.",
							 NULL,
							 &os_jit_use_cmake,
							 false,
							 PGC_POSTMASTER,
							 0,
							 NULL,
							 jit_use_cmake_assign_hook,
							 NULL);
}
