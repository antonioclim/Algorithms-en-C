# =============================================================================
# WEEK 07: BINARY TREES - Makefile
# =============================================================================
# Algorithms and Programming Techniques
# ASE-CSIE, Bucharest
#
# Build automation for binary tree laboratory exercises
# Supports: compilation, testing, memory checking, cleaning
# =============================================================================

# Compiler configuration
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS = -lm

# Directories
SRC_DIR = src
DATA_DIR = data
TEST_DIR = tests
SOL_DIR = solution

# =============================================================================
# ANSI Colour Codes
# =============================================================================
GREEN  = \033[0;32m
RED    = \033[0;31m
YELLOW = \033[0;33m
CYAN   = \033[0;36m
BLUE   = \033[0;34m
MAGENTA = \033[0;35m
BOLD   = \033[1m
NC     = \033[0m

# =============================================================================
# Target Executables
# =============================================================================
TARGETS = example1 exercise1 exercise2
SOLUTIONS = exercise1_sol exercise2_sol homework1_sol homework2_sol

# =============================================================================
# Phony Targets
# =============================================================================
.PHONY: all clean run test valgrind help solutions demo \
        run-example run-exercise1 run-exercise2 \
        test1 test2 check

# =============================================================================
# Default Target
# =============================================================================
all: $(TARGETS)
	@echo ""
	@echo "$(GREEN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(GREEN)║         ✓ All targets built successfully                      ║$(NC)"
	@echo "$(GREEN)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Available executables:$(NC)"
	@echo "  ./example1   - Complete binary tree demonstration"
	@echo "  ./exercise1  - Tree construction exercise"
	@echo "  ./exercise2  - Expression tree exercise"
	@echo ""
	@echo "$(YELLOW)Run 'make help' for available commands$(NC)"

# =============================================================================
# Individual Build Targets
# =============================================================================
example1: $(SRC_DIR)/example1.c
	@echo "$(CYAN)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│ Building example1 - Complete Binary Tree Demonstration          │$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ example1 compiled successfully$(NC)"

exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(CYAN)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│ Building exercise1 - Tree Construction Exercise                 │$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise1 compiled successfully$(NC)"

exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(CYAN)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│ Building exercise2 - Expression Tree Exercise                   │$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise2 compiled successfully$(NC)"

# =============================================================================
# Solution Builds (Instructor Only)
# =============================================================================
solutions: $(SOLUTIONS)
	@echo "$(GREEN)  ✓ All solutions built successfully$(NC)"

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
# Run Targets
# =============================================================================
run: all
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║                  Running All Examples                          ║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./example1

run-example: example1
	@echo "$(YELLOW)Running example1...$(NC)"
	@./example1

run-exercise1: exercise1
	@echo "$(YELLOW)Running exercise1...$(NC)"
	@./exercise1

run-exercise2: exercise2
	@echo "$(YELLOW)Running exercise2...$(NC)"
	@./exercise2

demo: example1
	@echo ""
	@echo "$(MAGENTA)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(MAGENTA)║          Week 07: Binary Trees - Interactive Demo             ║$(NC)"
	@echo "$(MAGENTA)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./example1

# =============================================================================
# Test Targets
# =============================================================================
test: exercise1 exercise2 test1 test2
	@echo ""
	@echo "$(GREEN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(GREEN)║              All tests completed                               ║$(NC)"
	@echo "$(GREEN)╚═══════════════════════════════════════════════════════════════╝$(NC)"

test1: exercise1
	@echo ""
	@echo "$(YELLOW)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(YELLOW)│ Testing Exercise 1: Tree Construction                           │$(NC)"
	@echo "$(YELLOW)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@if ./exercise1 < $(TEST_DIR)/test1_input.txt > /tmp/test1_output.txt 2>&1; then \
		if diff -q /tmp/test1_output.txt $(TEST_DIR)/test1_expected.txt > /dev/null 2>&1; then \
			echo "$(GREEN)  ✓ Test 1 PASSED$(NC)"; \
		else \
			echo "$(RED)  ✗ Test 1 FAILED - Output mismatch$(NC)"; \
			echo "$(YELLOW)  Expected:$(NC)"; \
			head -5 $(TEST_DIR)/test1_expected.txt; \
			echo "$(YELLOW)  Got:$(NC)"; \
			head -5 /tmp/test1_output.txt; \
		fi \
	else \
		echo "$(RED)  ✗ Test 1 FAILED - Runtime error$(NC)"; \
	fi

test2: exercise2
	@echo ""
	@echo "$(YELLOW)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(YELLOW)│ Testing Exercise 2: Expression Trees                            │$(NC)"
	@echo "$(YELLOW)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@if ./exercise2 < $(TEST_DIR)/test2_input.txt > /tmp/test2_output.txt 2>&1; then \
		if diff -q /tmp/test2_output.txt $(TEST_DIR)/test2_expected.txt > /dev/null 2>&1; then \
			echo "$(GREEN)  ✓ Test 2 PASSED$(NC)"; \
		else \
			echo "$(RED)  ✗ Test 2 FAILED - Output mismatch$(NC)"; \
			echo "$(YELLOW)  Expected:$(NC)"; \
			head -5 $(TEST_DIR)/test2_expected.txt; \
			echo "$(YELLOW)  Got:$(NC)"; \
			head -5 /tmp/test2_output.txt; \
		fi \
	else \
		echo "$(RED)  ✗ Test 2 FAILED - Runtime error$(NC)"; \
	fi

# =============================================================================
# Memory Check with Valgrind
# =============================================================================
valgrind: all
	@echo ""
	@echo "$(MAGENTA)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(MAGENTA)║           Running Valgrind Memory Check                        ║$(NC)"
	@echo "$(MAGENTA)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(YELLOW)Checking example1...$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--error-exitcode=1 ./example1 2>&1 | tail -20
	@echo ""
	@echo "$(GREEN)  ✓ Memory check complete$(NC)"

valgrind-exercise1: exercise1
	@echo "$(YELLOW)Memory checking exercise1...$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all ./exercise1 < $(TEST_DIR)/test1_input.txt

valgrind-exercise2: exercise2
	@echo "$(YELLOW)Memory checking exercise2...$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all ./exercise2 < $(TEST_DIR)/test2_input.txt

# =============================================================================
# Static Analysis
# =============================================================================
check: $(SRC_DIR)/*.c
	@echo "$(YELLOW)Running static analysis...$(NC)"
	@for file in $(SRC_DIR)/*.c; do \
		echo "  Checking $$file..."; \
		$(CC) $(CFLAGS) -fsyntax-only $$file; \
	done
	@echo "$(GREEN)  ✓ Static analysis complete$(NC)"

# =============================================================================
# Clean Target
# =============================================================================
clean:
	@echo "$(YELLOW)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(YELLOW)│ Cleaning build artefacts...                                     │$(NC)"
	@echo "$(YELLOW)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@rm -f $(TARGETS) $(SOLUTIONS)
	@rm -f *.o
	@rm -f /tmp/test*_output.txt
	@rm -f core vgcore.*
	@echo "$(GREEN)  ✓ Clean complete$(NC)"

# =============================================================================
# Help Target
# =============================================================================
help:
	@echo ""
	@echo "$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BOLD)║     WEEK 07: BINARY TREES - Build System Help                 ║$(NC)"
	@echo "$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Build Targets:$(NC)"
	@echo "  $(GREEN)make$(NC)              - Build all student targets"
	@echo "  $(GREEN)make example1$(NC)     - Build main demonstration"
	@echo "  $(GREEN)make exercise1$(NC)    - Build exercise 1"
	@echo "  $(GREEN)make exercise2$(NC)    - Build exercise 2"
	@echo "  $(GREEN)make solutions$(NC)    - Build all solutions (instructor)"
	@echo ""
	@echo "$(CYAN)Run Targets:$(NC)"
	@echo "  $(GREEN)make run$(NC)          - Run all examples"
	@echo "  $(GREEN)make demo$(NC)         - Run interactive demonstration"
	@echo "  $(GREEN)make run-exercise1$(NC) - Run exercise 1"
	@echo "  $(GREEN)make run-exercise2$(NC) - Run exercise 2"
	@echo ""
	@echo "$(CYAN)Test Targets:$(NC)"
	@echo "  $(GREEN)make test$(NC)         - Run all automated tests"
	@echo "  $(GREEN)make test1$(NC)        - Run exercise 1 tests"
	@echo "  $(GREEN)make test2$(NC)        - Run exercise 2 tests"
	@echo ""
	@echo "$(CYAN)Analysis Targets:$(NC)"
	@echo "  $(GREEN)make valgrind$(NC)     - Check for memory leaks"
	@echo "  $(GREEN)make check$(NC)        - Run static analysis"
	@echo ""
	@echo "$(CYAN)Maintenance:$(NC)"
	@echo "  $(GREEN)make clean$(NC)        - Remove build artefacts"
	@echo "  $(GREEN)make help$(NC)         - Show this help message"
	@echo ""
	@echo "$(YELLOW)Compiler: $(CC) $(CFLAGS)$(NC)"
	@echo ""

# =============================================================================
# Dependency Information
# =============================================================================
.SUFFIXES: .c .o

# Print variables for debugging
debug-vars:
	@echo "CC = $(CC)"
	@echo "CFLAGS = $(CFLAGS)"
	@echo "TARGETS = $(TARGETS)"
	@echo "SRC_DIR = $(SRC_DIR)"
