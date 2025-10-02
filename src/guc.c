#include "onesparse.h"

static void burble_assign_hook(bool newvalue, void *extra);
static bool os_burble = false;

static char *os_jit_control = NULL;
static GxB_JIT_Control os_jit_control_mode = GxB_JIT_OFF;

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
							   NULL
		);
}
