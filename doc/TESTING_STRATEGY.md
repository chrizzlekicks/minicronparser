# Mini Cron Parser - Testing Strategy with Unity

**Date:** December 2025
**Version:** 1.0
**Framework:** Unity Test Framework

---

## Table of Contents

1. [Overview](#overview)
2. [Why Unity?](#why-unity)
3. [Setup Guide](#setup-guide)
4. [Test Architecture](#test-architecture)
5. [Unit Testing Strategy](#unit-testing-strategy)
6. [Integration Testing](#integration-testing)
7. [Security & Edge Cases](#security--edge-cases)
8. [Running Tests](#running-tests)
9. [CI/CD Integration](#cicd-integration)
10. [Best Practices](#best-practices)

---

## Overview

### Testing Goals

1. **Prevent Regressions** - Ensure changes don't break existing functionality
2. **Document Behavior** - Tests serve as executable specifications
3. **Enable Refactoring** - Safely improve code with test coverage
4. **Find Bugs Early** - Catch issues before production

### Current Status

❌ **No tests exist**

### Target Coverage

| Component | Target | Priority |
|-----------|--------|----------|
| Initialization | 100% | High |
| Insertion | 100% | High |
| Parsing logic | 95%+ | Critical |
| Memory management | 100% | Critical |
| Error handling | 90%+ | High |
| Edge cases | 80%+ | Medium |

### Test Pyramid Strategy

```
        /\
       /  \
      / E2E \           5% - End-to-end (full program)
     /------\          ~5 tests
    /        \
   / Integration \      15% - Multiple components
  /-------------\      ~15 tests
 /               \
/  Unit Tests     \     80% - Individual functions
-------------------    ~80 tests
```

**Total:** ~100 test cases
**Test code:** ~2000 lines (10:1 ratio with production code)

---

## Why Unity?

### Framework Comparison

| Framework | Pros | Cons | Verdict |
|-----------|------|------|---------|
| **Unity** | Simple, portable, no dependencies | No mocking, manual registration | ⭐ **RECOMMENDED** |
| CMocka | Mocking, memory leak detection | Requires CMake, heavier | Good alternative |
| Check | Crash-safe (fork), XML output | Complex setup, slower | Overkill |
| Custom | No dependencies, full control | Reinventing wheel, missing features | Not recommended |

### Why Unity is Best for This Project

✅ **Lightweight** - Just 2 files (unity.c, unity.h)
✅ **No dependencies** - Works everywhere
✅ **Easy to learn** - Minimal API surface
✅ **Portable** - Embedded to desktop
✅ **Sufficient features** - Has everything we need
✅ **Active development** - Well-maintained

### Unity Features

- Assertions (integer, string, pointer, memory)
- Setup/teardown hooks
- Test filtering
- Colored output
- Custom messages
- Extensible

---

## Setup Guide

### Step 1: Add Unity as Submodule

```bash
# From project root
git submodule add https://github.com/ThrowTheSwitch/Unity.git test/unity
git submodule update --init --recursive
```

**Alternative (without Git submodules):**
```bash
# Download Unity
mkdir -p test/unity
cd test/unity
wget https://raw.githubusercontent.com/ThrowTheSwitch/Unity/master/src/unity.c
wget https://raw.githubusercontent.com/ThrowTheSwitch/Unity/master/src/unity.h
wget https://raw.githubusercontent.com/ThrowTheSwitch/Unity/master/src/unity_internals.h
```

---

### Step 2: Create Test Directory Structure

```bash
mkdir -p test/fixtures
touch test/test_init.c
touch test/test_insert.c
touch test/test_reading.c
touch test/test_parsing.c
touch test/test_memory.c
touch test/test_integration.c
touch test/test_security.c
touch test/test_edge_cases.c
touch test/test_runner.c
```

**Final structure:**
```
project/
├── src/
│   ├── main.c
│   └── minicron.c
├── lib/
│   └── minicron.h
├── test/
│   ├── unity/              # Unity framework (submodule)
│   │   ├── unity.c
│   │   ├── unity.h
│   │   └── unity_internals.h
│   ├── test_init.c         # Test initialization functions
│   ├── test_insert.c       # Test insertion functions
│   ├── test_reading.c      # Test file reading
│   ├── test_parsing.c      # Test parsing logic
│   ├── test_memory.c       # Test memory management
│   ├── test_integration.c  # Integration tests
│   ├── test_security.c     # Security tests
│   ├── test_edge_cases.c   # Edge case tests
│   ├── test_runner.c       # Main test runner
│   └── fixtures/           # Test input files
│       ├── valid_input.txt
│       ├── malformed.txt
│       └── empty.txt
├── Makefile
└── TESTING_STRATEGY.md     # This file
```

---

### Step 3: Create Test Fixtures

**test/fixtures/valid_input.txt:**
```
30 1 /bin/run_me_daily
45 * /bin/run_me_hourly
0 12 /bin/noon_task
* 18 /bin/evening_task
```

**test/fixtures/empty.txt:**
```
(empty file)
```

**test/fixtures/malformed.txt:**
```
30 1 /bin/valid_line
invalid line here
45
30 * /bin/another_valid
```

**test/fixtures/large_input.txt** (generate programmatically):
```bash
for i in {0..1000}; do
    echo "$((i % 60)) $((i % 24)) /bin/task$i"
done > test/fixtures/large_input.txt
```

---

### Step 4: Update Makefile

```makefile
CC = clang
CFLAGS = -g -Wall -Wextra -Werror -pedantic
OBJ = src/*.c
BIN = main
TEST_DIR = test
UNITY_DIR = $(TEST_DIR)/unity

# Source files for testing (exclude main.c)
TEST_SOURCES = src/minicron.c
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)

# Test files
TEST_FILES = $(TEST_DIR)/test_init.c \
             $(TEST_DIR)/test_insert.c \
             $(TEST_DIR)/test_reading.c \
             $(TEST_DIR)/test_parsing.c \
             $(TEST_DIR)/test_memory.c \
             $(TEST_DIR)/test_integration.c \
             $(TEST_DIR)/test_security.c \
             $(TEST_DIR)/test_edge_cases.c \
             $(TEST_DIR)/test_runner.c

# Unity framework
UNITY_SRC = $(UNITY_DIR)/unity.c

all: $(BIN)

main:
	mkdir -p bin
	cp -r input.txt bin
	$(CC) $(CFLAGS) $(OBJ) -o bin/$(BIN)

# Build test executable
test: $(BIN)_test
	./$(BIN)_test

$(BIN)_test: $(TEST_FILES) $(TEST_OBJECTS) $(UNITY_SRC)
	$(CC) $(CFLAGS) -I$(UNITY_DIR) -I./lib $(TEST_FILES) $(TEST_OBJECTS) $(UNITY_SRC) -o $(BIN)_test

# Memory leak detection with Valgrind
test-memory: $(BIN)_test
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --error-exitcode=1 ./$(BIN)_test

# Code coverage with gcov
test-coverage: CFLAGS += --coverage
test-coverage: clean test
	gcov src/minicron.c
	@echo "Coverage report generated: minicron.c.gcov"
	@grep -A 3 "Lines executed" *.gcov

# Static analysis
analyze:
	clang --analyze $(CFLAGS) src/*.c

# Run all quality checks
check: test test-memory analyze
	@echo "✅ All checks passed!"

# Quick test (no memory check, faster feedback)
test-quick: $(BIN)_test
	./$(BIN)_test

clean:
	rm -rf bin
	rm -f $(BIN)_test
	rm -f *.o src/*.o test/*.o
	rm -f *.gcov *.gcda *.gcno
	rm -f src/*.gcda src/*.gcno

.PHONY: all test test-memory test-coverage analyze check test-quick clean
```

---

## Test Architecture

### Test File Organization

Each test file focuses on a specific component:

| File | Purpose | Test Count |
|------|---------|------------|
| `test_init.c` | Initialization functions | ~5 tests |
| `test_insert.c` | Linked list insertion | ~8 tests |
| `test_reading.c` | File reading logic | ~10 tests |
| `test_parsing.c` | Time parsing & normalization | ~25 tests |
| `test_memory.c` | Memory management | ~8 tests |
| `test_integration.c` | End-to-end pipeline | ~15 tests |
| `test_security.c` | Security vulnerabilities | ~10 tests |
| `test_edge_cases.c` | Edge cases & boundaries | ~20 tests |

---

### Basic Test Structure

```c
#include "unity.h"
#include "../lib/minicron.h"

// Runs before each test
void setUp(void) {
    // Initialize test state
}

// Runs after each test
void tearDown(void) {
    // Cleanup test state
}

// A single test
void test_something(void) {
    // Arrange
    int expected = 42;

    // Act
    int actual = some_function();

    // Assert
    TEST_ASSERT_EQUAL(expected, actual);
}

// Test runner calls this
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_something);
    return UNITY_END();
}
```

---

### Unity Assertions Reference

```c
// Integer assertions
TEST_ASSERT_EQUAL(expected, actual)
TEST_ASSERT_NOT_EQUAL(expected, actual)
TEST_ASSERT_EQUAL_INT(expected, actual)
TEST_ASSERT_GREATER_THAN(threshold, actual)
TEST_ASSERT_LESS_THAN(threshold, actual)

// Boolean assertions
TEST_ASSERT_TRUE(condition)
TEST_ASSERT_FALSE(condition)
TEST_ASSERT(condition)

// Pointer assertions
TEST_ASSERT_NULL(pointer)
TEST_ASSERT_NOT_NULL(pointer)
TEST_ASSERT_EQUAL_PTR(expected, actual)

// String assertions
TEST_ASSERT_EQUAL_STRING(expected, actual)
TEST_ASSERT_EQUAL_STRING_LEN(expected, actual, length)

// Memory assertions
TEST_ASSERT_EQUAL_MEMORY(expected, actual, length)

// Float assertions
TEST_ASSERT_EQUAL_FLOAT(expected, actual)
TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual)

// Custom messages
TEST_ASSERT_EQUAL_MESSAGE(expected, actual, "Custom message")

// Always pass/fail
TEST_PASS()
TEST_FAIL()
TEST_FAIL_MESSAGE("Reason")
```

---

## Unit Testing Strategy

### Important: Refactor for Testability

The current code has side effects that make testing difficult. We need to refactor first.

#### Problem 1: parse_jobs() Does Too Much

**Current:**
```c
void parse_jobs(char *current_time, cron_jobs *src, parsed_jobs *dest) {
    // Validates format
    // Parses time
    // Normalizes time
    // Prints debug output (side effect!)
    // Applies business logic
    // Builds output list
}
```

**Refactored:**
```c
// In minicron.h - add these declarations:
typedef struct {
    int hour;
    int minute;
    int is_valid;
} parsed_time;

parsed_time parse_time_string(char *time_str);
void normalize_time(int *hour, int *minute);
char* determine_day(int job_hour, int current_hour);
```

**Benefits:**
- Each function is independently testable
- No side effects
- Pure functions (same input → same output)
- Easier to understand

---

#### Problem 2: read_input() Has File I/O

**Current:**
```c
void read_input(char *filename, cron_jobs *jobs) {
    FILE *fp = fopen(filename, "r");
    // ... read and parse
    fclose(fp);
}
```

**Refactored:**
```c
// In minicron.h - add:
int read_input_from_stream(FILE *fp, cron_jobs *jobs);

// In minicron.c - refactor:
void read_input(char *filename, cron_jobs *jobs) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Could not find input file\n");
        exit(1);
    }

    int result = read_input_from_stream(fp, jobs);
    fclose(fp);

    if (result != 0) {
        exit(1);
    }
}

int read_input_from_stream(FILE *fp, cron_jobs *jobs) {
    // Same logic, but takes FILE* instead of filename
    // Returns error code instead of exit()
}
```

**Benefits:**
- Can test with `tmpfile()` or in-memory streams
- No actual file system access needed
- Faster tests
- Can test error conditions

---

### Test Suite 1: Initialization (test_init.c)

```c
#include "unity.h"
#include "../lib/minicron.h"

void setUp(void) {}
void tearDown(void) {}

// Test: init_jobs creates empty list
void test_init_jobs_creates_empty_list(void) {
    cron_jobs jobs;
    init_jobs(&jobs);

    TEST_ASSERT_NULL(jobs.first);
    TEST_ASSERT_NULL(jobs.last);
}

// Test: init_parsed_jobs creates empty list
void test_init_parsed_jobs_creates_empty_list(void) {
    parsed_jobs pJobs;
    init_parsed_jobs(&pJobs);

    TEST_ASSERT_NULL(pJobs.first);
    TEST_ASSERT_NULL(pJobs.last);
}

// Test: multiple initializations are safe
void test_multiple_inits_are_safe(void) {
    cron_jobs jobs;
    init_jobs(&jobs);
    init_jobs(&jobs);  // Should not crash

    TEST_ASSERT_NULL(jobs.first);
    TEST_ASSERT_NULL(jobs.last);
}

// Test: init after population resets list
void test_init_after_population_resets(void) {
    cron_jobs jobs;
    init_jobs(&jobs);

    // Add a job
    cron_job *job = malloc(sizeof(cron_job));
    jobs.first = job;
    jobs.last = job;

    // Re-initialize
    init_jobs(&jobs);

    TEST_ASSERT_NULL(jobs.first);
    TEST_ASSERT_NULL(jobs.last);

    // Note: This is a memory leak in current implementation!
    // The job we allocated is now unreachable
    free(job);  // Clean up for this test
}
```

**Run this test:**
```bash
clang -g -Wall -Itest/unity -Ilib \
  test/test_init.c src/minicron.c test/unity/unity.c \
  -o test_init && ./test_init
```

---

### Test Suite 2: Insertion (test_insert.c)

```c
#include "unity.h"
#include "../lib/minicron.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

// Test: insert single job
void test_insert_single_job(void) {
    cron_jobs jobs;
    init_jobs(&jobs);

    cron_job *job = malloc(sizeof(cron_job));
    strcpy(job->minute, "30");
    strcpy(job->hour, "1");
    strcpy(job->fire_task, "/bin/test");

    insert_jobs(job, &jobs);

    TEST_ASSERT_NOT_NULL(jobs.first);
    TEST_ASSERT_NOT_NULL(jobs.last);
    TEST_ASSERT_EQUAL_PTR(jobs.first, jobs.last);
    TEST_ASSERT_NULL(jobs.first->next);
    TEST_ASSERT_EQUAL_STRING("30", jobs.first->minute);
    TEST_ASSERT_EQUAL_STRING("1", jobs.first->hour);
    TEST_ASSERT_EQUAL_STRING("/bin/test", jobs.first->fire_task);

    free(job);
}

// Test: insert multiple jobs maintains order (FIFO)
void test_insert_multiple_jobs_maintains_order(void) {
    cron_jobs jobs;
    init_jobs(&jobs);

    cron_job *job1 = malloc(sizeof(cron_job));
    strcpy(job1->fire_task, "/bin/first");
    cron_job *job2 = malloc(sizeof(cron_job));
    strcpy(job2->fire_task, "/bin/second");
    cron_job *job3 = malloc(sizeof(cron_job));
    strcpy(job3->fire_task, "/bin/third");

    insert_jobs(job1, &jobs);
    insert_jobs(job2, &jobs);
    insert_jobs(job3, &jobs);

    // Verify order: first → second → third
    TEST_ASSERT_EQUAL_PTR(job1, jobs.first);
    TEST_ASSERT_EQUAL_PTR(job3, jobs.last);
    TEST_ASSERT_EQUAL_PTR(job2, job1->next);
    TEST_ASSERT_EQUAL_PTR(job3, job2->next);
    TEST_ASSERT_NULL(job3->next);

    free(job1);
    free(job2);
    free(job3);
}

// Test: insert to uninitialized list (documents current behavior)
void test_insert_to_uninitialized_list(void) {
    cron_jobs jobs;
    // NOTE: Not calling init_jobs()
    jobs.first = NULL;
    jobs.last = NULL;

    cron_job *job = malloc(sizeof(cron_job));
    strcpy(job->fire_task, "/bin/test");

    insert_jobs(job, &jobs);

    TEST_ASSERT_EQUAL_PTR(job, jobs.first);
    TEST_ASSERT_EQUAL_PTR(job, jobs.last);

    free(job);
}

// Test: insert 1000 jobs (stress test)
void test_insert_many_jobs(void) {
    cron_jobs jobs;
    init_jobs(&jobs);

    for (int i = 0; i < 1000; i++) {
        cron_job *job = malloc(sizeof(cron_job));
        sprintf(job->fire_task, "/bin/task%d", i);
        insert_jobs(job, &jobs);
    }

    // Verify count
    int count = 0;
    cron_job *job = jobs.first;
    while (job != NULL) {
        count++;
        job = job->next;
    }

    TEST_ASSERT_EQUAL(1000, count);

    // Cleanup
    free_jobs(&jobs);
}
```

---

### Test Suite 3: File Reading (test_reading.c)

**Important:** First implement `read_input_from_stream()` as described in the refactoring section.

```c
#include "unity.h"
#include "../lib/minicron.h"
#include <stdio.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

// Test: read valid input
void test_read_valid_input(void) {
    cron_jobs jobs;
    init_jobs(&jobs);

    // Create temporary file
    FILE *fp = tmpfile();
    fprintf(fp, "30 1 /bin/task1\n");
    fprintf(fp, "45 * /bin/task2\n");
    rewind(fp);

    int result = read_input_from_stream(fp, &jobs);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_NOT_NULL(jobs.first);
    TEST_ASSERT_EQUAL_STRING("30", jobs.first->minute);
    TEST_ASSERT_EQUAL_STRING("1", jobs.first->hour);
    TEST_ASSERT_EQUAL_STRING("/bin/task1", jobs.first->fire_task);

    TEST_ASSERT_NOT_NULL(jobs.first->next);
    TEST_ASSERT_EQUAL_STRING("45", jobs.first->next->minute);
    TEST_ASSERT_EQUAL_STRING("*", jobs.first->next->hour);

    fclose(fp);
    free_jobs(&jobs);
}

// Test: read empty file
void test_read_empty_file(void) {
    cron_jobs jobs;
    init_jobs(&jobs);

    FILE *fp = tmpfile();
    // Empty file - nothing written

    int result = read_input_from_stream(fp, &jobs);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_NULL(jobs.first);
    TEST_ASSERT_NULL(jobs.last);

    fclose(fp);
}

// Test: read malformed input (only 2 fields) - should skip line
void test_read_malformed_input_skips_line(void) {
    cron_jobs jobs;
    init_jobs(&jobs);

    FILE *fp = tmpfile();
    fprintf(fp, "30 1\n");  // Missing task
    fprintf(fp, "45 2 /bin/valid\n");
    rewind(fp);

    int result = read_input_from_stream(fp, &jobs);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_NOT_NULL(jobs.first);
    TEST_ASSERT_EQUAL_STRING("45", jobs.first->minute);
    TEST_ASSERT_NULL(jobs.first->next);  // Only one valid job

    fclose(fp);
    free_jobs(&jobs);
}

// Test: read line with extra whitespace
void test_read_extra_whitespace(void) {
    cron_jobs jobs;
    init_jobs(&jobs);

    FILE *fp = tmpfile();
    fprintf(fp, "  30   1    /bin/task  \n");
    rewind(fp);

    int result = read_input_from_stream(fp, &jobs);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_NOT_NULL(jobs.first);
    TEST_ASSERT_EQUAL_STRING("30", jobs.first->minute);
    TEST_ASSERT_EQUAL_STRING("1", jobs.first->hour);

    fclose(fp);
    free_jobs(&jobs);
}

// Test: read very long line (buffer overflow protection)
void test_read_long_line_truncates_safely(void) {
    cron_jobs jobs;
    init_jobs(&jobs);

    FILE *fp = tmpfile();
    fprintf(fp, "30 1 /bin/");
    for (int i = 0; i < 300; i++) fprintf(fp, "x");
    fprintf(fp, "\n");
    rewind(fp);

    int result = read_input_from_stream(fp, &jobs);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_NOT_NULL(jobs.first);
    // Task should be truncated to MAX_STR-1 (254 chars)
    TEST_ASSERT_EQUAL(254, strlen(jobs.first->fire_task));

    fclose(fp);
    free_jobs(&jobs);
}

// Test: read file from test fixtures
void test_read_fixtures_valid_input(void) {
    cron_jobs jobs;
    init_jobs(&jobs);

    read_input("test/fixtures/valid_input.txt", &jobs);

    TEST_ASSERT_NOT_NULL(jobs.first);

    // Count jobs
    int count = 0;
    cron_job *job = jobs.first;
    while (job != NULL) {
        count++;
        job = job->next;
    }

    TEST_ASSERT_EQUAL(4, count);  // valid_input.txt has 4 lines

    free_jobs(&jobs);
}
```

---

### Test Suite 4: Parsing Logic (test_parsing.c)

**Important:** First implement helper functions as described in refactoring section.

```c
#include "unity.h"
#include "../lib/minicron.h"

void setUp(void) {}
void tearDown(void) {}

// ===== Time Parsing Tests =====

void test_parse_valid_time(void) {
    parsed_time result = parse_time_string("16:30");

    TEST_ASSERT_EQUAL(1, result.is_valid);
    TEST_ASSERT_EQUAL(16, result.hour);
    TEST_ASSERT_EQUAL(30, result.minute);
}

void test_parse_midnight(void) {
    parsed_time result = parse_time_string("00:00");

    TEST_ASSERT_EQUAL(1, result.is_valid);
    TEST_ASSERT_EQUAL(0, result.hour);
    TEST_ASSERT_EQUAL(0, result.minute);
}

void test_parse_almost_midnight(void) {
    parsed_time result = parse_time_string("23:59");

    TEST_ASSERT_EQUAL(1, result.is_valid);
    TEST_ASSERT_EQUAL(23, result.hour);
    TEST_ASSERT_EQUAL(59, result.minute);
}

void test_parse_invalid_format_no_colon(void) {
    parsed_time result = parse_time_string("1630");

    TEST_ASSERT_EQUAL(0, result.is_valid);
}

void test_parse_invalid_format_letters(void) {
    parsed_time result = parse_time_string("12:3a");

    TEST_ASSERT_EQUAL(0, result.is_valid);
}

void test_parse_null_input(void) {
    parsed_time result = parse_time_string(NULL);

    TEST_ASSERT_EQUAL(0, result.is_valid);
}

void test_parse_empty_string(void) {
    parsed_time result = parse_time_string("");

    TEST_ASSERT_EQUAL(0, result.is_valid);
}

void test_parse_out_of_bounds_hour(void) {
    parsed_time result = parse_time_string("25:00");

    TEST_ASSERT_EQUAL(0, result.is_valid);
}

void test_parse_out_of_bounds_minute(void) {
    parsed_time result = parse_time_string("12:61");

    TEST_ASSERT_EQUAL(0, result.is_valid);
}

void test_parse_negative_hour(void) {
    parsed_time result = parse_time_string("-5:30");

    TEST_ASSERT_EQUAL(0, result.is_valid);
}

void test_parse_leading_zeros(void) {
    parsed_time result = parse_time_string("08:05");

    TEST_ASSERT_EQUAL(1, result.is_valid);
    TEST_ASSERT_EQUAL(8, result.hour);
    TEST_ASSERT_EQUAL(5, result.minute);
}

// ===== Time Normalization Tests =====

void test_normalize_24_60_becomes_01_00(void) {
    int hour = 24, minute = 60;
    normalize_time(&hour, &minute);

    TEST_ASSERT_EQUAL(1, hour);
    TEST_ASSERT_EQUAL(0, minute);
}

void test_normalize_23_60_becomes_00_00(void) {
    int hour = 23, minute = 60;
    normalize_time(&hour, &minute);

    TEST_ASSERT_EQUAL(0, hour);
    TEST_ASSERT_EQUAL(0, minute);
}

void test_normalize_12_60_becomes_13_00(void) {
    int hour = 12, minute = 60;
    normalize_time(&hour, &minute);

    TEST_ASSERT_EQUAL(13, hour);
    TEST_ASSERT_EQUAL(0, minute);
}

void test_normalize_24_00_becomes_00_00(void) {
    int hour = 24, minute = 0;
    normalize_time(&hour, &minute);

    TEST_ASSERT_EQUAL(0, hour);
    TEST_ASSERT_EQUAL(0, minute);
}

void test_normalize_24_30_becomes_00_30(void) {
    int hour = 24, minute = 30;
    normalize_time(&hour, &minute);

    TEST_ASSERT_EQUAL(0, hour);
    TEST_ASSERT_EQUAL(30, minute);
}

void test_normalize_valid_time_unchanged(void) {
    int hour = 14, minute = 30;
    normalize_time(&hour, &minute);

    TEST_ASSERT_EQUAL(14, hour);
    TEST_ASSERT_EQUAL(30, minute);
}

void test_normalize_0_60_becomes_1_00(void) {
    int hour = 0, minute = 60;
    normalize_time(&hour, &minute);

    TEST_ASSERT_EQUAL(1, hour);
    TEST_ASSERT_EQUAL(0, minute);
}

// ===== Day Determination Tests =====

void test_determine_day_job_after_current_is_today(void) {
    char *day = determine_day(16, 10);  // Job at 16:xx, current 10:xx
    TEST_ASSERT_EQUAL_STRING("today", day);
}

void test_determine_day_job_before_current_is_tomorrow(void) {
    char *day = determine_day(10, 16);  // Job at 10:xx, current 16:xx
    TEST_ASSERT_EQUAL_STRING("tomorrow", day);
}

void test_determine_day_same_hour_is_today(void) {
    char *day = determine_day(14, 14);
    TEST_ASSERT_EQUAL_STRING("today", day);
}

void test_determine_day_midnight_job_in_afternoon(void) {
    char *day = determine_day(0, 14);  // Midnight job, current 2pm
    TEST_ASSERT_EQUAL_STRING("tomorrow", day);
}

void test_determine_day_late_night_job_in_morning(void) {
    char *day = determine_day(23, 2);  // 11pm job, current 2am
    TEST_ASSERT_EQUAL_STRING("today", day);
}
```

---

### Test Suite 5: Memory Management (test_memory.c)

```c
#include "unity.h"
#include "../lib/minicron.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

// Test: free_jobs on empty list
void test_free_jobs_empty_list(void) {
    cron_jobs jobs;
    init_jobs(&jobs);

    free_jobs(&jobs);  // Should not crash

    TEST_PASS();
}

// Test: free_jobs with single job
void test_free_jobs_single_job(void) {
    cron_jobs jobs;
    init_jobs(&jobs);

    cron_job *job = malloc(sizeof(cron_job));
    strcpy(job->fire_task, "/bin/test");
    insert_jobs(job, &jobs);

    free_jobs(&jobs);

    // Memory should be freed (verify with Valgrind)
    TEST_PASS();
}

// Test: free_jobs with multiple jobs
void test_free_jobs_multiple_jobs(void) {
    cron_jobs jobs;
    init_jobs(&jobs);

    for (int i = 0; i < 100; i++) {
        cron_job *job = malloc(sizeof(cron_job));
        sprintf(job->fire_task, "/bin/test%d", i);
        insert_jobs(job, &jobs);
    }

    free_jobs(&jobs);

    TEST_PASS();
}

// Test: free_parsed with empty list
void test_free_parsed_empty_list(void) {
    parsed_jobs pJobs;
    init_parsed_jobs(&pJobs);

    free_parsed(&pJobs);

    TEST_PASS();
}

// Test: free_parsed with single job
void test_free_parsed_single_job(void) {
    parsed_jobs pJobs;
    init_parsed_jobs(&pJobs);

    parsed_job *pJob = malloc(sizeof(parsed_job));
    pJob->hour = 10;
    pJob->minute = 30;
    strcpy(pJob->day, "today");
    strcpy(pJob->fire_task, "/bin/test");
    insert_parsed(pJob, &pJobs);

    free_parsed(&pJobs);

    TEST_PASS();
}

// Test: Complete lifecycle (read → parse → free)
void test_complete_memory_lifecycle(void) {
    cron_jobs jobs;
    parsed_jobs pJobs;

    init_jobs(&jobs);
    init_parsed_jobs(&pJobs);

    // Create test data
    FILE *fp = tmpfile();
    fprintf(fp, "30 1 /bin/task1\n");
    fprintf(fp, "45 * /bin/task2\n");
    rewind(fp);

    read_input_from_stream(fp, &jobs);
    parse_jobs("16:10", &jobs, &pJobs);

    // Cleanup
    free_jobs(&jobs);
    free_parsed(&pJobs);
    fclose(fp);

    TEST_PASS();
}
```

**Run with Valgrind:**
```bash
make test-memory
```

---

## Integration Testing

Integration tests verify the complete pipeline works end-to-end.

### Test Suite 6: Integration Tests (test_integration.c)

```c
#include "unity.h"
#include "../lib/minicron.h"

void setUp(void) {}
void tearDown(void) {}

// Test: Complete pipeline with valid input
void test_full_pipeline_valid_input(void) {
    cron_jobs jobs;
    parsed_jobs pJobs;

    init_jobs(&jobs);
    init_parsed_jobs(&pJobs);

    read_input("test/fixtures/valid_input.txt", &jobs);
    TEST_ASSERT_NOT_NULL(jobs.first);

    parse_jobs("16:10", &jobs, &pJobs);
    TEST_ASSERT_NOT_NULL(pJobs.first);

    // Verify first job: 30 1 /bin/run_me_daily
    // At 16:10, 01:30 is tomorrow
    parsed_job *pJob = pJobs.first;
    TEST_ASSERT_EQUAL(1, pJob->hour);
    TEST_ASSERT_EQUAL(30, pJob->minute);
    TEST_ASSERT_EQUAL_STRING("tomorrow", pJob->day);
    TEST_ASSERT_EQUAL_STRING("/bin/run_me_daily", pJob->fire_task);

    free_jobs(&jobs);
    free_parsed(&pJobs);
}

// Test: Pipeline with empty input
void test_full_pipeline_empty_input(void) {
    cron_jobs jobs;
    parsed_jobs pJobs;

    init_jobs(&jobs);
    init_parsed_jobs(&pJobs);

    read_input("test/fixtures/empty.txt", &jobs);
    TEST_ASSERT_NULL(jobs.first);

    parse_jobs("16:10", &jobs, &pJobs);
    TEST_ASSERT_NULL(pJobs.first);

    free_jobs(&jobs);
    free_parsed(&pJobs);
}

// Test: Wildcard minute expansion
void test_wildcard_minute_expansion(void) {
    cron_jobs jobs;
    parsed_jobs pJobs;

    init_jobs(&jobs);
    init_parsed_jobs(&pJobs);

    cron_job *job = malloc(sizeof(cron_job));
    strcpy(job->minute, "*");
    strcpy(job->hour, "10");
    strcpy(job->fire_task, "/bin/test");
    insert_jobs(job, &jobs);

    parse_jobs("14:25", &jobs, &pJobs);

    // Wildcard minute should become current minute (25)
    // Hour 10 < 14, so tomorrow
    TEST_ASSERT_EQUAL(10, pJobs.first->hour);
    TEST_ASSERT_EQUAL(25, pJobs.first->minute);
    TEST_ASSERT_EQUAL_STRING("tomorrow", pJobs.first->day);

    free_jobs(&jobs);
    free_parsed(&pJobs);
}

// Test: Wildcard hour expansion
void test_wildcard_hour_expansion(void) {
    cron_jobs jobs;
    parsed_jobs pJobs;

    init_jobs(&jobs);
    init_parsed_jobs(&pJobs);

    cron_job *job = malloc(sizeof(cron_job));
    strcpy(job->minute, "30");
    strcpy(job->hour, "*");
    strcpy(job->fire_task, "/bin/test");
    insert_jobs(job, &jobs);

    parse_jobs("14:25", &jobs, &pJobs);

    // Wildcard hour should become current hour (14)
    // Fires today (same hour)
    TEST_ASSERT_EQUAL(14, pJobs.first->hour);
    TEST_ASSERT_EQUAL(30, pJobs.first->minute);
    TEST_ASSERT_EQUAL_STRING("today", pJobs.first->day);

    free_jobs(&jobs);
    free_parsed(&pJobs);
}

// Test: Both wildcards
void test_both_wildcards(void) {
    cron_jobs jobs;
    parsed_jobs pJobs;

    init_jobs(&jobs);
    init_parsed_jobs(&pJobs);

    cron_job *job = malloc(sizeof(cron_job));
    strcpy(job->minute, "*");
    strcpy(job->hour, "*");
    strcpy(job->fire_task, "/bin/test");
    insert_jobs(job, &jobs);

    parse_jobs("14:25", &jobs, &pJobs);

    // Both should become current time
    TEST_ASSERT_EQUAL(14, pJobs.first->hour);
    TEST_ASSERT_EQUAL(25, pJobs.first->minute);
    TEST_ASSERT_EQUAL_STRING("today", pJobs.first->day);

    free_jobs(&jobs);
    free_parsed(&pJobs);
}

// Test: Time normalization in pipeline
void test_pipeline_with_time_normalization(void) {
    cron_jobs jobs;
    parsed_jobs pJobs;

    init_jobs(&jobs);
    init_parsed_jobs(&pJobs);

    cron_job *job = malloc(sizeof(cron_job));
    strcpy(job->minute, "30");
    strcpy(job->hour, "10");
    strcpy(job->fire_task, "/bin/test");
    insert_jobs(job, &jobs);

    // Pass time that needs normalization
    parse_jobs("24:60", &jobs, &pJobs);  // Should normalize to 01:00

    // Job at 10:30, current (normalized) 01:00
    // 10 > 1, so today
    TEST_ASSERT_EQUAL(10, pJobs.first->hour);
    TEST_ASSERT_EQUAL(30, pJobs.first->minute);
    TEST_ASSERT_EQUAL_STRING("today", pJobs.first->day);

    free_jobs(&jobs);
    free_parsed(&pJobs);
}

// Test: Large input performance
void test_large_input_performance(void) {
    cron_jobs jobs;
    parsed_jobs pJobs;

    init_jobs(&jobs);
    init_parsed_jobs(&pJobs);

    read_input("test/fixtures/large_input.txt", &jobs);

    // Count jobs
    int count = 0;
    cron_job *job = jobs.first;
    while (job != NULL) {
        count++;
        job = job->next;
    }
    TEST_ASSERT_EQUAL(1001, count);

    parse_jobs("16:10", &jobs, &pJobs);

    // Verify all parsed
    count = 0;
    parsed_job *pJob = pJobs.first;
    while (pJob != NULL) {
        count++;
        pJob = pJob->next;
    }
    TEST_ASSERT_EQUAL(1001, count);

    free_jobs(&jobs);
    free_parsed(&pJobs);
}
```

---

## Security & Edge Cases

### Test Suite 7: Security Tests (test_security.c)

```c
#include "unity.h"
#include "../lib/minicron.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

// Test: Buffer overflow protection
void test_long_input_truncates_safely(void) {
    cron_jobs jobs;
    init_jobs(&jobs);

    FILE *fp = tmpfile();

    // 300-character task path (exceeds MAX_STR=255)
    fprintf(fp, "30 1 /bin/");
    for (int i = 0; i < 300; i++) fprintf(fp, "x");
    fprintf(fp, "\n");
    rewind(fp);

    read_input_from_stream(fp, &jobs);

    TEST_ASSERT_NOT_NULL(jobs.first);
    // Task should be truncated to 254 chars + null
    TEST_ASSERT_EQUAL(254, strlen(jobs.first->fire_task));

    fclose(fp);
    free_jobs(&jobs);
}

// Test: Command injection attempt (documents behavior)
void test_command_injection_stored_literally(void) {
    cron_jobs jobs;
    init_jobs(&jobs);

    FILE *fp = tmpfile();
    fprintf(fp, "30 1 /bin/task;rm_-rf_/\n");
    rewind(fp);

    read_input_from_stream(fp, &jobs);

    // Should store exactly what was input
    // (Note: sscanf will stop at first space, so ';' ends the token)
    TEST_ASSERT_NOT_NULL(jobs.first);
    TEST_ASSERT_EQUAL_STRING("/bin/task;rm_-rf_/", jobs.first->fire_task);

    fclose(fp);
    free_jobs(&jobs);
}

// Test: Path traversal stored literally
void test_path_traversal_stored_literally(void) {
    cron_jobs jobs;
    init_jobs(&jobs);

    FILE *fp = tmpfile();
    fprintf(fp, "30 1 ../../../../etc/passwd\n");
    rewind(fp);

    read_input_from_stream(fp, &jobs);

    TEST_ASSERT_NOT_NULL(jobs.first);
    TEST_ASSERT_EQUAL_STRING("../../../../etc/passwd", jobs.first->fire_task);

    fclose(fp);
    free_jobs(&jobs);
}

// Test: Format string characters preserved
void test_format_string_preserved(void) {
    cron_jobs jobs;
    init_jobs(&jobs);

    FILE *fp = tmpfile();
    fprintf(fp, "30 1 /bin/task%%s%%n\n");
    rewind(fp);

    read_input_from_stream(fp, &jobs);

    TEST_ASSERT_NOT_NULL(jobs.first);
    // Should contain literal % characters
    TEST_ASSERT_TRUE(strchr(jobs.first->fire_task, '%') != NULL);

    fclose(fp);
    free_jobs(&jobs);
}

// Test: Null byte handling
void test_null_byte_in_input(void) {
    cron_jobs jobs;
    init_jobs(&jobs);

    FILE *fp = tmpfile();
    fprintf(fp, "30 1 /bin/task");
    fputc('\0', fp);
    fprintf(fp, "malicious\n");
    rewind(fp);

    read_input_from_stream(fp, &jobs);

    // Should stop at null byte
    TEST_ASSERT_NOT_NULL(jobs.first);
    TEST_ASSERT_EQUAL_STRING("/bin/task", jobs.first->fire_task);

    fclose(fp);
    free_jobs(&jobs);
}
```

---

### Test Suite 8: Edge Cases (test_edge_cases.c)

```c
#include "unity.h"
#include "../lib/minicron.h"

void setUp(void) {}
void tearDown(void) {}

// Test: Boundary times
void test_midnight_boundary(void) {
    parsed_time result = parse_time_string("00:00");
    TEST_ASSERT_EQUAL(1, result.is_valid);
    TEST_ASSERT_EQUAL(0, result.hour);
    TEST_ASSERT_EQUAL(0, result.minute);
}

void test_almost_midnight(void) {
    parsed_time result = parse_time_string("23:59");
    TEST_ASSERT_EQUAL(1, result.is_valid);
    TEST_ASSERT_EQUAL(23, result.hour);
    TEST_ASSERT_EQUAL(59, result.minute);
}

// Test: Edge case normalizations
void test_normalize_0_60(void) {
    int hour = 0, minute = 60;
    normalize_time(&hour, &minute);
    TEST_ASSERT_EQUAL(1, hour);
    TEST_ASSERT_EQUAL(0, minute);
}

void test_normalize_24_59(void) {
    int hour = 24, minute = 59;
    normalize_time(&hour, &minute);
    TEST_ASSERT_EQUAL(0, hour);
    TEST_ASSERT_EQUAL(59, minute);
}

// Test: Empty fields
void test_empty_hour(void) {
    parsed_time result = parse_time_string(":30");
    TEST_ASSERT_EQUAL(0, result.is_valid);
}

void test_empty_minute(void) {
    parsed_time result = parse_time_string("10:");
    TEST_ASSERT_EQUAL(0, result.is_valid);
}

void test_only_colon(void) {
    parsed_time result = parse_time_string(":");
    TEST_ASSERT_EQUAL(0, result.is_valid);
}

// Test: Multiple colons
void test_multiple_colons(void) {
    parsed_time result = parse_time_string("10:30:45");
    // Should parse first two fields
    TEST_ASSERT_EQUAL(1, result.is_valid);
    TEST_ASSERT_EQUAL(10, result.hour);
    TEST_ASSERT_EQUAL(30, result.minute);
}

// Test: Whitespace variations
void test_tab_separated_input(void) {
    cron_jobs jobs;
    init_jobs(&jobs);

    FILE *fp = tmpfile();
    fprintf(fp, "30\t1\t/bin/task\n");
    rewind(fp);

    read_input_from_stream(fp, &jobs);
    TEST_ASSERT_NOT_NULL(jobs.first);

    fclose(fp);
    free_jobs(&jobs);
}

// Test: Unicode in task path
void test_unicode_in_task(void) {
    cron_jobs jobs;
    init_jobs(&jobs);

    FILE *fp = tmpfile();
    fprintf(fp, "30 1 /bin/täsk\n");  // German umlaut
    rewind(fp);

    read_input_from_stream(fp, &jobs);

    TEST_ASSERT_NOT_NULL(jobs.first);
    TEST_ASSERT_EQUAL_STRING("/bin/täsk", jobs.first->fire_task);

    fclose(fp);
    free_jobs(&jobs);
}

// Test: Very large numbers
void test_huge_hour(void) {
    parsed_time result = parse_time_string("999999:30");
    TEST_ASSERT_EQUAL(0, result.is_valid);
}

void test_huge_minute(void) {
    parsed_time result = parse_time_string("10:999999");
    TEST_ASSERT_EQUAL(0, result.is_valid);
}

// Test: Negative numbers
void test_negative_values(void) {
    parsed_time result = parse_time_string("-10:-30");
    TEST_ASSERT_EQUAL(0, result.is_valid);
}
```

---

## Running Tests

### Basic Test Run

```bash
# Build and run all tests
make test

# Output:
# unity.c:1234: test_init_jobs_creates_empty_list:PASS
# unity.c:1235: test_insert_single_job:PASS
# ...
# ----------------------
# 100 Tests 0 Failures 0 Ignored
# OK
```

---

### Memory Leak Detection

```bash
# Run with Valgrind
make test-memory

# Output:
# ==12345== HEAP SUMMARY:
# ==12345==     in use at exit: 0 bytes in 0 blocks
# ==12345==   total heap usage: 1,234 allocs, 1,234 frees, 45,678 bytes allocated
# ==12345==
# ==12345== All heap blocks were freed -- no leaks are possible
```

---

### Code Coverage

```bash
# Generate coverage report
make test-coverage

# Output:
# File 'src/minicron.c'
# Lines executed:95.23% of 105
# Creating 'minicron.c.gcov'
```

**View detailed coverage:**
```bash
cat minicron.c.gcov | less

# Lines with '#####' were not executed
# Lines with numbers show execution count
```

---

### Quick Feedback Loop

```bash
# Fast test run (no Valgrind)
make test-quick
```

---

### Run Single Test File

```bash
# Compile and run just one test
clang -g -Wall -Itest/unity -Ilib \
  test/test_parsing.c src/minicron.c test/unity/unity.c \
  -o test_parsing && ./test_parsing
```

---

### Run All Quality Checks

```bash
# Tests + Valgrind + Static Analysis
make check

# Output:
# Running tests...
# ✓ All tests passed
# Running Valgrind...
# ✓ No memory leaks
# Running static analysis...
# ✓ No warnings
# ✅ All checks passed!
```

---

## CI/CD Integration

### GitHub Actions Workflow

Create `.github/workflows/test.yml`:

```yaml
name: Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest

    steps:
    - name: Checkout code
      uses: actions/checkout@v3
      with:
        submodules: recursive  # For Unity submodule

    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y valgrind clang

    - name: Build main program
      run: make

    - name: Run tests
      run: make test

    - name: Memory leak check
      run: make test-memory

    - name: Static analysis
      run: make analyze

    - name: Code coverage
      run: make test-coverage

    - name: Upload coverage to Codecov
      uses: codecov/codecov-action@v3
      with:
        files: ./minicron.c.gcov
        fail_ci_if_error: true
```

---

### Add Status Badge to README

```markdown
[![Tests](https://github.com/username/minicronparser/workflows/Tests/badge.svg)](https://github.com/username/minicronparser/actions)
[![codecov](https://codecov.io/gh/username/minicronparser/branch/main/graph/badge.svg)](https://codecov.io/gh/username/minicronparser)
```

---

## Best Practices

### 1. Test Naming Convention

```c
// Good: Describes what is being tested
void test_parse_time_string_with_valid_input(void)
void test_insert_job_to_empty_list(void)
void test_normalize_time_24_60_becomes_01_00(void)

// Bad: Vague or unclear
void test1(void)
void test_parsing(void)
void test_stuff(void)
```

---

### 2. AAA Pattern (Arrange-Act-Assert)

```c
void test_example(void) {
    // Arrange: Set up test data
    cron_jobs jobs;
    init_jobs(&jobs);
    cron_job *job = malloc(sizeof(cron_job));
    strcpy(job->minute, "30");

    // Act: Execute the function being tested
    insert_jobs(job, &jobs);

    // Assert: Verify the results
    TEST_ASSERT_NOT_NULL(jobs.first);
    TEST_ASSERT_EQUAL_STRING("30", jobs.first->minute);

    // Cleanup
    free(job);
}
```

---

### 3. One Assertion Focus Per Test

```c
// Good: Tests one specific behavior
void test_insert_updates_first_pointer(void) {
    cron_jobs jobs;
    init_jobs(&jobs);
    cron_job *job = malloc(sizeof(cron_job));

    insert_jobs(job, &jobs);

    TEST_ASSERT_EQUAL_PTR(job, jobs.first);
    free(job);
}

void test_insert_updates_last_pointer(void) {
    cron_jobs jobs;
    init_jobs(&jobs);
    cron_job *job = malloc(sizeof(cron_job));

    insert_jobs(job, &jobs);

    TEST_ASSERT_EQUAL_PTR(job, jobs.last);
    free(job);
}

// Bad: Tests too many things
void test_insert_does_everything(void) {
    // Tests first, last, next, data all at once
    // Hard to debug when it fails
}
```

---

### 4. Test Edge Cases and Boundaries

```c
// Test boundaries
test_time_00_00()  // Midnight
test_time_23_59()  // Almost midnight
test_time_24_00()  // Wrap around

// Test limits
test_max_str_length()
test_buffer_overflow()
test_empty_input()

// Test invalid input
test_null_pointer()
test_negative_numbers()
test_invalid_format()
```

---

### 5. Use setUp and tearDown

```c
static cron_jobs *global_jobs;
static parsed_jobs *global_pjobs;

void setUp(void) {
    global_jobs = malloc(sizeof(cron_jobs));
    global_pjobs = malloc(sizeof(parsed_jobs));
    init_jobs(global_jobs);
    init_parsed_jobs(global_pjobs);
}

void tearDown(void) {
    free_jobs(global_jobs);
    free_parsed(global_pjobs);
    free(global_jobs);
    free(global_pjobs);
}

void test_something(void) {
    // Can use global_jobs without setup
    // Will be cleaned automatically
}
```

---

### 6. Document Test Intent

```c
// Test: Ensures wildcards are expanded to current time values
// This is critical for the "run every hour" use case
void test_wildcard_expansion_uses_current_time(void) {
    // ...
}
```

---

### 7. Keep Tests Fast

- Use `tmpfile()` instead of real files
- Mock file I/O when possible
- Avoid `sleep()` or delays
- Run heavy tests separately

---

### 8. Make Tests Deterministic

```c
// Bad: Uses current time (non-deterministic)
time_t now = time(NULL);
parse_jobs(now, &jobs, &pJobs);

// Good: Uses fixed time (deterministic)
parse_jobs("16:10", &jobs, &pJobs);
```

---

### 9. Test Failure Messages

```c
// Without message
TEST_ASSERT_EQUAL(expected, actual);
// Output: "Expected 10 Was 20"

// With message
TEST_ASSERT_EQUAL_MESSAGE(expected, actual,
    "Hour normalization failed for input 24:60");
// Output: "Hour normalization failed for input 24:60. Expected 1 Was 2"
```

---

### 10. Don't Test Implementation, Test Behavior

```c
// Bad: Tests internal structure
void test_linked_list_uses_malloc(void) {
    // This tests HOW it works
}

// Good: Tests external behavior
void test_insert_job_appears_in_list(void) {
    // This tests WHAT it does
}
```

---

## Summary

### Quick Start Checklist

- [ ] Add Unity as submodule
- [ ] Create test directory structure
- [ ] Create test fixtures
- [ ] Update Makefile
- [ ] Refactor code for testability (extract functions)
- [ ] Write unit tests
- [ ] Write integration tests
- [ ] Run with Valgrind
- [ ] Set up CI/CD
- [ ] Achieve 90%+ coverage

### Test Coverage Goals

| Metric | Current | Target |
|--------|---------|--------|
| Line Coverage | 0% | 90%+ |
| Function Coverage | 0% | 95%+ |
| Branch Coverage | 0% | 85%+ |
| Test Count | 0 | 100+ |

### Next Steps

1. **Week 1:** Set up Unity and write initialization tests
2. **Week 2:** Refactor `parse_jobs()` and add parsing tests
3. **Week 3:** Add integration and memory tests
4. **Week 4:** Security tests and CI/CD setup

---

**Document Version:** 1.0
**Last Updated:** December 2025
**Framework Version:** Unity 2.5.x