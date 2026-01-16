# =============================================================================
# WEEK 04: LINKED LISTS - Makefile
# =============================================================================
#
# Usage:
#   make          - Build all targets
#   make run      - Run the example demonstration
#   make test     - Run automated tests
#   make valgrind - Check for memory leaks
#   make clean    - Remove build artifacts
#   make help     - Display help information
#
# =============================================================================

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS = -lm

# Directories
SRC_DIR = src
SOL_DIR = solution
DATA_DIR = data
TEST_DIR = tests

# Colours for output
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
CYAN = \033[0;36m
MAGENTA = \033[0;35m
NC = \033[0m

# Main targets (student exercises)
TARGETS = example1 exercise1 exercise2

# Solution targets (instructor only)
SOLUTIONS = exercise1_sol exercise2_sol homework1_sol homework2_sol

# =============================================================================
# PHONY TARGETS
# =============================================================================

.PHONY: all clean run test test-solutions valgrind solutions help check

# =============================================================================
# DEFAULT TARGET
# =============================================================================

all: $(TARGETS)
	@echo "$(GREEN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(GREEN)✓ All targets built successfully$(NC)"
	@echo "$(GREEN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "Available executables:"
	@echo "  $(CYAN)./example1$(NC)   - Complete demonstration of linked list operations"
	@echo "  $(CYAN)./exercise1$(NC)  - Student records exercise (complete)"
	@echo "  $(CYAN)./exercise2$(NC)  - Polynomial calculator exercise (complete)"
	@echo ""
	@echo "Run '$(YELLOW)make help$(NC)' for more options."

# =============================================================================
# BUILD TARGETS
# =============================================================================

example1: $(SRC_DIR)/example1.c
	@echo "$(CYAN)Building example1...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)✓ example1 built$(NC)"

exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(CYAN)Building exercise1...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)✓ exercise1 built$(NC)"

exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(CYAN)Building exercise2...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)✓ exercise2 built$(NC)"

# =============================================================================
# SOLUTION TARGETS (Instructor only)
# =============================================================================

solutions: $(SOLUTIONS)
	@echo "$(GREEN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(GREEN)✓ All solutions built successfully$(NC)"
	@echo "$(GREEN)═══════════════════════════════════════════════════════════════$(NC)"

exercise1_sol: $(SOL_DIR)/exercise1_sol.c
	@echo "$(MAGENTA)Building exercise1_sol...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)✓ exercise1_sol built$(NC)"

exercise2_sol: $(SOL_DIR)/exercise2_sol.c
	@echo "$(MAGENTA)Building exercise2_sol...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)✓ exercise2_sol built$(NC)"

homework1_sol: $(SOL_DIR)/homework1_sol.c
	@echo "$(MAGENTA)Building homework1_sol...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)✓ homework1_sol built$(NC)"

homework2_sol: $(SOL_DIR)/homework2_sol.c
	@echo "$(MAGENTA)Building homework2_sol...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)✓ homework2_sol built$(NC)"

# =============================================================================
# RUN TARGETS
# =============================================================================

run: example1
	@echo ""
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)Running example1 demonstration...$(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@./example1

run-exercise1: exercise1
	@echo ""
	@echo "$(YELLOW)Running exercise1...$(NC)"
	@./exercise1

run-exercise1-file: exercise1
	@echo ""
	@echo "$(YELLOW)Running exercise1 with data file...$(NC)"
	@./exercise1 $(DATA_DIR)/students.txt

run-exercise2: exercise2
	@echo ""
	@echo "$(YELLOW)Running exercise2...$(NC)"
	@./exercise2

run-solutions: solutions
	@echo ""
	@echo "$(MAGENTA)Running all solutions...$(NC)"
	@echo ""
	@echo "$(CYAN)--- Exercise 1 Solution ---$(NC)"
	@./exercise1_sol
	@echo ""
	@echo "$(CYAN)--- Exercise 2 Solution ---$(NC)"
	@./exercise2_sol
	@echo ""
	@echo "$(CYAN)--- Homework 1 Solution ---$(NC)"
	@./homework1_sol
	@echo ""
	@echo "$(CYAN)--- Homework 2 Solution ---$(NC)"
	@./homework2_sol

# =============================================================================
# TEST TARGETS
# =============================================================================

test: exercise1 exercise2
	@echo ""
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)Running automated tests...$(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(CYAN)Test 1: Student Records$(NC)"
	@./exercise1 $(TEST_DIR)/test1_input.txt > /tmp/test1_output.txt 2>&1
	@if diff -q /tmp/test1_output.txt $(TEST_DIR)/test1_expected.txt > /dev/null 2>&1; then \
		echo "$(GREEN)✓ Test 1 PASSED$(NC)"; \
	else \
		echo "$(RED)✗ Test 1 FAILED$(NC)"; \
		echo "  Expected output differs from actual output"; \
	fi
	@echo ""
	@echo "$(CYAN)Test 2: Polynomial Calculator$(NC)"
	@./exercise2 $(TEST_DIR)/test2_input.txt > /tmp/test2_output.txt 2>&1
	@if diff -q /tmp/test2_output.txt $(TEST_DIR)/test2_expected.txt > /dev/null 2>&1; then \
		echo "$(GREEN)✓ Test 2 PASSED$(NC)"; \
	else \
		echo "$(RED)✗ Test 2 FAILED$(NC)"; \
		echo "  Expected output differs from actual output"; \
	fi
	@echo ""
	@echo "$(GREEN)Testing complete.$(NC)"

test-solutions: exercise1_sol exercise2_sol
	@echo ""
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)Running solution smoke tests...$(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(CYAN)Solution 1: Student Records$(NC)"
	@./exercise1_sol $(TEST_DIR)/test1_input.txt > /tmp/test1_sol_output.txt 2>&1
	@echo "$(GREEN)✓ Executed$(NC)"
	@echo ""
	@echo "$(CYAN)Solution 2: Polynomial Calculator$(NC)"
	@./exercise2_sol > /tmp/test2_sol_output.txt 2>&1
	@echo "$(GREEN)✓ Executed$(NC)"

# =============================================================================
# MEMORY CHECK TARGETS
# =============================================================================

valgrind: example1
	@echo ""
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)Running Valgrind memory check on example1...$(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./example1

valgrind-solutions: solutions
	@echo ""
	@echo "$(YELLOW)Running Valgrind on all solutions...$(NC)"
	@echo ""
	@echo "$(CYAN)--- Exercise 1 Solution ---$(NC)"
	@valgrind --leak-check=full --error-exitcode=1 ./exercise1_sol > /dev/null 2>&1 && \
		echo "$(GREEN)✓ No memory leaks$(NC)" || echo "$(RED)✗ Memory leaks detected$(NC)"
	@echo ""
	@echo "$(CYAN)--- Exercise 2 Solution ---$(NC)"
	@valgrind --leak-check=full --error-exitcode=1 ./exercise2_sol > /dev/null 2>&1 && \
		echo "$(GREEN)✓ No memory leaks$(NC)" || echo "$(RED)✗ Memory leaks detected$(NC)"
	@echo ""
	@echo "$(CYAN)--- Homework 1 Solution ---$(NC)"
	@valgrind --leak-check=full --error-exitcode=1 ./homework1_sol > /dev/null 2>&1 && \
		echo "$(GREEN)✓ No memory leaks$(NC)" || echo "$(RED)✗ Memory leaks detected$(NC)"
	@echo ""
	@echo "$(CYAN)--- Homework 2 Solution ---$(NC)"
	@valgrind --leak-check=full --error-exitcode=1 ./homework2_sol > /dev/null 2>&1 && \
		echo "$(GREEN)✓ No memory leaks$(NC)" || echo "$(RED)✗ Memory leaks detected$(NC)"

# =============================================================================
# CHECK TARGET (Compile check without linking)
# =============================================================================

check:
	@echo "$(YELLOW)Checking source files for compilation errors...$(NC)"
	@echo ""
	@for f in $(SRC_DIR)/*.c; do \
		echo "$(CYAN)Checking $$f...$(NC)"; \
		$(CC) $(CFLAGS) -fsyntax-only $$f && echo "$(GREEN)✓ OK$(NC)" || echo "$(RED)✗ ERRORS$(NC)"; \
	done

# =============================================================================
# CLEAN TARGET
# =============================================================================

clean:
	@echo "$(YELLOW)Cleaning build artifacts...$(NC)"
	@rm -f $(TARGETS) $(SOLUTIONS)
	@rm -f *.o
	@rm -f /tmp/test*_output.txt
	@echo "$(GREEN)✓ Clean complete$(NC)"

# =============================================================================
# HELP TARGET
# =============================================================================

help:
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)   WEEK 04: LINKED LISTS - Build System                        $(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(YELLOW)Student Targets:$(NC)"
	@echo "  $(GREEN)make$(NC)              - Build all student exercises"
	@echo "  $(GREEN)make run$(NC)          - Run the example demonstration"
	@echo "  $(GREEN)make run-exercise1$(NC) - Run exercise 1 in demo mode"
	@echo "  $(GREEN)make run-exercise1-file$(NC) - Run exercise 1 with data file"
	@echo "  $(GREEN)make run-exercise2$(NC) - Run exercise 2"
	@echo "  $(GREEN)make valgrind$(NC)     - Check example1 for memory leaks"
	@echo "  $(GREEN)make check$(NC)        - Check source files for syntax errors"
	@echo "  $(GREEN)make clean$(NC)        - Remove all build artifacts"
	@echo ""
	@echo "$(YELLOW)Instructor Targets:$(NC)"
	@echo "  $(MAGENTA)make solutions$(NC)    - Build all solution files"
	@echo "  $(MAGENTA)make run-solutions$(NC) - Run all solution demonstrations"
	@echo "  $(MAGENTA)make test$(NC)         - Run automated tests"
	@echo "  $(MAGENTA)make test-solutions$(NC) - Run solution smoke tests"
	@echo "  $(MAGENTA)make valgrind-solutions$(NC) - Memory check all solutions"
	@echo ""
	@echo "$(YELLOW)Source Files:$(NC)"
	@echo "  $(SRC_DIR)/example1.c   - Complete working demonstration"
	@echo "  $(SRC_DIR)/exercise1.c  - Student records (complete implementation)"
	@echo "  $(SRC_DIR)/exercise2.c  - Polynomial calculator (complete implementation)"
	@echo ""
	@echo "$(YELLOW)Data Files:$(NC)"
	@echo "  $(DATA_DIR)/students.txt     - Sample student data"
	@echo "  $(DATA_DIR)/polynomials.txt  - Sample polynomial data"
	@echo ""
