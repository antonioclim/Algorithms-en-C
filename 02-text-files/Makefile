# =============================================================================
# WEEK 02: TEXT FILE PROCESSING - Makefile
# =============================================================================
#
# Algorithms and Programming Techniques (ATP)
# Academy of Economic Studies, Bucharest
#
# Usage:
#   make            - Build all targets
#   make run        - Build and run the example
#   make test       - Run automated tests
#   make valgrind   - Run with memory checking
#   make clean      - Remove build artifacts
#   make help       - Show this help message
#
# =============================================================================

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS =

# Directories
SRC_DIR = src
SOL_DIR = solution
DATA_DIR = data
TEST_DIR = tests
OUT_DIR = output

# ANSI colour codes
GREEN  = \033[0;32m
RED    = \033[0;31m
YELLOW = \033[0;33m
BLUE   = \033[0;34m
CYAN   = \033[0;36m
BOLD   = \033[1m
NC     = \033[0m

# Source files and targets
EXAMPLE = example1
EXERCISES = exercise1 exercise2
SOLUTIONS = exercise1_sol exercise2_sol homework1_sol homework2_sol

# All targets
ALL_TARGETS = $(EXAMPLE) $(EXERCISES)
SOL_TARGETS = $(SOLUTIONS)

# =============================================================================
# MAIN TARGETS
# =============================================================================

.PHONY: all clean run test valgrind help solutions debug

# Default target: build example and exercises
all: $(ALL_TARGETS)
	@echo "$(GREEN)$(BOLD)✓ All targets built successfully$(NC)"
	@echo "$(CYAN)Run 'make run' to execute the example$(NC)"

# Build solutions (instructor only)
solutions: $(SOL_TARGETS)
	@echo "$(GREEN)$(BOLD)✓ All solutions built$(NC)"

# =============================================================================
# COMPILATION RULES
# =============================================================================

# Example
$(EXAMPLE): $(SRC_DIR)/$(EXAMPLE).c
	@echo "$(CYAN)Building $(EXAMPLE)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ $(EXAMPLE) built$(NC)"

# Exercise 1
exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(CYAN)Building exercise1...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise1 built$(NC)"

# Exercise 2
exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(CYAN)Building exercise2...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise2 built$(NC)"

# Solutions
exercise1_sol: $(SOL_DIR)/exercise1_sol.c
	@echo "$(CYAN)Building exercise1_sol...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise1_sol built$(NC)"

exercise2_sol: $(SOL_DIR)/exercise2_sol.c
	@echo "$(CYAN)Building exercise2_sol...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise2_sol built$(NC)"

homework1_sol: $(SOL_DIR)/homework1_sol.c
	@echo "$(CYAN)Building homework1_sol...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ homework1_sol built$(NC)"

homework2_sol: $(SOL_DIR)/homework2_sol.c
	@echo "$(CYAN)Building homework2_sol...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ homework2_sol built$(NC)"

# Debug build with extra symbols
debug: CFLAGS += -DDEBUG -O0
debug: clean all
	@echo "$(YELLOW)Debug build complete$(NC)"

# =============================================================================
# EXECUTION TARGETS
# =============================================================================

# Run the example
run: $(EXAMPLE)
	@echo ""
	@echo "$(YELLOW)$(BOLD)Running $(EXAMPLE)...$(NC)"
	@echo "$(YELLOW)════════════════════════════════════════════════════════════════$(NC)"
	@./$(EXAMPLE)
	@echo "$(YELLOW)════════════════════════════════════════════════════════════════$(NC)"

# Run exercise 1 solution
run-ex1: exercise1_sol
	@mkdir -p $(OUT_DIR)
	@echo ""
	@echo "$(YELLOW)$(BOLD)Running Exercise 1 Solution...$(NC)"
	@./exercise1_sol

# Run exercise 2 solution
run-ex2: exercise2_sol
	@echo ""
	@echo "$(YELLOW)$(BOLD)Running Exercise 2 Solution...$(NC)"
	@./exercise2_sol

# =============================================================================
# TESTING TARGETS
# =============================================================================

test: exercise1_sol exercise2_sol
	@echo ""
	@echo "$(YELLOW)$(BOLD)Running Automated Tests$(NC)"
	@echo "$(YELLOW)────────────────────────────────────────────────────────────────$(NC)"
	@mkdir -p $(OUT_DIR)
	@echo ""
	@echo "$(CYAN)Test 1: Exercise 1 (Student Grade Processor)$(NC)"
	@./exercise1_sol > /dev/null 2>&1 && \
		echo "$(GREEN)  ✓ Exercise 1 runs without errors$(NC)" || \
		echo "$(RED)  ✗ Exercise 1 failed$(NC)"
	@echo ""
	@echo "$(CYAN)Test 2: Exercise 2 (CSV Transformer)$(NC)"
	@./exercise2_sol > /dev/null 2>&1 && \
		echo "$(GREEN)  ✓ Exercise 2 runs without errors$(NC)" || \
		echo "$(RED)  ✗ Exercise 2 failed$(NC)"
	@echo ""
	@echo "$(YELLOW)────────────────────────────────────────────────────────────────$(NC)"
	@echo "$(GREEN)$(BOLD)Tests completed$(NC)"

# Verbose tests with output comparison
test-verbose: exercise1_sol exercise2_sol
	@echo ""
	@echo "$(YELLOW)$(BOLD)Running Verbose Tests$(NC)"
	@echo ""
	@echo "$(CYAN)Exercise 1 Output:$(NC)"
	@./exercise1_sol
	@echo ""
	@echo "$(CYAN)Exercise 2 Output:$(NC)"
	@./exercise2_sol

# =============================================================================
# MEMORY CHECKING
# =============================================================================

valgrind: $(EXAMPLE)
	@echo ""
	@echo "$(YELLOW)$(BOLD)Running Valgrind Memory Check$(NC)"
	@echo "$(YELLOW)────────────────────────────────────────────────────────────────$(NC)"
	@valgrind --leak-check=full \
	          --show-leak-kinds=all \
	          --track-origins=yes \
	          --verbose \
	          ./$(EXAMPLE) 2>&1 | tail -20
	@echo "$(YELLOW)────────────────────────────────────────────────────────────────$(NC)"

valgrind-ex1: exercise1_sol
	@mkdir -p $(OUT_DIR)
	@echo "$(YELLOW)Running Valgrind on Exercise 1...$(NC)"
	@valgrind --leak-check=full ./exercise1_sol

valgrind-ex2: exercise2_sol
	@echo "$(YELLOW)Running Valgrind on Exercise 2...$(NC)"
	@valgrind --leak-check=full ./exercise2_sol

# =============================================================================
# CLEANUP
# =============================================================================

clean:
	@echo "$(YELLOW)Cleaning build artifacts...$(NC)"
	@rm -f $(ALL_TARGETS) $(SOL_TARGETS)
	@rm -f *.o
	@rm -rf $(OUT_DIR)
	@rm -f test_*.txt students_demo.txt products_demo.csv sample_text.txt
	@echo "$(GREEN)  ✓ Clean complete$(NC)"

# Clean everything including generated files
distclean: clean
	@rm -f *.log *.tmp
	@echo "$(GREEN)  ✓ Distribution clean complete$(NC)"

# =============================================================================
# HELP
# =============================================================================

help:
	@echo ""
	@echo "$(BOLD)$(CYAN)Week 02: Text File Processing - Makefile$(NC)"
	@echo "$(CYAN)════════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(BOLD)Build Targets:$(NC)"
	@echo "  $(GREEN)make$(NC)            Build example and exercises"
	@echo "  $(GREEN)make solutions$(NC)  Build solution files (instructor)"
	@echo "  $(GREEN)make debug$(NC)      Build with debug symbols"
	@echo ""
	@echo "$(BOLD)Run Targets:$(NC)"
	@echo "  $(GREEN)make run$(NC)        Run the complete example"
	@echo "  $(GREEN)make run-ex1$(NC)    Run exercise 1 solution"
	@echo "  $(GREEN)make run-ex2$(NC)    Run exercise 2 solution"
	@echo ""
	@echo "$(BOLD)Test Targets:$(NC)"
	@echo "  $(GREEN)make test$(NC)       Run automated tests"
	@echo "  $(GREEN)make test-verbose$(NC)  Run tests with full output"
	@echo ""
	@echo "$(BOLD)Memory Checking:$(NC)"
	@echo "  $(GREEN)make valgrind$(NC)   Check example for memory leaks"
	@echo "  $(GREEN)make valgrind-ex1$(NC)  Check exercise 1 for leaks"
	@echo "  $(GREEN)make valgrind-ex2$(NC)  Check exercise 2 for leaks"
	@echo ""
	@echo "$(BOLD)Cleanup:$(NC)"
	@echo "  $(GREEN)make clean$(NC)      Remove build artifacts"
	@echo "  $(GREEN)make distclean$(NC)  Remove all generated files"
	@echo ""
	@echo "$(CYAN)════════════════════════════════════════════════════════════════$(NC)"
	@echo ""

# =============================================================================
# SPECIAL TARGETS
# =============================================================================

# Prevent make from removing intermediate files
.PRECIOUS: %.o

# Mark targets that don't create files
.PHONY: all clean run test valgrind help solutions debug \
        run-ex1 run-ex2 test-verbose valgrind-ex1 valgrind-ex2 distclean
