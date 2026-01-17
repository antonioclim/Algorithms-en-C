# =============================================================================
# WEEK 14: Advanced Topics and Comprehensive Review - Makefile
# =============================================================================
#
# This Makefile provides build automation for the Week 14 laboratory exercises.
# It supports compilation, testing, memory checking and cleaning operations.
#
# Usage:
#   make          - Build all targets
#   make run      - Run the example program
#   make test     - Run automated tests
#   make valgrind - Check for memory leaks
#   make clean    - Remove build artefacts
#   make help     - Show available targets
#
# =============================================================================

# Compiler configuration
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
CFLAGS_RELEASE = -Wall -Wextra -std=c11 -O2
LDFLAGS = -lm

# Directory structure
SRC_DIR = src
DATA_DIR = data
TEST_DIR = tests
SOL_DIR = solution

# ANSI colour codes for pretty output
GREEN  = \033[0;32m
RED    = \033[0;31m
YELLOW = \033[0;33m
CYAN   = \033[0;36m
BLUE   = \033[0;34m
MAGENTA = \033[0;35m
NC     = \033[0m
BOLD   = \033[1m

# Target executables
TARGETS = example1 exercise1 exercise2

# Solution executables (instructor only)
SOLUTIONS = exercise1_sol exercise2_sol homework1_sol homework2_sol

# Phony targets (not files)
.PHONY: all clean run test valgrind help solutions demo

# =============================================================================
# Main Targets
# =============================================================================

all: $(TARGETS)
	@echo ""
	@echo "$(GREEN)$(BOLD)✓ All targets built successfully$(NC)"
	@echo ""
	@echo "$(CYAN)Available executables:$(NC)"
	@echo "  ./example1   - Complete working demonstration"
	@echo "  ./exercise1  - Benchmarking suite (student exercise)"
	@echo "  ./exercise2  - Graph analyser (student exercise)"
	@echo ""

# Build example1 - Algorithm Portfolio Manager
example1: $(SRC_DIR)/example1.c
	@echo "$(CYAN)Building example1 (Algorithm Portfolio Manager)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ example1 built$(NC)"

# Build exercise1 - Benchmarking Suite
exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(CYAN)Building exercise1 (Benchmarking Suite)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise1 built$(NC)"

# Build exercise2 - Graph Analyser
exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(CYAN)Building exercise2 (Graph Analyser)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise2 built$(NC)"

# =============================================================================
# Solution Targets (Instructor Only)
# =============================================================================

solutions: $(SOLUTIONS)
	@echo ""
	@echo "$(MAGENTA)$(BOLD)✓ All solutions built$(NC)"

exercise1_sol: $(SOL_DIR)/exercise1_sol.c
	@echo "$(MAGENTA)Building exercise1_sol...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise1_sol built$(NC)"

exercise2_sol: $(SOL_DIR)/exercise2_sol.c
	@echo "$(MAGENTA)Building exercise2_sol...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise2_sol built$(NC)"

homework1_sol: $(SOL_DIR)/homework1_sol.c
	@echo "$(MAGENTA)Building homework1_sol...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ homework1_sol built$(NC)"

homework2_sol: $(SOL_DIR)/homework2_sol.c
	@echo "$(MAGENTA)Building homework2_sol...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ homework2_sol built$(NC)"

# =============================================================================
# Run Targets
# =============================================================================

run: example1
	@echo ""
	@echo "$(YELLOW)$(BOLD)Running example1...$(NC)"
	@echo "$(YELLOW)════════════════════════════════════════════════════════════════$(NC)"
	@./example1
	@echo "$(YELLOW)════════════════════════════════════════════════════════════════$(NC)"

run-ex1: exercise1
	@echo ""
	@echo "$(YELLOW)$(BOLD)Running exercise1...$(NC)"
	@echo "$(YELLOW)════════════════════════════════════════════════════════════════$(NC)"
	@./exercise1
	@echo "$(YELLOW)════════════════════════════════════════════════════════════════$(NC)"

run-ex2: exercise2
	@echo ""
	@echo "$(YELLOW)$(BOLD)Running exercise2...$(NC)"
	@echo "$(YELLOW)════════════════════════════════════════════════════════════════$(NC)"
	@./exercise2 $(DATA_DIR)/graph_sample.txt
	@echo "$(YELLOW)════════════════════════════════════════════════════════════════$(NC)"

demo: all
	@echo ""
	@echo "$(BLUE)$(BOLD)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(BLUE)$(BOLD)              WEEK 14 DEMONSTRATION                            $(NC)"
	@echo "$(BLUE)$(BOLD)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@./example1
	@echo ""
	@echo "$(GREEN)$(BOLD)Demo completed successfully$(NC)"

# =============================================================================
# Test Targets
# =============================================================================

test: exercise1 exercise2
	@echo ""
	@echo "$(YELLOW)$(BOLD)Running automated tests...$(NC)"
	@echo ""
	@rm -f /tmp/week14_test*_out.txt /tmp/week14_test*_diff.txt
	@echo "$(CYAN)Test 1: Sorting Algorithms$(NC)"
	@echo "  Input: $(TEST_DIR)/test1_input.txt"
	@echo "  Expected: $(TEST_DIR)/test1_expected.txt"
	@./exercise1 < $(TEST_DIR)/test1_input.txt > /tmp/week14_test1_out.txt
	@if diff -u $(TEST_DIR)/test1_expected.txt /tmp/week14_test1_out.txt > /tmp/week14_test1_diff.txt; then 		echo "  $(GREEN)✓ Test 1 PASSED$(NC)"; 	else 		echo "  $(RED)✗ Test 1 FAILED$(NC)"; 		echo "  $(RED)Diff (expected vs actual):$(NC)"; 		cat /tmp/week14_test1_diff.txt; 		exit 1; 	fi
	@echo ""
	@echo "$(CYAN)Test 2: Graph Analysis$(NC)"
	@echo "  Input: $(TEST_DIR)/test2_input.txt"
	@echo "  Expected: $(TEST_DIR)/test2_expected.txt"
	@./exercise2 $(TEST_DIR)/test2_input.txt > /tmp/week14_test2_out.txt
	@if diff -u $(TEST_DIR)/test2_expected.txt /tmp/week14_test2_out.txt > /tmp/week14_test2_diff.txt; then 		echo "  $(GREEN)✓ Test 2 PASSED$(NC)"; 	else 		echo "  $(RED)✗ Test 2 FAILED$(NC)"; 		echo "  $(RED)Diff (expected vs actual):$(NC)"; 		cat /tmp/week14_test2_diff.txt; 		exit 1; 	fi
	@echo ""
	@echo "$(GREEN)$(BOLD)✓ All tests passed$(NC)"
	@rm -f /tmp/week14_test*_out.txt /tmp/week14_test*_diff.txt


test-solutions: solutions
	@echo ""
	@echo "$(MAGENTA)$(BOLD)Running regression tests for solution binaries...$(NC)"
	@rm -f /tmp/week14_sol*_out.txt /tmp/week14_sol*_diff.txt
	@./exercise1_sol < $(TEST_DIR)/test1_input.txt > /tmp/week14_sol1_out.txt
	@if diff -u $(TEST_DIR)/test1_expected.txt /tmp/week14_sol1_out.txt > /tmp/week14_sol1_diff.txt; then 		echo "$(GREEN)✓ Exercise 1 Solution: PASSED$(NC)"; 	else 		echo "$(RED)✗ Exercise 1 Solution: FAILED$(NC)"; 		cat /tmp/week14_sol1_diff.txt; 		exit 1; 	fi
	@./exercise2_sol $(TEST_DIR)/test2_input.txt > /tmp/week14_sol2_out.txt
	@if diff -u $(TEST_DIR)/test2_expected.txt /tmp/week14_sol2_out.txt > /tmp/week14_sol2_diff.txt; then 		echo "$(GREEN)✓ Exercise 2 Solution: PASSED$(NC)"; 	else 		echo "$(RED)✗ Exercise 2 Solution: FAILED$(NC)"; 		cat /tmp/week14_sol2_diff.txt; 		exit 1; 	fi
	@echo ""
	@echo "$(GREEN)$(BOLD)✓ Solution regression tests passed$(NC)"
	@rm -f /tmp/week14_sol*_out.txt /tmp/week14_sol*_diff.txt

# =============================================================================
# Memory Check Targets
# =============================================================================

valgrind: example1
	@echo ""
	@echo "$(YELLOW)$(BOLD)Running Valgrind memory check on example1...$(NC)"
	@echo ""
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--error-exitcode=1 ./example1 2>&1 | head -100
	@echo ""
	@if valgrind --leak-check=full --error-exitcode=1 ./example1 > /dev/null 2>&1; then \
		echo "$(GREEN)$(BOLD)✓ No memory leaks detected$(NC)"; \
	else \
		echo "$(RED)$(BOLD)✗ Memory issues detected$(NC)"; \
	fi

valgrind-ex1: exercise1
	@echo ""
	@echo "$(YELLOW)Running Valgrind on exercise1...$(NC)"
	@valgrind --leak-check=full ./exercise1

valgrind-ex2: exercise2
	@echo ""
	@echo "$(YELLOW)Running Valgrind on exercise2...$(NC)"
	@valgrind --leak-check=full ./exercise2 $(DATA_DIR)/graph_sample.txt

# =============================================================================
# Utility Targets
# =============================================================================

clean:
	@echo "$(YELLOW)Cleaning build artefacts...$(NC)"
	@rm -f $(TARGETS) $(SOLUTIONS)
	@rm -f *.o *.csv *.txt
	@rm -f /tmp/sol*_out.txt
	@rm -f benchmark_results.csv algorithm_stats.txt graph_analysis.txt
	@echo "$(GREEN)✓ Clean complete$(NC)"

# Display help information
help:
	@echo ""
	@echo "$(BOLD)Week 14: Advanced Topics and Comprehensive Review$(NC)"
	@echo "$(BOLD)═══════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(CYAN)Build Targets:$(NC)"
	@echo "  $(BOLD)make$(NC)            Build all student exercises"
	@echo "  $(BOLD)make example1$(NC)   Build the demonstration program"
	@echo "  $(BOLD)make exercise1$(NC)  Build benchmarking suite"
	@echo "  $(BOLD)make exercise2$(NC)  Build graph analyser"
	@echo "  $(BOLD)make solutions$(NC)  Build solution files (instructor)"
	@echo ""
	@echo "$(CYAN)Run Targets:$(NC)"
	@echo "  $(BOLD)make run$(NC)        Run example1 demonstration"
	@echo "  $(BOLD)make run-ex1$(NC)    Run exercise1"
	@echo "  $(BOLD)make run-ex2$(NC)    Run exercise2 with sample graph"
	@echo "  $(BOLD)make demo$(NC)       Run complete demonstration"
	@echo ""
	@echo "$(CYAN)Test Targets:$(NC)"
	@echo "  $(BOLD)make test$(NC)       Run automated tests"
	@echo "  $(BOLD)make valgrind$(NC)   Check for memory leaks"
	@echo ""
	@echo "$(CYAN)Utility Targets:$(NC)"
	@echo "  $(BOLD)make clean$(NC)      Remove build artefacts"
	@echo "  $(BOLD)make help$(NC)       Show this help message"
	@echo ""
	@echo "$(CYAN)Directory Structure:$(NC)"
	@echo "  src/       Source files"
	@echo "  data/      Sample data files"
	@echo "  tests/     Test input and expected output"
	@echo "  solution/  Complete solutions (instructor)"
	@echo "  teme/      Homework specifications"
	@echo "  slides/    Lecture presentations"
	@echo ""

# =============================================================================
# Debug Target
# =============================================================================

debug: CFLAGS += -DDEBUG -O0
debug: clean all
	@echo "$(MAGENTA)Debug build complete (with -DDEBUG -O0)$(NC)"

# =============================================================================
# Release Target
# =============================================================================

release: CFLAGS = $(CFLAGS_RELEASE)
release: clean all
	@echo "$(GREEN)Release build complete (with -O2)$(NC)"

# =============================================================================
# End of Makefile
# =============================================================================
