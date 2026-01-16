# =============================================================================
# WEEK 20: PARALLEL AND CONCURRENT PROGRAMMING - Makefile
# =============================================================================
# 
# Algorithms and Programming Techniques
# Academy of Economic Studies, Bucharest
#
# Build automation for parallel programming laboratory exercises
#
# =============================================================================

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O2
LDFLAGS = -pthread -lm

# Thread sanitiser flags (for debugging race conditions)
TSAN_FLAGS = -fsanitize=thread -g -O1

# Directory structure
SRC_DIR = src
DATA_DIR = data
TEST_DIR = tests
SOL_DIR = solution

# ANSI colour codes for formatted output
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
CYAN = \033[0;36m
MAGENTA = \033[0;35m
BLUE = \033[0;34m
BOLD = \033[1m
NC = \033[0m

# Executable targets
TARGETS = example1 exercise1 exercise2
SOLUTIONS = exercise1_sol exercise2_sol homework1_sol homework2_sol

# Phony targets (not files)
.PHONY: all clean run run-example run-ex1 run-ex2 test test1 test2 \
        valgrind valgrind-ex1 valgrind-ex2 tsan solutions help info \
        docker docker-build docker-run

# =============================================================================
# PRIMARY TARGETS
# =============================================================================

all: $(TARGETS)
	@echo ""
	@echo "$(GREEN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(GREEN)║$(NC)  $(BOLD)✓ All targets built successfully$(NC)                            $(GREEN)║$(NC)"
	@echo "$(GREEN)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Available executables:$(NC)"
	@echo "  ./example1   - Complete parallel programming demonstration"
	@echo "  ./exercise1  - Producer-consumer log processor"
	@echo "  ./exercise2  - Parallel Quick Sort implementation"
	@echo ""
	@echo "$(YELLOW)Run 'make help' for available commands$(NC)"

# =============================================================================
# INDIVIDUAL BUILD TARGETS
# =============================================================================

example1: $(SRC_DIR)/example1.c
	@echo "$(CYAN)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│$(NC) Building $(BOLD)example1$(NC) - Parallel Programming Demonstration           $(CYAN)│$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ example1 compiled successfully$(NC)"

exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(CYAN)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│$(NC) Building $(BOLD)exercise1$(NC) - Producer-Consumer Log Processor             $(CYAN)│$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise1 compiled successfully$(NC)"

exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(CYAN)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│$(NC) Building $(BOLD)exercise2$(NC) - Parallel Quick Sort                          $(CYAN)│$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise2 compiled successfully$(NC)"

# =============================================================================
# THREAD SANITISER BUILDS
# =============================================================================

tsan: tsan-example1 tsan-exercise1 tsan-exercise2
	@echo "$(GREEN)  ✓ All thread sanitiser builds complete$(NC)"

tsan-example1: $(SRC_DIR)/example1.c
	@echo "$(MAGENTA)Building example1 with Thread Sanitiser...$(NC)"
	@$(CC) $(TSAN_FLAGS) -o example1_tsan $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ example1_tsan compiled$(NC)"
	@echo "$(YELLOW)  Running thread sanitiser analysis...$(NC)"
	@./example1_tsan 2>&1 | head -50 || true
	@rm -f example1_tsan

tsan-exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(MAGENTA)Building exercise1 with Thread Sanitiser...$(NC)"
	@$(CC) $(TSAN_FLAGS) -o exercise1_tsan $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise1_tsan compiled$(NC)"

tsan-exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(MAGENTA)Building exercise2 with Thread Sanitiser...$(NC)"
	@$(CC) $(TSAN_FLAGS) -o exercise2_tsan $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise2_tsan compiled$(NC)"

# =============================================================================
# SOLUTION BUILDS (Instructor Only)
# =============================================================================

solutions: $(SOLUTIONS)
	@echo "$(GREEN)  ✓ All solutions built$(NC)"

exercise1_sol: $(SOL_DIR)/exercise1_sol.c
	@echo "$(MAGENTA)Building solution: exercise1_sol$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

exercise2_sol: $(SOL_DIR)/exercise2_sol.c
	@echo "$(MAGENTA)Building solution: exercise2_sol$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

homework1_sol: $(SOL_DIR)/homework1_sol.c
	@echo "$(MAGENTA)Building solution: homework1_sol$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

homework2_sol: $(SOL_DIR)/homework2_sol.c
	@echo "$(MAGENTA)Building solution: homework2_sol$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# =============================================================================
# RUN TARGETS
# =============================================================================

run: all run-example
	@echo ""

run-example: example1
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║$(NC)  $(BOLD)Running: Complete Parallel Programming Demonstration$(NC)        $(YELLOW)║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./example1

run-ex1: exercise1
	@echo ""
	@echo "$(YELLOW)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(YELLOW)│$(NC)  Running: Exercise 1 - Producer-Consumer Log Processor         $(YELLOW)│$(NC)"
	@echo "$(YELLOW)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@./exercise1 $(DATA_DIR)/access.log $(DATA_DIR)/error.log

run-ex2: exercise2
	@echo ""
	@echo "$(YELLOW)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(YELLOW)│$(NC)  Running: Exercise 2 - Parallel Quick Sort                      $(YELLOW)│$(NC)"
	@echo "$(YELLOW)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@./exercise2 1000000

# =============================================================================
# BENCHMARK TARGETS
# =============================================================================

benchmark: exercise2
	@echo ""
	@echo "$(CYAN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)║$(NC)  $(BOLD)Running Parallel Sort Benchmark$(NC)                               $(CYAN)║$(NC)"
	@echo "$(CYAN)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(YELLOW)Testing with 100,000 elements...$(NC)"
	@./exercise2 100000
	@echo ""
	@echo "$(YELLOW)Testing with 1,000,000 elements...$(NC)"
	@./exercise2 1000000
	@echo ""
	@echo "$(YELLOW)Testing with 10,000,000 elements...$(NC)"
	@./exercise2 10000000

# =============================================================================
# TEST TARGETS
# =============================================================================

test: test1 test2
	@echo ""
	@echo "$(GREEN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(GREEN)║$(NC)  $(BOLD)✓ All tests completed$(NC)                                        $(GREEN)║$(NC)"
	@echo "$(GREEN)╚═══════════════════════════════════════════════════════════════╝$(NC)"

test1: exercise1
	@echo ""
	@echo "$(YELLOW)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(YELLOW)│$(NC)  Testing Exercise 1: Producer-Consumer Log Processor           $(YELLOW)│$(NC)"
	@echo "$(YELLOW)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@./exercise1 $(TEST_DIR)/test1_input.txt > /tmp/test1_output.txt 2>&1
	@if diff -q /tmp/test1_output.txt $(TEST_DIR)/test1_expected.txt > /dev/null 2>&1; then \
		echo "$(GREEN)  ✓ Exercise 1: PASSED$(NC)"; \
	else \
		echo "$(RED)  ✗ Exercise 1: FAILED$(NC)"; \
		echo "$(YELLOW)  Expected:$(NC)"; \
		head -10 $(TEST_DIR)/test1_expected.txt; \
		echo "$(YELLOW)  Got:$(NC)"; \
		head -10 /tmp/test1_output.txt; \
	fi

test2: exercise2
	@echo ""
	@echo "$(YELLOW)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(YELLOW)│$(NC)  Testing Exercise 2: Parallel Quick Sort                        $(YELLOW)│$(NC)"
	@echo "$(YELLOW)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@./exercise2 10000 > /tmp/test2_output.txt 2>&1
	@if grep -q "SORTED: YES" /tmp/test2_output.txt; then \
		echo "$(GREEN)  ✓ Exercise 2: PASSED (array correctly sorted)$(NC)"; \
	else \
		echo "$(RED)  ✗ Exercise 2: FAILED (array not sorted)$(NC)"; \
		cat /tmp/test2_output.txt; \
	fi

# =============================================================================
# MEMORY AND THREAD CHECKING
# =============================================================================

valgrind: valgrind-ex1 valgrind-ex2
	@echo ""
	@echo "$(GREEN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(GREEN)║$(NC)  $(BOLD)✓ Memory check complete$(NC)                                      $(GREEN)║$(NC)"
	@echo "$(GREEN)╚═══════════════════════════════════════════════════════════════╝$(NC)"

valgrind-ex1: exercise1
	@echo ""
	@echo "$(YELLOW)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(YELLOW)│$(NC)  Valgrind: Exercise 1                                           $(YELLOW)│$(NC)"
	@echo "$(YELLOW)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all \
		--error-exitcode=1 ./exercise1 $(TEST_DIR)/test1_input.txt 2>&1 | \
		grep -E "(ERROR SUMMARY|definitely lost|indirectly lost|possibly lost|still reachable)" || true

valgrind-ex2: exercise2
	@echo ""
	@echo "$(YELLOW)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(YELLOW)│$(NC)  Valgrind: Exercise 2                                           $(YELLOW)│$(NC)"
	@echo "$(YELLOW)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all \
		--error-exitcode=1 ./exercise2 1000 2>&1 | \
		grep -E "(ERROR SUMMARY|definitely lost|indirectly lost|possibly lost|still reachable)" || true

helgrind: exercise1 exercise2
	@echo ""
	@echo "$(CYAN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)║$(NC)  $(BOLD)Running Helgrind Race Condition Detector$(NC)                     $(CYAN)║$(NC)"
	@echo "$(CYAN)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(YELLOW)Checking exercise1...$(NC)"
	@valgrind --tool=helgrind ./exercise1 $(TEST_DIR)/test1_input.txt 2>&1 | \
		grep -E "(ERROR SUMMARY|Possible data race)" || echo "$(GREEN)  ✓ No races detected$(NC)"
	@echo ""
	@echo "$(YELLOW)Checking exercise2...$(NC)"
	@valgrind --tool=helgrind ./exercise2 1000 2>&1 | \
		grep -E "(ERROR SUMMARY|Possible data race)" || echo "$(GREEN)  ✓ No races detected$(NC)"

# =============================================================================
# DOCKER TARGETS
# =============================================================================

docker: docker-build docker-run

docker-build:
	@echo "$(CYAN)Building Docker image...$(NC)"
	@docker build -t week20-parallel .
	@echo "$(GREEN)  ✓ Docker image built$(NC)"

docker-run:
	@echo "$(CYAN)Running in Docker container...$(NC)"
	@docker run --rm week20-parallel

docker-shell:
	@echo "$(CYAN)Starting interactive Docker shell...$(NC)"
	@docker run --rm -it week20-parallel /bin/bash

docker-compose-up:
	@echo "$(CYAN)Starting Docker Compose services...$(NC)"
	@docker-compose up --build

docker-compose-down:
	@echo "$(CYAN)Stopping Docker Compose services...$(NC)"
	@docker-compose down

# =============================================================================
# CLEANUP
# =============================================================================

clean:
	@echo "$(YELLOW)Cleaning build artefacts...$(NC)"
	@rm -f $(TARGETS)
	@rm -f $(SOLUTIONS)
	@rm -f *_tsan
	@rm -f /tmp/test*_output.txt
	@rm -f *.o
	@rm -f core core.*
	@echo "$(GREEN)  ✓ Clean complete$(NC)"

distclean: clean
	@echo "$(YELLOW)Removing generated data files...$(NC)"
	@rm -f $(DATA_DIR)/*.generated
	@echo "$(GREEN)  ✓ Distribution clean complete$(NC)"

# =============================================================================
# INFORMATION AND HELP
# =============================================================================

info:
	@echo ""
	@echo "$(CYAN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)║$(NC)  $(BOLD)WEEK 20: PARALLEL AND CONCURRENT PROGRAMMING$(NC)                 $(CYAN)║$(NC)"
	@echo "$(CYAN)║$(NC)  Algorithms and Programming Techniques - ASE CSIE             $(CYAN)║$(NC)"
	@echo "$(CYAN)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(YELLOW)Topics covered:$(NC)"
	@echo "  • POSIX Threads (pthreads) programming"
	@echo "  • Mutexes and synchronisation primitives"
	@echo "  • Semaphores and condition variables"
	@echo "  • Producer-consumer pattern"
	@echo "  • Thread pools and task-based parallelism"
	@echo "  • C11 atomics and lock-free programming"
	@echo "  • Deadlock prevention and detection"
	@echo "  • Amdahl's Law and parallel speedup analysis"
	@echo ""

help:
	@echo ""
	@echo "$(CYAN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)║$(NC)  $(BOLD)WEEK 20: PARALLEL PROGRAMMING - MAKEFILE HELP$(NC)                $(CYAN)║$(NC)"
	@echo "$(CYAN)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(BOLD)Build Targets:$(NC)"
	@echo "  $(YELLOW)make$(NC)              - Compile all source files"
	@echo "  $(YELLOW)make example1$(NC)     - Build main demonstration"
	@echo "  $(YELLOW)make exercise1$(NC)    - Build producer-consumer exercise"
	@echo "  $(YELLOW)make exercise2$(NC)    - Build parallel sort exercise"
	@echo "  $(YELLOW)make solutions$(NC)    - Build solution files (instructor)"
	@echo ""
	@echo "$(BOLD)Run Targets:$(NC)"
	@echo "  $(YELLOW)make run$(NC)          - Run all demonstrations"
	@echo "  $(YELLOW)make run-example$(NC)  - Run main example only"
	@echo "  $(YELLOW)make run-ex1$(NC)      - Run exercise 1"
	@echo "  $(YELLOW)make run-ex2$(NC)      - Run exercise 2"
	@echo "  $(YELLOW)make benchmark$(NC)    - Run performance benchmarks"
	@echo ""
	@echo "$(BOLD)Test Targets:$(NC)"
	@echo "  $(YELLOW)make test$(NC)         - Run all automated tests"
	@echo "  $(YELLOW)make test1$(NC)        - Test exercise 1 only"
	@echo "  $(YELLOW)make test2$(NC)        - Test exercise 2 only"
	@echo ""
	@echo "$(BOLD)Debug Targets:$(NC)"
	@echo "  $(YELLOW)make valgrind$(NC)     - Check for memory leaks"
	@echo "  $(YELLOW)make helgrind$(NC)     - Check for race conditions"
	@echo "  $(YELLOW)make tsan$(NC)         - Build with Thread Sanitiser"
	@echo ""
	@echo "$(BOLD)Docker Targets:$(NC)"
	@echo "  $(YELLOW)make docker$(NC)       - Build and run in Docker"
	@echo "  $(YELLOW)make docker-shell$(NC) - Interactive Docker shell"
	@echo ""
	@echo "$(BOLD)Utility Targets:$(NC)"
	@echo "  $(YELLOW)make clean$(NC)        - Remove compiled files"
	@echo "  $(YELLOW)make info$(NC)         - Display week information"
	@echo "  $(YELLOW)make help$(NC)         - Show this help message"
	@echo ""

.PRECIOUS: $(TARGETS)
