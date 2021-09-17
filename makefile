MAKEFILE_PATH:=$(abspath $(lastword $(MAKEFILE_LIST)))
APP:=$(notdir $(patsubst %/,%,$(dir $(MAKEFILE_PATH))))
TEST_EXE:=bin/test_$(APP)
COVERAGE_EXE:=bin/coverage_$(APP)

include cfg/cfg.mk

CC=x86_64-w64-mingw32-gcc
CFLAGS=-Wall -Werror -pedantic -std=c11
CFLAGS_COVERAGE=-coverage -fprofile-arcs -O0
CFLAGS_DEBUG=-g -ggdb
BUILDCMD=${CC} ${CFLAGS_OUTPUT} ${CFLAGS} ${INCLUDES} $^ ${LIBRARIES} ${FRAMEWORKS}

all: docs coverage test

%.a: CFLAGS_OUTPUT := -c
%.a: $(SOURCE)
	mkdir -p tmp
	mkdir -p bin
	$(BUILDCMD)
	mv *.o tmp
	ar rvs $@ tmp/*.o
build: bin/lib$(APP).a

debug: CFLAGS+=$(CFLAGS_DEBUG)
debug: build

# Build unit test executable and link with library using release parameters.
$(TEST_EXE): CFLAGS_OUTPUT := -o $(TEST_EXE)
$(TEST_EXE): LIBRARIES := $(LIBRARIES) -L bin -l$(APP)
$(TEST_EXE): $(TEST_SOURCE) bin/lib$(APP).a
	$(BUILDCMD)
test: $(TEST_EXE)
	./$<

# Build unit test executable and link with library using coverage parameters.
$(COVERAGE_EXE): CFLAGS_OUTPUT := -o $(COVERAGE_EXE)
$(COVERAGE_EXE): LIBRARIES := $(LIBRARIES) -L tmp -l$(APP)
$(COVERAGE_EXE): $(TEST_SOURCE) tmp/lib$(APP).a
	$(BUILDCMD)

# Generate coverage report.
bin/coverage.html: CFLAGS+=$(CFLAGS_DEBUG) $(CFLAGS_COVERAGE)
bin/coverage.html: $(COVERAGE_EXE)
	./$<
	mv *.gcno tmp
	mv *.gcda tmp
	py -m gcovr \
		--root . \
		--object-directory tmp \
		--exclude=".*/*test.c" \
		--html \
		--html-details \
		--html-title "Test Results" \
		--sort-percentage \
		-j 4 \
		--output bin/coverage.html \
		--print-summary
coverage: bin/coverage.html

# Generate documentation.
docs:
	mkdir -p bin
	cp doc/*.css bin
	pandoc doc/documentation.md \
		--metadata pagetitle="${APP} Documentation" \
		--filter pandoc-citeproc \
		--table-of-contents \
		-s \
		--mathjax \
		--css x-dark.css \
		-o bin/documentation-dark.html
	pandoc doc/documentation.md \
		--metadata pagetitle="${APP} Documentation" \
		--filter pandoc-citeproc \
		--table-of-contents \
		-s \
		--mathjax \
		--css x.css \
		-o bin/documentation.html

clean:
	rm -rf bin
	rm -rf tmp
	rm -f *.gcda # If Tests fail .gcda and .gcno are
	rm -f *.gcno # left in project root.
	rm -rf *.dSYM

$(V).SILENT:
