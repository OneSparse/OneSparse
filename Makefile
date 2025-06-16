#http://blog.pgxn.org/post/4783001135/extension-makefiles pg makefiles

EXTENSION = onesparse
PG_CONFIG ?= pg_config
DATA = $(wildcard onesparse/*--*.sql)
PGXS := $(shell $(PG_CONFIG) --pgxs)
MODULE_big = onesparse
OBJS = $(patsubst %.c,%.o,$(shell find src -name '*.c'))
SHLIB_LINK = -lc -lgraphblas -llagraph -lpq
PG_CPPFLAGS = -Wfatal-errors -std=c11 -I/usr/local/include/suitesparse/

TESTS        = $(wildcard sql/*.sql)
REGRESS      = $(patsubst sql/%.sql,%,$(TESTS))
# REGRESS_OPTS = --load-language=plpgsql
include $(PGXS)
