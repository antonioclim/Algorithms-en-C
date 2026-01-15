# =============================================================================
# WEEK 08: Binary Search Trees - Makefile
# =============================================================================
# Algorithms and Programming Techniques (ATP)
# Academy of Economic Studies - CSIE Bucharest
#
# Usage:
#   make          - Build all targets
#   make run      - Run example demonstration
#   make test     - Run automated tests
#   make valgrind - Check for memory leaks
#   make clean    - Remove build artifacts
#   make help     - Display this help
# =============================================================================

# Compiler configuration
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS = 

# Directories
SRC_DIR = src
DATA_DIR = data
TEST_DIR = tests
SOL_DIR = solution

# ANSI Colour Codes
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
CYAN = \033[0;36m
MAGENTA = \033[0;35m
BOLD = \033[1m
NC = \033[0m

# Unicode symbols
CHECK = ✓
CROSS = ✗
ARROW = →
GEAR = ⚙

# Target executables
TARGETS = example1 exercise1 exercise2
SOLUTIONS = exercise1_sol exercise2_sol homework1_sol homework2_sol

# =============================================================================
# PHONY TARGETS
# =============================================================================
.PHONY: all clean run test valgrind solutions help info dirs

# =============================================================================
# DEFAULT TARGET
# =============================================================================
all: dirs $(TARGETS)
	@echo ""
	@echo "$(GREEN)$(CHECK) All targets built successfully$(NC)"
	@echo "$(CYAN)$(ARROW) Run 'make run' to execute the example$(NC)"
	@echo "$(CYAN)$(ARROW) Run 'make test' to execute automated tests$(NC)"
	@echo ""

# =============================================================================
# DIRECTORY CREATION
# =============================================================================
dirs:
	@mkdir -p $(DATA_DIR) $(TEST_DIR)

# =============================================================================
# MAIN TARGETS
# =============================================================================
example1: $(SRC_DIR)/example1.c
	@echo "$(CYAN)$(GEAR) Building example1...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) example1 built$(NC)"

exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(CYAN)$(GEAR) Building exercise1...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) 2>&1 | while read line; do \
		if echo "$$line" | grep -q "error:"; then \
			echo "$(RED)  $(CROSS) $$line$(NC)"; \
		elif echo "$$line" | grep -q "warning:"; then \
			echo "$(YELLOW)  ! $$line$(NC)"; \
		else \
			echo "$$line"; \
		fi \
	done
	@if [ -f exercise1 ]; then echo "$(GREEN)  $(CHECK) exercise1 built$(NC)"; fi

exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(CYAN)$(GEAR) Building exercise2...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) 2>&1 | while read line; do \
		if echo "$$line" | grep -q "error:"; then \
			echo "$(RED)  $(CROSS) $$line$(NC)"; \
		elif echo "$$line" | grep -q "warning:"; then \
			echo "$(YELLOW)  ! $$line$(NC)"; \
		else \
			echo "$$line"; \
		fi \
	done
	@if [ -f exercise2 ]; then echo "$(GREEN)  $(CHECK) exercise2 built$(NC)"; fi

# =============================================================================
# SOLUTION TARGETS (Instructor Only)
# =============================================================================
solutions: dirs $(SOLUTIONS)
	@echo ""
	@echo "$(MAGENTA)$(CHECK) All solutions built$(NC)"
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
	@echo "$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BOLD)║     Running Week 08 Example: Binary Search Trees              ║$(NC)"
	@echo "$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./example1

run-exercise1: exercise1
	@echo ""
	@echo "$(YELLOW)$(ARROW) Running exercise1...$(NC)"
	@echo ""
	@./exercise1 < $(TEST_DIR)/test1_input.txt

run-exercise2: exercise2
	@echo ""
	@echo "$(YELLOW)$(ARROW) Running exercise2...$(NC)"
	@echo ""
	@./exercise2 < $(TEST_DIR)/test2_input.txt

# =============================================================================
# TEST TARGETS
# =============================================================================
test: test1 test2
	@echo ""
	@echo "$(GREEN)$(CHECK) All tests completed$(NC)"
	@echo ""

test1: exercise1
	@echo ""
	@echo "$(YELLOW)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(NC)"
	@echo "$(YELLOW)  Testing Exercise 1: Basic BST Operations$(NC)"
	@echo "$(YELLOW)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(NC)"
	@./exercise1 < $(TEST_DIR)/test1_input.txt > /tmp/test1_output.txt 2>&1 || true
	@if diff -q /tmp/test1_output.txt $(TEST_DIR)/test1_expected.txt > /dev/null 2>&1; then \
		echo "$(GREEN)  $(CHECK) Test 1 PASSED$(NC)"; \
	else \
		echo "$(RED)  $(CROSS) Test 1 FAILED$(NC)"; \
		echo "$(YELLOW)  Expected:$(NC)"; \
		head -10 $(TEST_DIR)/test1_expected.txt | sed 's/^/    /'; \
		echo "$(YELLOW)  Got:$(NC)"; \
		head -10 /tmp/test1_output.txt | sed 's/^/    /'; \
	fi

test2: exercise2
	@echo ""
	@echo "$(YELLOW)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(NC)"
	@echo "$(YELLOW)  Testing Exercise 2: BST Deletion & Advanced$(NC)"
	@echo "$(YELLOW)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(NC)"
	@./exercise2 < $(TEST_DIR)/test2_input.txt > /tmp/test2_output.txt 2>&1 || true
	@if diff -q /tmp/test2_output.txt $(TEST_DIR)/test2_expected.txt > /dev/null 2>&1; then \
		echo "$(GREEN)  $(CHECK) Test 2 PASSED$(NC)"; \
	else \
		echo "$(RED)  $(CROSS) Test 2 FAILED$(NC)"; \
		echo "$(YELLOW)  Expected:$(NC)"; \
		head -10 $(TEST_DIR)/test2_expected.txt | sed 's/^/    /'; \
		echo "$(YELLOW)  Got:$(NC)"; \
		head -10 /tmp/test2_output.txt | sed 's/^/    /'; \
	fi

# =============================================================================
# MEMORY CHECK TARGETS
# =============================================================================
valgrind: example1
	@echo ""
	@echo "$(YELLOW)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(NC)"
	@echo "$(YELLOW)  Running Valgrind Memory Check$(NC)"
	@echo "$(YELLOW)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(NC)"
	@echo ""
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./example1 2>&1 | \
		while read line; do \
			if echo "$$line" | grep -q "ERROR SUMMARY: 0"; then \
				echo "$(GREEN)$$line$(NC)"; \
			elif echo "$$line" | grep -q "ERROR"; then \
				echo "$(RED)$$line$(NC)"; \
			elif echo "$$line" | grep -q "definitely lost\|indirectly lost\|possibly lost"; then \
				echo "$(YELLOW)$$line$(NC)"; \
			else \
				echo "$$line"; \
			fi \
		done

valgrind-exercise1: exercise1
	@echo "$(YELLOW)$(ARROW) Memory check for exercise1...$(NC)"
	@valgrind --leak-check=full ./exercise1 < $(TEST_DIR)/test1_input.txt

valgrind-exercise2: exercise2
	@echo "$(YELLOW)$(ARROW) Memory check for exercise2...$(NC)"
	@valgrind --leak-check=full ./exercise2 < $(TEST_DIR)/test2_input.txt

# =============================================================================
# CLEAN TARGET
# =============================================================================
clean:
	@echo "$(YELLOW)$(GEAR) Cleaning build artifacts...$(NC)"
	@rm -f $(TARGETS) $(SOLUTIONS)
	@rm -f *.o
	@rm -f /tmp/test*_output.txt
	@echo "$(GREEN)$(CHECK) Clean complete$(NC)"

# =============================================================================
# INFO TARGET
# =============================================================================
info:
	@echo ""
	@echo "$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BOLD)║           Week 08: Binary Search Trees                        ║$(NC)"
	@echo "$(BOLD)║       Algorithms and Programming Techniques                   ║$(NC)"
	@echo "$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Compiler:$(NC) $(CC)"
	@echo "$(CYAN)Flags:$(NC) $(CFLAGS)"
	@echo ""
	@echo "$(CYAN)Source files:$(NC)"
	@ls -la $(SRC_DIR)/*.c 2>/dev/null | awk '{print "  " $$NF}'
	@echo ""
	@echo "$(CYAN)Test files:$(NC)"
	@ls -la $(TEST_DIR)/* 2>/dev/null | awk '{print "  " $$NF}'
	@echo ""

# =============================================================================
# HELP TARGET
# =============================================================================
help:
	@echo ""
	@echo "$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BOLD)║           Week 08: Binary Search Trees - Help                 ║$(NC)"
	@echo "$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(BOLD)Available targets:$(NC)"
	@echo ""
	@echo "  $(CYAN)make$(NC)              - Build all executable targets"
	@echo "  $(CYAN)make example1$(NC)     - Build the example demonstration"
	@echo "  $(CYAN)make exercise1$(NC)    - Build exercise 1"
	@echo "  $(CYAN)make exercise2$(NC)    - Build exercise 2"
	@echo ""
	@echo "  $(CYAN)make run$(NC)          - Run the example demonstration"
	@echo "  $(CYAN)make run-exercise1$(NC)- Run exercise 1 with test input"
	@echo "  $(CYAN)make run-exercise2$(NC)- Run exercise 2 with test input"
	@echo ""
	@echo "  $(CYAN)make test$(NC)         - Run all automated tests"
	@echo "  $(CYAN)make test1$(NC)        - Run test for exercise 1"
	@echo "  $(CYAN)make test2$(NC)        - Run test for exercise 2"
	@echo ""
	@echo "  $(CYAN)make valgrind$(NC)     - Check example for memory leaks"
	@echo "  $(CYAN)make valgrind-exercise1$(NC) - Check exercise 1 for memory leaks"
	@echo "  $(CYAN)make valgrind-exercise2$(NC) - Check exercise 2 for memory leaks"
	@echo ""
	@echo "  $(CYAN)make solutions$(NC)    - Build solution files (instructor only)"
	@echo "  $(CYAN)make clean$(NC)        - Remove all build artifacts"
	@echo "  $(CYAN)make info$(NC)         - Display project information"
	@echo "  $(CYAN)make help$(NC)         - Display this help message"
	@echo ""
	@echo "$(BOLD)Directory structure:$(NC)"
	@echo ""
	@echo "  $(SRC_DIR)/          - Source files"
	@echo "  $(DATA_DIR)/         - Sample data files"
	@echo "  $(TEST_DIR)/         - Test input and expected output"
	@echo "  $(SOL_DIR)/          - Solution files (instructor only)"
	@echo "  slides/        - Presentation slides"
	@echo "  teme/          - Homework assignments"
	@echo ""
