# =============================================================================
# WEEK 09: AVL TREES - Makefile
# Algorithms and Programming Techniques
# =============================================================================

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
CFLAGS_RELEASE = -Wall -Wextra -std=c11 -O2

# Directories
SRC_DIR = src
SOL_DIR = solution
DATA_DIR = data
TEST_DIR = tests

# Colours for terminal output
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
CYAN = \033[0;36m
MAGENTA = \033[0;35m
NC = \033[0m

# Source files
EXAMPLE = example1
EXERCISE1 = exercise1
EXERCISE2 = exercise2

# Solution files
EXERCISE1_SOL = exercise1_sol
EXERCISE2_SOL = exercise2_sol
HOMEWORK1_SOL = homework1_sol
HOMEWORK2_SOL = homework2_sol

# All targets
TARGETS = $(EXAMPLE) $(EXERCISE1) $(EXERCISE2)
SOLUTIONS = $(EXERCISE1_SOL) $(EXERCISE2_SOL) $(HOMEWORK1_SOL) $(HOMEWORK2_SOL)

# =============================================================================
# MAIN TARGETS
# =============================================================================

.PHONY: all clean run test valgrind help solutions

all: $(TARGETS)
	@echo "$(GREEN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(GREEN)║              All targets built successfully ✓                 ║$(NC)"
	@echo "$(GREEN)╚═══════════════════════════════════════════════════════════════╝$(NC)"

# =============================================================================
# BUILD TARGETS
# =============================================================================

$(EXAMPLE): $(SRC_DIR)/$(EXAMPLE).c
	@echo "$(CYAN)Building $(EXAMPLE)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  ✓ $(EXAMPLE) built$(NC)"

$(EXERCISE1): $(SRC_DIR)/$(EXERCISE1).c
	@echo "$(CYAN)Building $(EXERCISE1)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  ✓ $(EXERCISE1) built$(NC)"

$(EXERCISE2): $(SRC_DIR)/$(EXERCISE2).c
	@echo "$(CYAN)Building $(EXERCISE2)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  ✓ $(EXERCISE2) built$(NC)"

# =============================================================================
# SOLUTION TARGETS
# =============================================================================

solutions: $(SOLUTIONS)
	@echo "$(GREEN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(GREEN)║              All solutions built successfully ✓               ║$(NC)"
	@echo "$(GREEN)╚═══════════════════════════════════════════════════════════════╝$(NC)"

$(EXERCISE1_SOL): $(SOL_DIR)/$(EXERCISE1_SOL).c
	@echo "$(MAGENTA)Building $(EXERCISE1_SOL)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  ✓ $(EXERCISE1_SOL) built$(NC)"

$(EXERCISE2_SOL): $(SOL_DIR)/$(EXERCISE2_SOL).c
	@echo "$(MAGENTA)Building $(EXERCISE2_SOL)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  ✓ $(EXERCISE2_SOL) built$(NC)"

$(HOMEWORK1_SOL): $(SOL_DIR)/$(HOMEWORK1_SOL).c
	@echo "$(MAGENTA)Building $(HOMEWORK1_SOL)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  ✓ $(HOMEWORK1_SOL) built$(NC)"

$(HOMEWORK2_SOL): $(SOL_DIR)/$(HOMEWORK2_SOL).c
	@echo "$(MAGENTA)Building $(HOMEWORK2_SOL)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  ✓ $(HOMEWORK2_SOL) built$(NC)"

# =============================================================================
# RUN TARGETS
# =============================================================================

run: $(EXAMPLE)
	@echo "$(YELLOW)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(NC)"
	@echo "$(YELLOW)Running $(EXAMPLE)...$(NC)"
	@echo "$(YELLOW)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(NC)"
	@./$(EXAMPLE)

run-ex1: $(EXERCISE1)
	@echo "$(YELLOW)Running $(EXERCISE1)...$(NC)"
	@./$(EXERCISE1)

run-ex2: $(EXERCISE2)
	@echo "$(YELLOW)Running $(EXERCISE2)...$(NC)"
	@./$(EXERCISE2)

run-sol1: $(EXERCISE1_SOL)
	@echo "$(YELLOW)Running $(EXERCISE1_SOL)...$(NC)"
	@./$(EXERCISE1_SOL)

run-sol2: $(EXERCISE2_SOL)
	@echo "$(YELLOW)Running $(EXERCISE2_SOL)...$(NC)"
	@./$(EXERCISE2_SOL)

run-hw1: $(HOMEWORK1_SOL)
	@echo "$(YELLOW)Running $(HOMEWORK1_SOL)...$(NC)"
	@./$(HOMEWORK1_SOL)

run-hw2: $(HOMEWORK2_SOL)
	@echo "$(YELLOW)Running $(HOMEWORK2_SOL)...$(NC)"
	@./$(HOMEWORK2_SOL)

run-all: all solutions
	@echo "$(YELLOW)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(NC)"
	@echo "$(YELLOW)Running all examples and solutions...$(NC)"
	@echo "$(YELLOW)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(NC)"
	@./$(EXAMPLE)
	@echo ""
	@./$(EXERCISE1_SOL)
	@echo ""
	@./$(EXERCISE2_SOL)

# =============================================================================
# TEST TARGETS
# =============================================================================

test: $(EXERCISE1_SOL) $(EXERCISE2_SOL)
	@echo "$(YELLOW)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(NC)"
	@echo "$(YELLOW)Running automated tests...$(NC)"
	@echo "$(YELLOW)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(NC)"
	@echo ""
	@echo "$(CYAN)Testing Exercise 1 Solution:$(NC)"
	@./$(EXERCISE1_SOL) > /tmp/ex1_output.txt 2>&1
	@if grep -q "All Tests Passed" /tmp/ex1_output.txt; then \
		echo "$(GREEN)  ✓ Exercise 1 tests passed$(NC)"; \
	else \
		echo "$(RED)  ✗ Exercise 1 tests failed$(NC)"; \
	fi
	@echo ""
	@echo "$(CYAN)Testing Exercise 2 Solution:$(NC)"
	@./$(EXERCISE2_SOL) > /tmp/ex2_output.txt 2>&1
	@if grep -q "All Tests Passed" /tmp/ex2_output.txt; then \
		echo "$(GREEN)  ✓ Exercise 2 tests passed$(NC)"; \
	else \
		echo "$(RED)  ✗ Exercise 2 tests failed$(NC)"; \
	fi
	@echo ""
	@echo "$(GREEN)Testing complete.$(NC)"

# =============================================================================
# MEMORY CHECK
# =============================================================================

valgrind: $(EXAMPLE)
	@echo "$(YELLOW)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(NC)"
	@echo "$(YELLOW)Running Valgrind memory check on $(EXAMPLE)...$(NC)"
	@echo "$(YELLOW)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(EXAMPLE)

valgrind-sol1: $(EXERCISE1_SOL)
	@echo "$(YELLOW)Running Valgrind on $(EXERCISE1_SOL)...$(NC)"
	@valgrind --leak-check=full ./$(EXERCISE1_SOL)

valgrind-sol2: $(EXERCISE2_SOL)
	@echo "$(YELLOW)Running Valgrind on $(EXERCISE2_SOL)...$(NC)"
	@valgrind --leak-check=full ./$(EXERCISE2_SOL)

valgrind-hw1: $(HOMEWORK1_SOL)
	@echo "$(YELLOW)Running Valgrind on $(HOMEWORK1_SOL)...$(NC)"
	@valgrind --leak-check=full ./$(HOMEWORK1_SOL)

# =============================================================================
# CLEAN
# =============================================================================

clean:
	@echo "$(YELLOW)Cleaning build artifacts...$(NC)"
	@rm -f $(TARGETS) $(SOLUTIONS)
	@rm -f *.o
	@rm -f /tmp/ex1_output.txt /tmp/ex2_output.txt
	@echo "$(GREEN)  ✓ Clean complete$(NC)"

# =============================================================================
# HELP
# =============================================================================

help:
	@echo "$(CYAN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)║           Week 09: AVL Trees - Available Targets              ║$(NC)"
	@echo "$(CYAN)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(GREEN)Build targets:$(NC)"
	@echo "  make              - Build all student exercises"
	@echo "  make solutions    - Build all solution files"
	@echo "  make example1     - Build the demonstration example"
	@echo "  make exercise1    - Build exercise 1 (student version)"
	@echo "  make exercise2    - Build exercise 2 (student version)"
	@echo ""
	@echo "$(GREEN)Run targets:$(NC)"
	@echo "  make run          - Run the demonstration example"
	@echo "  make run-ex1      - Run exercise 1"
	@echo "  make run-ex2      - Run exercise 2"
	@echo "  make run-sol1     - Run exercise 1 solution"
	@echo "  make run-sol2     - Run exercise 2 solution"
	@echo "  make run-hw1      - Run homework 1 solution"
	@echo "  make run-hw2      - Run homework 2 solution"
	@echo "  make run-all      - Run all examples and solutions"
	@echo ""
	@echo "$(GREEN)Test targets:$(NC)"
	@echo "  make test         - Run automated tests"
	@echo "  make valgrind     - Check example for memory leaks"
	@echo "  make valgrind-sol1 - Check exercise 1 solution for leaks"
	@echo "  make valgrind-sol2 - Check exercise 2 solution for leaks"
	@echo "  make valgrind-hw1  - Check homework 1 for leaks"
	@echo ""
	@echo "$(GREEN)Other targets:$(NC)"
	@echo "  make clean        - Remove all build artifacts"
	@echo "  make help         - Display this help message"
	@echo ""
	@echo "$(YELLOW)Tip: Use 'make run' to see the complete AVL demonstration!$(NC)"

# =============================================================================
# END OF MAKEFILE
# =============================================================================
