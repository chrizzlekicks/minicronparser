# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Mini Cron Parser is a C program that parses simplified cron job configurations and determines when they should execute relative to a given current time.

**Core functionality:**
- Reads cron jobs from a text file (format: `minute hour /path/to/task`)
- Supports wildcard `*` for minute/hour fields
- Determines if jobs fire "today" or "tomorrow" based on current time
- Implements smart time normalization (e.g., `24:60` → `01:00`, `23:60` → `00:00`)

## Build and Run Commands

### Build
```bash
make
```
Creates `bin/` directory with the compiled executable and copies `input.txt` into it.

### Run
```bash
cd bin && ./main input.txt 16:10
```
Arguments:
1. Path to input file containing cron jobs
2. Current time in `HH:MM` format

### Clean
```bash
make clean
```
Removes the `bin/` directory and all build artifacts.

## Code Architecture

### Two-Stage Data Pipeline

The program transforms cron job data through two distinct stages using separate linked list structures:

1. **Raw Input Stage (`cron_job` / `cron_jobs`)**
   - Defined in `lib/minicron.h:9-14`
   - Stores jobs as read from file with string fields (minute, hour, fire_task)
   - Preserves wildcard `*` characters
   - Created by `read_input()` in `src/minicron.c:40`

2. **Parsed Output Stage (`parsed_job` / `parsed_jobs`)**
   - Defined in `lib/minicron.h:19-25`
   - Stores jobs after parsing with integer time fields
   - Includes calculated "today"/"tomorrow" string
   - Created by `parse_jobs()` in `src/minicron.c:94`

**Why separate structures?** This design preserves the original input while enabling type-safe processing. The raw stage uses strings to handle wildcards, while the parsed stage uses integers for time calculations.

### Main Program Flow (`src/main.c`)

```
1. init_jobs() → initialize raw jobs list
2. read_input() → parse file into raw jobs
3. print_jobs() → display raw input
4. init_parsed_jobs() → initialize parsed jobs list
5. parse_jobs() → transform raw → parsed with time logic
6. print_parsed() → display final output
7. free_jobs() / free_parsed() → cleanup
```

### Core Parsing Logic (`src/minicron.c:94-175`)

The `parse_jobs()` function implements the critical business logic:

- **Time validation and normalization**: Lines 121-139 handle edge cases like `24:60`, converting them to valid times
- **Wildcard expansion**: Lines 156-165 replace `*` with current time values
- **Today/tomorrow determination**: Line 162-164 compare parsed hour with current hour
- **Important bounds**: `MAX_HOUR=24`, `MAX_MINUTE=60` (line 120-123 validates input)

### Memory Management

All dynamic allocations use `malloc()` and must be freed:
- Jobs are allocated in `read_input()` (line 56) and `parse_jobs()` (line 153)
- Cleanup functions `free_jobs()` and `free_parsed()` (lines 66-84) traverse lists and free each node
- **Critical**: When adding features that create new data, ensure corresponding cleanup

### Key Constants (`lib/minicron.h:1-4`)

- `MAX_BUF 1023`: Buffer size for reading file lines
- `MAX_STR 255`: String field size in structs
- `MAX_HOUR 24`: Upper bound for hour validation
- `MAX_MINUTE 60`: Upper bound for minute validation

## Development Guidelines

### When Modifying Parser Logic

The time normalization logic (lines 125-139 in `src/minicron.c`) handles special cases in this order:
1. Check for values beyond max bounds (exit if > 23 hours or > 59 minutes)
2. Handle `24:60` → `01:00`
3. Handle `23:60` → `00:00`
4. Handle minute overflow (`:60` adds to hour)
5. Handle hour overflow (`24:` wraps to `00:`)

Preserve this cascade when making changes.

### When Adding New Fields

1. Update both `cron_job` and `parsed_job` structs in `lib/minicron.h`
2. Modify `read_input()` to parse new fields from file
3. Update `parse_jobs()` to transform the field
4. Update both `print_jobs()` and `print_parsed()` for output
5. Ensure field sizes don't exceed `MAX_STR`

### Compiler Configuration

- **Compiler**: `clang` (specified in Makefile)
- **Flags**: `-g -Wall` (debug symbols + all warnings)
- Change compiler by modifying `CC` variable in Makefile