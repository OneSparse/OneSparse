#http://blog.pgxn.org/post/4783001135/extension-makefiles pg makefiles

EXTENSION = onesparse
PG_CONFIG ?= pg_config
DATA = $(wildcard onesparse/*--*.sql)
PGXS := $(shell $(PG_CONFIG) --pgxs)
MODULE_big = onesparse
OBJS = $(patsubst %.c,%.o,$(shell find src -name '*.c'))
SHLIB_LINK = -lc -lgraphblas -llagraph -llagraphx -lpq
PG_CPPFLAGS = -Wfatal-errors -std=c11 -I/usr/local/include/suitesparse/

ifeq ($(COVERAGE),1)
PG_CPPFLAGS += --coverage
SHLIB_LINK += --coverage
endif

TESTS        = $(wildcard sql/*.sql)
REGRESS      = $(patsubst sql/%.sql,%,$(TESTS))
UNIT_TEST_RUNNER = ./test/unit/run.sh

# REGRESS_OPTS = --load-language=plpgsql
include $(PGXS)

.PHONY: unitcheck check

unitcheck:
	$(UNIT_TEST_RUNNER)

check: installcheck unitcheck
