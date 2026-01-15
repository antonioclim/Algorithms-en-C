# =============================================================================
# WEEK 05: STACKS - Makefile
# Algorithms and Programming Techniques
# =============================================================================
#
# Usage:
#   make          - Build all targets
#   make run      - Run the complete example
#   make test     - Run automated tests
#   make valgrind - Check for memory leaks
#   make clean    - Remove build artefacts
#   make help     - Display this help message
#
# =============================================================================

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS =

# Directories
SRC_DIR = src
DATA_DIR = data
TEST_DIR = tests
SOL_DIR = solution

# Colour definitions (ANSI escape codes)
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
CYAN = \033[0;36m
MAGENTA = \033[0;35m
BLUE = \033[0;34m
BOLD = \033[1m
NC = \033[0m

# Unicode symbols
CHECK = ✓
CROSS = ✗
ARROW = →
GEAR = ⚙

# Target executables
TARGETS = example1 exercise1 exercise2
SOLUTION_TARGETS = exercise1_sol exercise2_sol homework1_sol homework2_sol

# =============================================================================
# PHONY TARGETS
# =============================================================================

.PHONY: all clean run test valgrind help solutions debug

# =============================================================================
# DEFAULT TARGET
# =============================================================================

all: header $(TARGETS)
	@echo ""
	@echo "$(GREEN)$(CHECK) All targets built successfully$(NC)"
	@echo "$(CYAN)$(ARROW) Run 'make run' to execute the example$(NC)"
	@echo "$(CYAN)$(ARROW) Run 'make help' for more options$(NC)"
	@echo ""

header:
	@echo ""
	@echo "$(BOLD)$(BLUE)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BOLD)$(BLUE)║     WEEK 05: STACKS - Build System                            ║$(NC)"
	@echo "$(BOLD)$(BLUE)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""

# =============================================================================
# MAIN EXECUTABLES
# =============================================================================

example1: $(SRC_DIR)/example1.c
	@echo "$(CYAN)$(GEAR) Building example1...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) example1 built$(NC)"

exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(CYAN)$(GEAR) Building exercise1...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) exercise1 built$(NC)"

exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(CYAN)$(GEAR) Building exercise2...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) exercise2 built$(NC)"

# =============================================================================
# SOLUTION EXECUTABLES (Instructor Only)
# =============================================================================

solutions: header $(SOLUTION_TARGETS)
	@echo ""
	@echo "$(GREEN)$(CHECK) All solution targets built$(NC)"
	@echo ""

exercise1_sol: $(SOL_DIR)/exercise1_sol.c
	@echo "$(MAGENTA)$(GEAR) Building exercise1_sol...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) exercise1_sol built$(NC)"

exercise2_sol: $(SOL_DIR)/exercise2_sol.c
	@echo "$(MAGENTA)$(GEAR) Building exercise2_sol...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) exercise2_sol built$(NC)"

homework1_sol: $(SOL_DIR)/homework1_sol.c
	@echo "$(MAGENTA)$(GEAR) Building homework1_sol...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) homework1_sol built$(NC)"

homework2_sol: $(SOL_DIR)/homework2_sol.c
	@echo "$(MAGENTA)$(GEAR) Building homework2_sol...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) homework2_sol built$(NC)"

# =============================================================================
# RUN TARGETS
# =============================================================================

run: example1
	@echo ""
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)  Running Complete Stack Example$(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@./example1
	@echo ""

run-exercise1: exercise1
	@echo ""
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)  Running Exercise 1: Array-Based Stack$(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@./exercise1
	@echo ""

run-exercise2: exercise2
	@echo ""
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)  Running Exercise 2: Bracket Validator$(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@./exercise2 < $(DATA_DIR)/expressions.txt
	@echo ""

# =============================================================================
# TEST TARGETS
# =============================================================================

test: exercise1 exercise2
	@echo ""
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)  Running Automated Tests$(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(CYAN)Testing Exercise 1...$(NC)"
	@./exercise1 > /tmp/ex1_output.txt 2>&1 || true
	@if diff -q /tmp/ex1_output.txt $(TEST_DIR)/test1_expected.txt > /dev/null 2>&1; then \
		echo "$(GREEN)  $(CHECK) Exercise 1: PASSED$(NC)"; \
	else \
		echo "$(RED)  $(CROSS) Exercise 1: FAILED$(NC)"; \
		echo "$(YELLOW)    Expected output in $(TEST_DIR)/test1_expected.txt$(NC)"; \
	fi
	@echo ""
	@echo "$(CYAN)Testing Exercise 2...$(NC)"
	@./exercise2 < $(TEST_DIR)/test2_input.txt > /tmp/ex2_output.txt 2>&1 || true
	@if diff -q /tmp/ex2_output.txt $(TEST_DIR)/test2_expected.txt > /dev/null 2>&1; then \
		echo "$(GREEN)  $(CHECK) Exercise 2: PASSED$(NC)"; \
	else \
		echo "$(RED)  $(CROSS) Exercise 2: FAILED$(NC)"; \
		echo "$(YELLOW)    Expected output in $(TEST_DIR)/test2_expected.txt$(NC)"; \
	fi
	@echo ""
	@rm -f /tmp/ex1_output.txt /tmp/ex2_output.txt

test-verbose: exercise1 exercise2
	@echo ""
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)  Running Verbose Tests (showing diff)$(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(CYAN)Testing Exercise 1...$(NC)"
	@./exercise1 > /tmp/ex1_output.txt 2>&1 || true
	@diff -u $(TEST_DIR)/test1_expected.txt /tmp/ex1_output.txt || true
	@echo ""
	@echo "$(CYAN)Testing Exercise 2...$(NC)"
	@./exercise2 < $(TEST_DIR)/test2_input.txt > /tmp/ex2_output.txt 2>&1 || true
	@diff -u $(TEST_DIR)/test2_expected.txt /tmp/ex2_output.txt || true
	@echo ""
	@rm -f /tmp/ex1_output.txt /tmp/ex2_output.txt

# =============================================================================
# MEMORY CHECK TARGETS
# =============================================================================

valgrind: example1 exercise1 exercise2
	@echo ""
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)  Running Valgrind Memory Check$(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(CYAN)Checking example1...$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--error-exitcode=1 ./example1 2>&1 | tail -20 || true
	@echo ""
	@echo "$(CYAN)Checking exercise1...$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--error-exitcode=1 ./exercise1 2>&1 | tail -20 || true
	@echo ""
	@echo "$(CYAN)Checking exercise2...$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--error-exitcode=1 ./exercise2 < $(DATA_DIR)/expressions.txt 2>&1 | tail -20 || true
	@echo ""

valgrind-full: example1
	@echo ""
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)  Running Full Valgrind Analysis$(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--verbose ./example1

# =============================================================================
# DEBUG TARGET
# =============================================================================

debug: CFLAGS += -DDEBUG -fsanitize=address -fsanitize=undefined
debug: clean all
	@echo "$(MAGENTA)$(CHECK) Debug build complete (with sanitizers)$(NC)"

# =============================================================================
# CLEAN TARGET
# =============================================================================

clean:
	@echo ""
	@echo "$(YELLOW)Cleaning build artefacts...$(NC)"
	@rm -f $(TARGETS) $(SOLUTION_TARGETS)
	@rm -f *.o
	@rm -f /tmp/ex*_output.txt
	@echo "$(GREEN)$(CHECK) Clean complete$(NC)"
	@echo ""

# =============================================================================
# HELP TARGET
# =============================================================================

help:
	@echo ""
	@echo "$(BOLD)$(BLUE)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BOLD)$(BLUE)║     WEEK 05: STACKS - Available Make Targets                  ║$(NC)"
	@echo "$(BOLD)$(BLUE)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(BOLD)Build Targets:$(NC)"
	@echo "  $(CYAN)make$(NC)              - Build all student targets"
	@echo "  $(CYAN)make example1$(NC)     - Build the complete example"
	@echo "  $(CYAN)make exercise1$(NC)    - Build exercise 1 (array stack)"
	@echo "  $(CYAN)make exercise2$(NC)    - Build exercise 2 (bracket validator)"
	@echo "  $(CYAN)make solutions$(NC)    - Build solution files (instructor)"
	@echo "  $(CYAN)make debug$(NC)        - Build with debug flags and sanitizers"
	@echo ""
	@echo "$(BOLD)Run Targets:$(NC)"
	@echo "  $(CYAN)make run$(NC)          - Run the complete example"
	@echo "  $(CYAN)make run-exercise1$(NC) - Run exercise 1"
	@echo "  $(CYAN)make run-exercise2$(NC) - Run exercise 2 with sample data"
	@echo ""
	@echo "$(BOLD)Test Targets:$(NC)"
	@echo "  $(CYAN)make test$(NC)         - Run automated tests"
	@echo "  $(CYAN)make test-verbose$(NC) - Run tests with diff output"
	@echo ""
	@echo "$(BOLD)Memory Check Targets:$(NC)"
	@echo "  $(CYAN)make valgrind$(NC)     - Run Valgrind on all targets"
	@echo "  $(CYAN)make valgrind-full$(NC) - Run detailed Valgrind analysis"
	@echo ""
	@echo "$(BOLD)Utility Targets:$(NC)"
	@echo "  $(CYAN)make clean$(NC)        - Remove all build artefacts"
	@echo "  $(CYAN)make help$(NC)         - Display this help message"
	@echo ""
	@echo "$(BOLD)Directory Structure:$(NC)"
	@echo "  $(SRC_DIR)/          - Source files"
	@echo "  $(DATA_DIR)/         - Sample data files"
	@echo "  $(TEST_DIR)/         - Test input/expected output"
	@echo "  $(SOL_DIR)/       - Solution files"
	@echo ""

# =============================================================================
# END OF MAKEFILE
# =============================================================================
