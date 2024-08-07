#http://blog.pgxn.org/post/4783001135/extension-makefiles pg makefiles

EXTENSION = onesparse
PG_CONFIG ?= pg_config
DATA = $(wildcard *--*.sql)
PGXS := $(shell $(PG_CONFIG) --pgxs)
MODULE_big = onesparse
OBJS = $(patsubst %.c,%.o,$(wildcard src/onesparse.c))
SHLIB_LINK = -lgraphblas
PG_CPPFLAGS = -O0 -Wfatal-errors

TESTS        = $(wildcard test/sql/*.sql)
REGRESS      = $(patsubst test/sql/%.sql,%,$(TESTS))
REGRESS_OPTS = --inputdir=test --load-language=plpgsql
include $(PGXS)

sql: onesparse--0.0.1.sql
	bash onesparse--0.0.1-gen.sql.sh
