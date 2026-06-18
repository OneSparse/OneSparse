#http://blog.pgxn.org/post/4783001135/extension-makefiles pg makefiles

EXTENSION = onesparse
PG_CONFIG ?= pg_config
DATA = $(wildcard onesparse/*--*.sql)
PGXS := $(shell $(PG_CONFIG) --pgxs)
MODULE_big = onesparse
OBJS = $(patsubst %.c,%.o,$(shell find src -name '*.c'))
SHLIB_LINK = -lc -lgraphblas -llagraph -llagraphx -lpq
PG_CPPFLAGS = -Wfatal-errors -std=c11 -I/usr/local/include/suitesparse/

# Experimental: build the matrix_sum aggregate, which requires the
# LAGraph_Matrix_Sum primitive from the LAGraph "matrix-sum" branch
# (https://github.com/GraphBLAS/LAGraph/pull/407). Off by default so stock
# LAGraph builds still compile. Enable with: make WITH_MATRIX_SUM=1
WITH_MATRIX_SUM ?= 0
ifeq ($(WITH_MATRIX_SUM),1)
PG_CPPFLAGS += -DONESPARSE_HAVE_LAGRAPH_MATRIX_SUM
endif

TESTS        = $(wildcard sql/*.sql)
REGRESS      = $(patsubst sql/%.sql,%,$(TESTS))
# REGRESS_OPTS = --load-language=plpgsql
include $(PGXS)
