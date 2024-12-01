#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_agg_final);
Datum matrix_agg_final(PG_FUNCTION_ARGS)
{
    os_Matrix *state = OS_GETARG_MATRIX(0);

	if (state == NULL)
	{
        PG_RETURN_NULL();
    }

    OS_RETURN_MATRIX(state);
}


/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
