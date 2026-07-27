# Arca - GNU Make build system
# Portable to Windows & POSIX environments (Linux, macOS, WSL, MSYS2)

ifeq ($(OS),Windows_NT)
    SHELL := powershell.exe
    .SHELLFLAGS := -NoProfile -ExecutionPolicy Bypass -Command

    define MKDIR_P
        @New-Item -ItemType Directory -Force -Path "$(1)" | Out-Null
    endef

    define RM_RF
        @if (Test-Path "$(1)") { Remove-Item -Recurse -Force "$(1)" }
    endef

else
    SHELL := /bin/sh

    define MKDIR_P
        @mkdir -p "$(1)"
    endef

    define RM_RF
        @rm -rf "$(1)"
    endef
endif

CC      := gcc
AR      := ar
ARFLAGS := rcs

PROJECT_NAME := arca

SRC_DIR        := src
INCLUDE_DIR    := include/$(PROJECT_NAME)
EXAMPLES_DIR   := examples
TESTS_DIR      := tests
BENCHMARKS_DIR := benchmarks

BUILD_DIR      := build
BIN_DIR        := $(BUILD_DIR)/bin
OBJ_DIR        := $(BUILD_DIR)/obj
LIB_DIR        := $(BUILD_DIR)/lib
BIN_EXAMPLES   := $(BIN_DIR)/examples
BIN_TESTS      := $(BIN_DIR)/tests
BIN_BENCH      := $(BIN_DIR)/benchmarks

LIB_TARGET := $(LIB_DIR)/lib$(PROJECT_NAME).a

COMMON_CFLAGS    := -std=c23 -Wall -Wextra -Wpedantic -Werror
LIB_CFLAGS       := $(COMMON_CFLAGS) -O2
EXAMPLE_CFLAGS   := $(COMMON_CFLAGS) -O3
TEST_CFLAGS      := $(COMMON_CFLAGS) -O0 -g3
BENCHMARK_CFLAGS := $(COMMON_CFLAGS) -O3

CPPFLAGS := -I$(INCLUDE_DIR)
LDFLAGS  :=
LDLIBS   := -lm
DEPFLAGS := -MMD -MP

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

LIB_SRCS := $(call rwildcard,$(SRC_DIR),*.c)
LIB_OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(LIB_SRCS))

EXAMPLE_SRCS := $(call rwildcard,$(EXAMPLES_DIR),*.c)
EXAMPLE_OBJS := $(patsubst $(EXAMPLES_DIR)/%.c,$(OBJ_DIR)/examples/%.o,$(EXAMPLE_SRCS))
EXAMPLE_BINS := $(patsubst $(EXAMPLES_DIR)/%.c,$(BIN_EXAMPLES)/%,$(EXAMPLE_SRCS))

TEST_SRCS := $(call rwildcard,$(TESTS_DIR),*.c)
TEST_OBJS := $(patsubst $(TESTS_DIR)/%.c,$(OBJ_DIR)/tests/%.o,$(TEST_SRCS))
TEST_BINS := $(patsubst $(TESTS_DIR)/%.c,$(BIN_TESTS)/%,$(TEST_SRCS))

BENCH_SRCS := $(call rwildcard,$(BENCHMARKS_DIR),*.c)
BENCH_OBJS := $(patsubst $(BENCHMARKS_DIR)/%.c,$(OBJ_DIR)/benchmarks/%.o,$(BENCH_SRCS))
BENCH_BINS := $(patsubst $(BENCHMARKS_DIR)/%.c,$(BIN_BENCH)/%,$(BENCH_SRCS))

DEPS := $(LIB_OBJS:.o=.d) $(EXAMPLE_OBJS:.o=.d) $(TEST_OBJS:.o=.d) $(BENCH_OBJS:.o=.d)


# --- Commands ---
.PHONY: all build examples test benchmark remove clean rebuild install uninstall

all: build

build: $(LIB_TARGET)
	@echo "Library Build completed."

examples: $(EXAMPLE_BINS)
	@echo "Examples Build completed."

test: $(TEST_BINS)
	@$(foreach t,$(TEST_BINS),\
		echo "";\
		echo "Running Test: $(t)" ;\
		$(t) ;\
		echo "" ;\
	)
	@echo "All tests passed."

benchmark: $(BENCH_BINS)
	@$(foreach b,$(BENCH_BINS),\
		echo "";\
		echo "Running Benchmark: $(b)" ;\
		$(b) ;\
		echo "" ;\
	)
	@echo "Benchmarking completed."

# Remove build artifacts
remove:
	$(call RM_RF,$(BUILD_DIR)/obj)
	@echo "Removed build artifacts."

# Remove all build artifacts and binaries
clean:
	$(call RM_RF,$(BUILD_DIR))
	@echo "Cleaned all build artifacts and binaries."

rebuild: clean build

install:
	@echo "TODO: install"

uninstall:
	@echo "TODO: uninstall"


# --- Files ---
$(LIB_TARGET): $(LIB_OBJS)
	$(call MKDIR_P,$(dir $@))
	$(AR) $(ARFLAGS) $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(call MKDIR_P,$(dir $@))
	$(CC) $(CPPFLAGS) $(LIB_CFLAGS) $(DEPFLAGS) -c $< -o $@

$(OBJ_DIR)/examples/%.o: $(EXAMPLES_DIR)/%.c
	$(call MKDIR_P,$(dir $@))
	$(CC) $(CPPFLAGS) $(EXAMPLE_CFLAGS) $(DEPFLAGS) -c $< -o $@

$(OBJ_DIR)/tests/%.o: $(TESTS_DIR)/%.c
	$(call MKDIR_P,$(dir $@))
	$(CC) $(CPPFLAGS) $(TEST_CFLAGS) $(DEPFLAGS) -c $< -o $@

$(OBJ_DIR)/benchmarks/%.o: $(BENCHMARKS_DIR)/%.c
	$(call MKDIR_P,$(dir $@))
	$(CC) $(CPPFLAGS) $(BENCHMARK_CFLAGS) $(DEPFLAGS) -c $< -o $@

$(BIN_EXAMPLES)/%: $(OBJ_DIR)/examples/%.o $(LIB_TARGET)
	$(call MKDIR_P,$(dir $@))
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BIN_TESTS)/%: $(OBJ_DIR)/tests/%.o $(LIB_TARGET)
	$(call MKDIR_P,$(dir $@))
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BIN_BENCH)/%: $(OBJ_DIR)/benchmarks/%.o $(LIB_TARGET)
	$(call MKDIR_P,$(dir $@))
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

-include $(DEPS)
