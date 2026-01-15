# =============================================================================
# WEEK 06: QUEUES - Makefile
# ATP - Algorithms and Programming Techniques
# =============================================================================

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS = 

# Directories
SRC_DIR = src
DATA_DIR = data
TEST_DIR = tests
SOL_DIR = solution
SLIDES_DIR = slides

# Colours for terminal output
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
CYAN = \033[0;36m
MAGENTA = \033[0;35m
NC = \033[0m

# Targets
TARGETS = example1 exercise1 exercise2
SOL_TARGETS = exercise1_sol exercise2_sol homework1_sol homework2_sol

# =============================================================================
# DEFAULT TARGET
# =============================================================================

.PHONY: all clean run test valgrind help solutions slides

all: $(TARGETS)
	@echo "$(GREEN)✓ All targets built successfully$(NC)"
	@echo ""
	@echo "$(CYAN)Available executables:$(NC)"
	@echo "  ./example1   - Complete queue demonstration"
	@echo "  ./exercise1  - Circular buffer exercise"
	@echo "  ./exercise2  - Task scheduler exercise"
	@echo ""
	@echo "$(YELLOW)Run 'make help' for more options$(NC)"

# =============================================================================
# BUILD TARGETS
# =============================================================================

example1: $(SRC_DIR)/example1.c
	@echo "$(CYAN)Building example1...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ example1 built$(NC)"

exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(CYAN)Building exercise1...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise1 built$(NC)"

exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(CYAN)Building exercise2...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise2 built$(NC)"

# =============================================================================
# SOLUTION TARGETS (Instructor Only)
# =============================================================================

solutions: $(SOL_TARGETS)
	@echo "$(GREEN)✓ All solutions built$(NC)"

exercise1_sol: $(SOL_DIR)/exercise1_sol.c
	@echo "$(MAGENTA)Building exercise1 solution...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise1_sol built$(NC)"

exercise2_sol: $(SOL_DIR)/exercise2_sol.c
	@echo "$(MAGENTA)Building exercise2 solution...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise2_sol built$(NC)"

homework1_sol: $(SOL_DIR)/homework1_sol.c
	@echo "$(MAGENTA)Building homework1 solution...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ homework1_sol built$(NC)"

homework2_sol: $(SOL_DIR)/homework2_sol.c
	@echo "$(MAGENTA)Building homework2 solution...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ homework2_sol built$(NC)"

# =============================================================================
# RUN TARGETS
# =============================================================================

run: example1
	@echo ""
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)  Running Queue Demonstration$(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@./example1

run-ex1: exercise1
	@echo ""
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)  Running Exercise 1: Circular Buffer Queue$(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@./exercise1

run-ex2: exercise2
	@echo ""
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)  Running Exercise 2: Task Scheduler$(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)Using: $(DATA_DIR)/processes.txt, quantum=3$(NC)"
	@./exercise2 $(DATA_DIR)/processes.txt 3

demo-all: all
	@echo ""
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)  Running Complete Demonstration$(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@./example1
	@echo ""
	@echo "$(GREEN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(GREEN)  Demonstration complete!$(NC)"
	@echo "$(GREEN)═══════════════════════════════════════════════════════════════$(NC)"

# =============================================================================
# TEST TARGETS
# =============================================================================

test: test-ex1 test-ex2
	@echo ""
	@echo "$(GREEN)✓ All tests completed$(NC)"

test-ex1: exercise1_sol
	@echo ""
	@echo "$(YELLOW)Testing Exercise 1...$(NC)"
	@./exercise1_sol < $(TEST_DIR)/test1_input.txt > /tmp/test1_output.txt 2>&1 || true
	@if grep -q "Enqueued: 10" /tmp/test1_output.txt && grep -q "Dequeued: 10" /tmp/test1_output.txt; then \
		echo "$(GREEN)  ✓ Exercise 1 basic tests passed$(NC)"; \
	else \
		echo "$(RED)  ✗ Exercise 1 tests failed$(NC)"; \
	fi

test-ex2: exercise2_sol
	@echo ""
	@echo "$(YELLOW)Testing Exercise 2...$(NC)"
	@./exercise2_sol $(TEST_DIR)/test2_input.txt 2 > /tmp/test2_output.txt 2>&1 || true
	@if grep -q "Loaded 3 processes" /tmp/test2_output.txt; then \
		echo "$(GREEN)  ✓ Exercise 2 basic tests passed$(NC)"; \
	else \
		echo "$(RED)  ✗ Exercise 2 tests failed$(NC)"; \
	fi

# =============================================================================
# MEMORY CHECK TARGETS
# =============================================================================

valgrind: example1
	@echo ""
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)  Running Valgrind Memory Check$(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./example1 2>&1 | tail -20

valgrind-ex1: exercise1_sol
	@echo ""
	@echo "$(YELLOW)Checking Exercise 1 for memory leaks...$(NC)"
	@echo "ENQUEUE 10\nENQUEUE 20\nDEQUEUE\nQUIT" | valgrind --leak-check=full ./exercise1_sol 2>&1 | grep -E "(definitely|indirectly|possibly) lost"

valgrind-ex2: exercise2_sol
	@echo ""
	@echo "$(YELLOW)Checking Exercise 2 for memory leaks...$(NC)"
	@valgrind --leak-check=full ./exercise2_sol $(DATA_DIR)/processes.txt 3 2>&1 | grep -E "(definitely|indirectly|possibly) lost"

# =============================================================================
# SLIDES TARGET
# =============================================================================

slides:
	@echo ""
	@echo "$(CYAN)Opening presentation slides...$(NC)"
	@if command -v xdg-open > /dev/null; then \
		xdg-open $(SLIDES_DIR)/presentation-week06.html; \
	elif command -v open > /dev/null; then \
		open $(SLIDES_DIR)/presentation-week06.html; \
	else \
		echo "$(YELLOW)Please open $(SLIDES_DIR)/presentation-week06.html manually$(NC)"; \
	fi

slides-compare:
	@echo ""
	@echo "$(CYAN)Opening comparison presentation...$(NC)"
	@if command -v xdg-open > /dev/null; then \
		xdg-open $(SLIDES_DIR)/presentation-comparativ.html; \
	elif command -v open > /dev/null; then \
		open $(SLIDES_DIR)/presentation-comparativ.html; \
	else \
		echo "$(YELLOW)Please open $(SLIDES_DIR)/presentation-comparativ.html manually$(NC)"; \
	fi

# =============================================================================
# CLEAN TARGET
# =============================================================================

clean:
	@echo "$(YELLOW)Cleaning build artifacts...$(NC)"
	@rm -f $(TARGETS) $(SOL_TARGETS)
	@rm -f *.o
	@rm -f /tmp/test*_output.txt
	@echo "$(GREEN)✓ Clean complete$(NC)"

# =============================================================================
# HELP TARGET
# =============================================================================

help:
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)  Week 06: Queues - Makefile Help$(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(YELLOW)Build targets:$(NC)"
	@echo "  make              Build all exercise executables"
	@echo "  make example1     Build the demonstration program"
	@echo "  make exercise1    Build exercise 1 (circular buffer)"
	@echo "  make exercise2    Build exercise 2 (scheduler)"
	@echo "  make solutions    Build solution files (instructor)"
	@echo ""
	@echo "$(YELLOW)Run targets:$(NC)"
	@echo "  make run          Run the main demonstration"
	@echo "  make run-ex1      Run exercise 1 interactively"
	@echo "  make run-ex2      Run exercise 2 with sample data"
	@echo "  make demo-all     Run complete demonstration"
	@echo ""
	@echo "$(YELLOW)Test targets:$(NC)"
	@echo "  make test         Run all automated tests"
	@echo "  make test-ex1     Run exercise 1 tests"
	@echo "  make test-ex2     Run exercise 2 tests"
	@echo ""
	@echo "$(YELLOW)Memory check targets:$(NC)"
	@echo "  make valgrind     Check example1 for memory leaks"
	@echo "  make valgrind-ex1 Check exercise 1 solution"
	@echo "  make valgrind-ex2 Check exercise 2 solution"
	@echo ""
	@echo "$(YELLOW)Other targets:$(NC)"
	@echo "  make slides       Open main lecture presentation"
	@echo "  make slides-compare  Open language comparison slides"
	@echo "  make clean        Remove all built files"
	@echo "  make help         Show this help message"
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"

# =============================================================================
# PHONY DECLARATIONS
# =============================================================================

.PHONY: all clean run run-ex1 run-ex2 demo-all test test-ex1 test-ex2
.PHONY: valgrind valgrind-ex1 valgrind-ex2 solutions slides slides-compare help
