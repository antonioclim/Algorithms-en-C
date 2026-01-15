# =============================================================================
# WEEK 10: HEAPS AND PRIORITY QUEUES - Makefile
# =============================================================================
# 
# Algorithms and Programming Techniques
# Academy of Economic Studies, Bucharest
#
# Targets:
#   all       - Build all executables
#   example1  - Build complete heap demonstration
#   exercise1 - Build priority queue exercise
#   exercise2 - Build heapsort exercise
#   run       - Run all demonstrations
#   test      - Execute automated tests
#   valgrind  - Memory leak detection
#   clean     - Remove build artefacts
#   help      - Display this help message
#
# =============================================================================

# Compiler configuration
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -g -O2
LDFLAGS = -lm

# Directories
SRC_DIR = src
DATA_DIR = data
TEST_DIR = tests
SOL_DIR = solution

# ANSI colour codes for terminal output
GREEN  = \033[0;32m
RED    = \033[0;31m
YELLOW = \033[0;33m
CYAN   = \033[0;36m
MAGENTA= \033[0;35m
BOLD   = \033[1m
NC     = \033[0m

# Target executables
TARGETS = example1 exercise1 exercise2
SOLUTIONS = exercise1_sol exercise2_sol homework1_sol homework2_sol

# Phony targets (not associated with files)
.PHONY: all clean run test valgrind help solutions check

# =============================================================================
# PRIMARY BUILD TARGETS
# =============================================================================

all: $(TARGETS)
	@echo ""
	@echo "$(GREEN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(GREEN)║  ✓ All targets built successfully                             ║$(NC)"
	@echo "$(GREEN)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""

example1: $(SRC_DIR)/example1.c
	@echo "$(CYAN)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│  Building example1 - Complete Heap Demonstration               │$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ example1 compiled successfully$(NC)"
	@echo ""

exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(CYAN)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│  Building exercise1 - Priority Queue Implementation            │$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise1 compiled successfully$(NC)"
	@echo ""

exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(CYAN)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│  Building exercise2 - Heapsort Implementation                  │$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise2 compiled successfully$(NC)"
	@echo ""

# =============================================================================
# SOLUTION BUILD TARGETS (Instructor use only)
# =============================================================================

solutions: $(SOLUTIONS)
	@echo ""
	@echo "$(MAGENTA)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(MAGENTA)║  ✓ All solutions built successfully                           ║$(NC)"
	@echo "$(MAGENTA)╚═══════════════════════════════════════════════════════════════╝$(NC)"

exercise1_sol: $(SOL_DIR)/exercise1_sol.c
	@echo "$(MAGENTA)  Building exercise1_sol...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise1_sol compiled$(NC)"

exercise2_sol: $(SOL_DIR)/exercise2_sol.c
	@echo "$(MAGENTA)  Building exercise2_sol...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise2_sol compiled$(NC)"

homework1_sol: $(SOL_DIR)/homework1_sol.c
	@echo "$(MAGENTA)  Building homework1_sol...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ homework1_sol compiled$(NC)"

homework2_sol: $(SOL_DIR)/homework2_sol.c
	@echo "$(MAGENTA)  Building homework2_sol...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ homework2_sol compiled$(NC)"

# =============================================================================
# EXECUTION TARGETS
# =============================================================================

run: example1
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║  Running example1 - Complete Heap Demonstration               ║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./example1
	@echo ""

run-all: $(TARGETS)
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║  Running all examples and exercises                           ║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(BOLD)>>> example1$(NC)"
	@./example1 || true
	@echo ""
	@echo "$(BOLD)>>> exercise1$(NC)"
	@./exercise1 < $(TEST_DIR)/test1_input.txt || true
	@echo ""
	@echo "$(BOLD)>>> exercise2$(NC)"
	@./exercise2 < $(TEST_DIR)/test2_input.txt || true
	@echo ""

# =============================================================================
# TESTING TARGETS
# =============================================================================

test: $(TARGETS)
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║  Running Automated Tests                                      ║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Test 1: Priority Queue Implementation$(NC)"
	@if ./exercise1 < $(TEST_DIR)/test1_input.txt > /tmp/test1_output.txt 2>&1; then \
		if diff -q /tmp/test1_output.txt $(TEST_DIR)/test1_expected.txt > /dev/null 2>&1; then \
			echo "$(GREEN)  ✓ Test 1 PASSED$(NC)"; \
		else \
			echo "$(RED)  ✗ Test 1 FAILED - Output mismatch$(NC)"; \
			echo "$(YELLOW)    Expected:$(NC)"; \
			head -5 $(TEST_DIR)/test1_expected.txt | sed 's/^/    /'; \
			echo "$(YELLOW)    Actual:$(NC)"; \
			head -5 /tmp/test1_output.txt | sed 's/^/    /'; \
		fi; \
	else \
		echo "$(RED)  ✗ Test 1 FAILED - Execution error$(NC)"; \
	fi
	@echo ""
	@echo "$(CYAN)Test 2: Heapsort Implementation$(NC)"
	@if ./exercise2 < $(TEST_DIR)/test2_input.txt > /tmp/test2_output.txt 2>&1; then \
		if diff -q /tmp/test2_output.txt $(TEST_DIR)/test2_expected.txt > /dev/null 2>&1; then \
			echo "$(GREEN)  ✓ Test 2 PASSED$(NC)"; \
		else \
			echo "$(RED)  ✗ Test 2 FAILED - Output mismatch$(NC)"; \
			echo "$(YELLOW)    Expected:$(NC)"; \
			head -5 $(TEST_DIR)/test2_expected.txt | sed 's/^/    /'; \
			echo "$(YELLOW)    Actual:$(NC)"; \
			head -5 /tmp/test2_output.txt | sed 's/^/    /'; \
		fi; \
	else \
		echo "$(RED)  ✗ Test 2 FAILED - Execution error$(NC)"; \
	fi
	@echo ""

# =============================================================================
# MEMORY CHECKING
# =============================================================================

valgrind: example1
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║  Running Valgrind Memory Check                                ║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--error-exitcode=1 ./example1 2>&1 | head -50
	@echo ""
	@echo "$(GREEN)  ✓ Memory check complete$(NC)"

valgrind-all: $(TARGETS)
	@echo ""
	@echo "$(YELLOW)Running Valgrind on all executables...$(NC)"
	@for target in $(TARGETS); do \
		echo "$(CYAN)Checking $$target...$(NC)"; \
		valgrind --leak-check=full --error-exitcode=1 ./$$target < /dev/null 2>&1 | \
			grep -E "(definitely|indirectly|possibly) lost" || \
			echo "$(GREEN)  ✓ No leaks detected$(NC)"; \
	done

# =============================================================================
# CODE QUALITY
# =============================================================================

check: $(SRC_DIR)/*.c
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║  Code Quality Check                                           ║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Checking for compiler warnings...$(NC)"
	@for src in $(SRC_DIR)/*.c; do \
		echo "  Checking $$src..."; \
		$(CC) $(CFLAGS) -fsyntax-only $$src 2>&1 || true; \
	done
	@echo ""
	@echo "$(CYAN)Line counts:$(NC)"
	@wc -l $(SRC_DIR)/*.c | tail -1
	@echo ""

# =============================================================================
# CLEANUP
# =============================================================================

clean:
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║  Cleaning build artefacts                                     ║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@rm -f $(TARGETS) $(SOLUTIONS)
	@rm -f *.o *.out
	@rm -f /tmp/test*_output.txt
	@echo "$(GREEN)  ✓ Clean complete$(NC)"
	@echo ""

# =============================================================================
# HELP
# =============================================================================

help:
	@echo ""
	@echo "$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BOLD)║  WEEK 10: HEAPS AND PRIORITY QUEUES                           ║$(NC)"
	@echo "$(BOLD)║  Makefile Help                                                ║$(NC)"
	@echo "$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Build targets:$(NC)"
	@echo "  $(GREEN)make$(NC)            Build all student executables"
	@echo "  $(GREEN)make example1$(NC)   Build complete heap demonstration"
	@echo "  $(GREEN)make exercise1$(NC)  Build priority queue exercise"
	@echo "  $(GREEN)make exercise2$(NC)  Build heapsort exercise"
	@echo "  $(GREEN)make solutions$(NC)  Build instructor solutions"
	@echo ""
	@echo "$(CYAN)Execution targets:$(NC)"
	@echo "  $(GREEN)make run$(NC)        Run example1 demonstration"
	@echo "  $(GREEN)make run-all$(NC)    Run all executables"
	@echo "  $(GREEN)make test$(NC)       Run automated tests"
	@echo ""
	@echo "$(CYAN)Quality targets:$(NC)"
	@echo "  $(GREEN)make valgrind$(NC)   Check for memory leaks"
	@echo "  $(GREEN)make check$(NC)      Run static analysis"
	@echo ""
	@echo "$(CYAN)Utility targets:$(NC)"
	@echo "  $(GREEN)make clean$(NC)      Remove all build artefacts"
	@echo "  $(GREEN)make help$(NC)       Display this help message"
	@echo ""
	@echo "$(YELLOW)Compiler: $(CC) $(CFLAGS)$(NC)"
	@echo ""
