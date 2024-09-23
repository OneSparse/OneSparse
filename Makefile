#http://blog.pgxn.org/post/4783001135/extension-makefiles pg makefiles

EXTENSION = onesparse
PG_CONFIG ?= pg_config
DATA = $(wildcard onesparse/*--*.sql)
PGXS := $(shell $(PG_CONFIG) --pgxs)
MODULE_big = onesparse
OBJS = $(patsubst %.c,%.o,$(wildcard src/*.c))
SHLIB_LINK = -lgraphblas -lpq
PG_CPPFLAGS = -O0 -Wfatal-errors

TESTS        = $(wildcard sql/*.sql)
REGRESS      = $(patsubst sql/%.sql,%,$(TESTS))
# REGRESS_OPTS = --load-language=plpgsql
include $(PGXS)
