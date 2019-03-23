//------------------------------------------------------------------------------
// SuiteSparse/GraphBLAS/Demo/Source/dpagerank: pagerank using a real semiring
//------------------------------------------------------------------------------

// A is a square unsymmetric binary matrix of size n-by-n, where A(i,j) is the
// edge (i,j).  Self-edges are LAGRAPH_OK.  A can be of any built-in type.

// On output, P is pointer to an array of LAGraph_PageRank structs.  P[0] is the
// highest ranked page, with pagerank P[0].pagerank and the page corresponds to
// node number P[0].page in the graph.  P[1] is the next page, and so on, to
// the lowest-ranked page P[n-1].page with rank P[n-1].pagerank.

// See dpagerank.m for the equivalent computation in MATLAB (except the random
// number generator differs).

//------------------------------------------------------------------------------
// helper macros
//------------------------------------------------------------------------------

// free all workspace
#define FREEWORK                                \
{                                               \
    GrB_free (&D) ;                             \
    GrB_free (&C) ;                             \
    GrB_free (&r) ;                             \
    GrB_free (&t) ;                             \
    GrB_free (&d) ;                             \
    if (I != NULL) { free_function (I) ; I = NULL ; }    \
    if (X != NULL) { free_function (X) ; X = NULL ; }    \
    GrB_free (&op_div) ;                        \
    GrB_free (&op_diff) ;                       \
}

// error handler: free output P and all workspace
#define LAGRAPH_FREE_ALL                        \
{                                               \
    if (P != NULL) { free_function (P) ; P = NULL ; }    \
    FREEWORK ;                                  \
}

//------------------------------------------------------------------------------
// scalar operators
//------------------------------------------------------------------------------

#define DAMPING 0.85

float rsum ;
#pragma omp threadprivate(rsum)

void fdiv  (void *z, const void *x)
{
    (*((float *) z)) = (* ((float *) x)) / rsum ;
}

void fdiff (void *z, const void *x, const void *y)
{
    float delta = (* ((float *) x)) - (* ((float *) y)) ;
    (*((float *) z)) = delta * delta ;
}

//------------------------------------------------------------------------------
// comparison function for qsort
//------------------------------------------------------------------------------

int compar (const void *x, const void *y)
{
    LAGraph_PageRank *a = (LAGraph_PageRank *) x ;
    LAGraph_PageRank *b = (LAGraph_PageRank *) y ;

    // sort by pagerank in descending order
    if (a->pagerank > b->pagerank)
    {
        return (-1) ;
    }
    else if (a->pagerank == b->pagerank)
    {
        return (0) ;
    }
    else
    {
        return (1) ;
    }
}

//------------------------------------------------------------------------------
// LAGraph_pagerank: compute the pagerank of all nodes in a graph
//------------------------------------------------------------------------------

GrB_Info LAGraph_pagerank2       // GrB_SUCCESS or error condition
(
    LAGraph_PageRank **Phandle, // output: array of LAGraph_PageRank structs
    GrB_Matrix A,               // binary input graph, not modified
    int itermax,                // max number of iterations
    double tol,                 // stop when norm (r-rnew,2) < tol
    int *iters                  // number of iterations taken
)
{

    //--------------------------------------------------------------------------
    // initializations
    //--------------------------------------------------------------------------

    double tic [2] ;
    LAGraph_tic (tic) ;

    GrB_Info info ;
    float *X = NULL ;
    GrB_Index n, nvals, *I = NULL ;
    LAGraph_PageRank *P = NULL ;
    GrB_Vector r = NULL, t = NULL, d = NULL ;
    GrB_UnaryOp op_div = NULL ;
    GrB_BinaryOp op_diff = NULL ;
    GrB_Matrix C = NULL, D = NULL ;

    if (Phandle == NULL) LAGRAPH_ERROR ("null pointer", GrB_NULL_POINTER) ;
    (*Phandle) = NULL ;

    // n = size (A,1) ;         // number of nodes
    LAGRAPH_OK (GrB_Matrix_nrows (&n, A)) ;
    if (n == 0) return (GrB_SUCCESS) ;

    // teleport = (1 - 0.85) / n
    float one = 1.0 ;
    float teleport = (one - DAMPING) / ((float) n) ;

    // r (i) = 1/n for all nodes i
    float x = 1.0 / ((float) n) ;
    LAGRAPH_OK (GrB_Vector_new (&r, GrB_FP32, n)) ;
    LAGRAPH_OK (GrB_assign (r, NULL, NULL, x, GrB_ALL, n, NULL)) ;

    // d (i) = out deg of node i
    LAGRAPH_OK (GrB_Vector_new (&d, GrB_FP32, n)) ;
    LAGRAPH_OK (GrB_reduce (d, NULL, NULL, GrB_PLUS_FP32, A, NULL)) ;

    // D = (1/diag (d)) * DAMPING
    #ifdef GxB_SUITESPARSE_GRAPHBLAS
    GrB_Type type ;
    LAGRAPH_OK (GxB_Vector_export (&d, &type, &n, &nvals, &I, (void **) (&X),
        NULL)) ;
    #else
    nvals = n ;
    I = malloc_function ((nvals + 1) * sizeof (GrB_Index)) ;
    X = malloc_function ((nvals + 1) * sizeof (float)) ;
    if (I == NULL || X == NULL)
    {
        LAGRAPH_ERROR ("out of memory", GrB_OUT_OF_MEMORY) ;
    }
    LAGRAPH_OK (GrB_Vector_extract (I, X, &nvals, d)) ;
    LAGRAPH_OK (GrB_free (&d)) ;
    #endif

    for (int64_t k = 0 ; k < nvals ; k++) X [k] = DAMPING / X [k] ;
    LAGRAPH_OK (GrB_Matrix_new (&D, GrB_FP32, n, n)) ;
    LAGRAPH_OK (GrB_Matrix_build (D, I, I, X, nvals, GrB_PLUS_FP32)) ;
    free_function (I) ;
    free_function (X) ;

    // GxB_print (D, 3) ;

    // C = diagonal matrix with all zeros on the diagonal.  This ensures that
    // the vectors r and t remain dense, which is faster.
    LAGRAPH_OK (GrB_Matrix_new (&C, GrB_FP32, n, n)) ;
    // GxB_set (C, GxB_HYPER, GxB_ALWAYS_HYPER) ;

    for (int64_t k = 0 ; k < n ; k++)
    {
        // C(k,k) = 0
        LAGRAPH_OK (GrB_Matrix_setElement (C, (float) 0, k, k)) ;
    }


    // GxB_print (A, 3) ;

    // GxB_print (C, 3) ;

    double tt = LAGraph_toc (tic) ;
    fprintf (stderr, "init time %g\n", tt) ;
    LAGraph_tic (tic) ;

    // C = C+(D*A)' = C+A'*D' using A(i,j)/D(i,i) as the multiplicative operator
    LAGRAPH_OK (GrB_mxm (C, NULL, GrB_PLUS_FP32, GxB_PLUS_TIMES_FP32, A, D,
        LAGraph_desc_ttoo)) ;

/*
    // C = C+(D*A)
    LAGRAPH_OK (GrB_mxm (C, NULL, GrB_PLUS_FP32, GxB_PLUS_TIMES_FP32, D, A,
        NULL)) ;
*/

    LAGRAPH_OK (GrB_free (&D)) ;

    // create operators
    LAGRAPH_OK (GrB_UnaryOp_new  (&op_div,  fdiv, GrB_FP32, GrB_FP32)) ;
    LAGRAPH_OK (GrB_BinaryOp_new (&op_diff, fdiff, GrB_FP32, GrB_FP32,
        GrB_FP32)) ;

    float ftol = tol*tol ;  // use tol^2 so sqrt(rdiff) not needed
    float rdiff = 1 ;       // so first iteration is always done

    LAGRAPH_OK (GrB_Vector_new (&t, GrB_FP32, n)) ;

    // GxB_print (C, 2) ;
    // GxB_fprint (C, 2, stderr) ;

    tt = LAGraph_toc (tic) ;
    fprintf (stderr, "C time %g\n", tt) ;
    LAGraph_tic (tic) ;

    //--------------------------------------------------------------------------
    // iterate to compute the pagerank of each node
    //--------------------------------------------------------------------------

    double t2 = 0 ;

    for ((*iters) = 0 ; (*iters) < itermax && rdiff > ftol ; (*iters)++)
    {

        //----------------------------------------------------------------------
        // t = r*C + (teleport * sum (r)) ;
        //----------------------------------------------------------------------

        // GxB_print (r, 2) ;

        float s = 1 ;
        LAGRAPH_OK (GrB_reduce (&s, NULL, GxB_PLUS_FP32_MONOID, r, NULL)) ;
        // printf ("s %g\n", s) ;

        double tic2 [2] ;
        LAGraph_tic (tic2) ;

//      LAGRAPH_OK (GrB_vxm (t, NULL, NULL, GxB_PLUS_TIMES_FP32, r, C, NULL)) ;
        LAGRAPH_OK (GrB_mxv (t, NULL, NULL, GxB_PLUS_TIMES_FP32, C, r, NULL)) ;

        double t3 = LAGraph_toc (tic2) ;
        t2 += t3 ;
//      fprintf (stderr, "one mxv %g\n", t3) ;

//      LAGraph_tic (tic2) ;
//      LAGRAPH_OK (GrB_mxv (t, NULL, NULL, GxB_PLUS_TIMES_FP32, C, r, NULL)) ;
//      t3 = LAGraph_toc (tic2) ;
//      fprintf (stderr, "another mxv %g\n", t3) ;

        s *= teleport ;
        LAGRAPH_OK (GrB_assign (t, NULL, GrB_PLUS_FP32, s, GrB_ALL, n, NULL)) ;

        //----------------------------------------------------------------------
        // rdiff = sum ((r-t).^2)
        //----------------------------------------------------------------------

        LAGRAPH_OK (GrB_eWiseAdd (r, NULL, NULL, op_diff, r, t, NULL)) ;
        LAGRAPH_OK (GrB_reduce (&rdiff, NULL, GxB_PLUS_FP32_MONOID, r, NULL)) ;

        //----------------------------------------------------------------------
        // swap r and t
        //----------------------------------------------------------------------

        GrB_Vector temp = r ;
        r = t ;
        t = temp ;
//      LAGRAPH_OK (GrB_Vector_clear (t)) ;

        // GxB_print (r, 3) ;
    }

        fprintf (stderr, "mxv %g\n", t2) ;

    LAGRAPH_OK (GrB_free (&C)) ;
    LAGRAPH_OK (GrB_free (&t)) ;

    tt = LAGraph_toc (tic) ;
    fprintf (stderr, "rank time %g\n", tt) ;
    LAGraph_tic (tic) ;

    //--------------------------------------------------------------------------
    // scale the result
    //--------------------------------------------------------------------------

    // rsum = sum (r)
    LAGRAPH_OK (GrB_reduce (&rsum, NULL, GxB_PLUS_FP32_MONOID, r, NULL)) ;

    // r = r / rsum
    LAGRAPH_OK (GrB_apply (r, NULL, NULL, op_div, r, NULL)) ;

    //--------------------------------------------------------------------------
    // sort the nodes by pagerank
    //--------------------------------------------------------------------------

    // [r,irank] = sort (r, 'descend') ;

    // extract the contents of r
    #ifdef GxB_SUITESPARSE_GRAPHBLAS
    LAGRAPH_OK (GxB_Vector_export (&r, &type, &n, &nvals, &I, (void **) (&X),
        NULL)) ;
    #else
    nvals = n ;
    I = malloc_function (n * sizeof (GrB_Index)) ;
    X = malloc_function (n * sizeof (float)) ;
    if (I == NULL || X == NULL)
    {
        LAGRAPH_ERROR ("out of memory", GrB_OUT_OF_MEMORY) ;
    }
    LAGRAPH_OK (GrB_Vector_extractTuples (I, X, &nvals, r)) ;
    LAGRAPH_OK (GrB_free (&r)) ;
    #endif

    // this will always be true since r is dense, but check anyway:
    if (nvals != n) return (GrB_PANIC) ;

    // P = struct (X,I)
    P = malloc_function (n * sizeof (LAGraph_PageRank)) ;
    if (P == NULL)
    {
        LAGRAPH_ERROR ("out of memory", GrB_OUT_OF_MEMORY) ;
    }
    for (int64_t k = 0 ; k < nvals ; k++)
    {
        // The kth ranked page is P[k].page (with k=0 being the highest rank),
        // and its pagerank is P[k].pagerank.
        P [k].pagerank = X [k] ;
        // I [k] == k will be true for SuiteSparse:GraphBLAS but in general I
        // can be returned in any order, so use I [k] instead of k, for other
        // GraphBLAS implementations.
        #ifdef GxB_SUITESPARSE_GRAPHBLAS
        P [k].page = k ;
        #else
        P [k].page = I [k] ;
        #endif
    }

    // free workspace
    FREEWORK ;

    // qsort (P) in descending order
    qsort (P, n, sizeof (LAGraph_PageRank), compar) ;

    //--------------------------------------------------------------------------
    // return result
    //--------------------------------------------------------------------------

    tt = LAGraph_toc (tic) ;
    fprintf (stderr, "fini time %g\n", tt) ;

    (*Phandle) = P ;
    return (GrB_SUCCESS) ;
}

