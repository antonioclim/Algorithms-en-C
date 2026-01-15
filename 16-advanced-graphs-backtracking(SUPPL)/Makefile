# =============================================================================
# WEEK 16: ADVANCED GRAPH ALGORITHMS & BACKTRACKING
# Makefile for Algorithms and Programming Techniques Laboratory
# =============================================================================

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -g -O2
LDFLAGS = -lm

# Directories
SRC_DIR = src
DATA_DIR = data
TEST_DIR = tests
SOL_DIR = solution

# Colours for terminal output
GREEN  = \033[0;32m
RED    = \033[0;31m
YELLOW = \033[0;33m
CYAN   = \033[0;36m
BLUE   = \033[0;34m
PURPLE = \033[0;35m
BOLD   = \033[1m
NC     = \033[0m

# Targets
EXAMPLE = example1
EXERCISES = exercise1 exercise2
SOLUTIONS = exercise1_sol exercise2_sol homework1_sol homework2_sol
ALL_TARGETS = $(EXAMPLE) $(EXERCISES)

# Phony targets
.PHONY: all clean run run-example run-ex1 run-ex2 test test-ex1 test-ex2 \
        valgrind valgrind-example valgrind-ex1 valgrind-ex2 \
        solutions help debug info

# =============================================================================
# PRIMARY TARGETS
# =============================================================================

all: banner $(ALL_TARGETS)
	@echo ""
	@echo "$(GREEN)$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(GREEN)$(BOLD)║     ✓ All targets built successfully                          ║$(NC)"
	@echo "$(GREEN)$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""

banner:
	@echo ""
	@echo "$(CYAN)$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)$(BOLD)║     WEEK 16: ADVANCED GRAPH ALGORITHMS & BACKTRACKING         ║$(NC)"
	@echo "$(CYAN)$(BOLD)║     Building laboratory materials...                          ║$(NC)"
	@echo "$(CYAN)$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""

# =============================================================================
# BUILD TARGETS
# =============================================================================

$(EXAMPLE): $(SRC_DIR)/example1.c
	@echo "$(BLUE)▸ Building$(NC) $(BOLD)$@$(NC)$(BLUE)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "  $(GREEN)✓$(NC) $@ built successfully"

exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(BLUE)▸ Building$(NC) $(BOLD)$@$(NC)$(BLUE)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "  $(GREEN)✓$(NC) $@ built successfully"

exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(BLUE)▸ Building$(NC) $(BOLD)$@$(NC)$(BLUE)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "  $(GREEN)✓$(NC) $@ built successfully"

# =============================================================================
# SOLUTION TARGETS (Instructor only)
# =============================================================================

solutions: $(SOLUTIONS)
	@echo ""
	@echo "$(PURPLE)$(BOLD)✓ All solutions built$(NC)"

exercise1_sol: $(SOL_DIR)/exercise1_sol.c
	@echo "$(PURPLE)▸ Building solution:$(NC) $(BOLD)$@$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

exercise2_sol: $(SOL_DIR)/exercise2_sol.c
	@echo "$(PURPLE)▸ Building solution:$(NC) $(BOLD)$@$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

homework1_sol: $(SOL_DIR)/homework1_sol.c
	@echo "$(PURPLE)▸ Building solution:$(NC) $(BOLD)$@$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

homework2_sol: $(SOL_DIR)/homework2_sol.c
	@echo "$(PURPLE)▸ Building solution:$(NC) $(BOLD)$@$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# =============================================================================
# RUN TARGETS
# =============================================================================

run: all
	@echo ""
	@echo "$(YELLOW)$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)$(BOLD)║     Running Complete Demonstration                            ║$(NC)"
	@echo "$(YELLOW)$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./$(EXAMPLE)

run-example: $(EXAMPLE)
	@echo ""
	@echo "$(YELLOW)▸ Running example demonstration...$(NC)"
	@echo ""
	@./$(EXAMPLE)

run-ex1: exercise1
	@echo ""
	@echo "$(YELLOW)▸ Running Exercise 1: MST & Topological Sort$(NC)"
	@echo ""
	@if [ -f $(DATA_DIR)/graph_mst.txt ]; then \
		./exercise1 < $(DATA_DIR)/graph_mst.txt; \
	else \
		./exercise1; \
	fi

run-ex2: exercise2
	@echo ""
	@echo "$(YELLOW)▸ Running Exercise 2: Backtracking Problems$(NC)"
	@echo ""
	@if [ -f $(DATA_DIR)/sudoku_puzzles.txt ]; then \
		./exercise2 < $(DATA_DIR)/sudoku_puzzles.txt; \
	else \
		./exercise2; \
	fi

# =============================================================================
# TEST TARGETS
# =============================================================================

test: test-ex1 test-ex2
	@echo ""
	@echo "$(GREEN)$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(GREEN)$(BOLD)║     ✓ All tests completed                                     ║$(NC)"
	@echo "$(GREEN)$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"

test-ex1: exercise1
	@echo ""
	@echo "$(CYAN)$(BOLD)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)$(BOLD)  Testing Exercise 1: MST & Topological Sort                   $(NC)"
	@echo "$(CYAN)$(BOLD)═══════════════════════════════════════════════════════════════$(NC)"
	@if [ -f $(TEST_DIR)/test1_input.txt ] && [ -f $(TEST_DIR)/test1_expected.txt ]; then \
		./exercise1 < $(TEST_DIR)/test1_input.txt > /tmp/test1_output.txt 2>&1; \
		if diff -q /tmp/test1_output.txt $(TEST_DIR)/test1_expected.txt > /dev/null 2>&1; then \
			echo "$(GREEN)  ✓ Test 1 PASSED$(NC)"; \
		else \
			echo "$(RED)  ✗ Test 1 FAILED$(NC)"; \
			echo "$(YELLOW)  Expected:$(NC)"; \
			cat $(TEST_DIR)/test1_expected.txt | head -10; \
			echo "$(YELLOW)  Got:$(NC)"; \
			cat /tmp/test1_output.txt | head -10; \
		fi; \
	else \
		echo "$(YELLOW)  ⚠ Test files not found, running basic check...$(NC)"; \
		./exercise1 && echo "$(GREEN)  ✓ Compilation and basic execution OK$(NC)"; \
	fi

test-ex2: exercise2
	@echo ""
	@echo "$(CYAN)$(BOLD)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)$(BOLD)  Testing Exercise 2: Backtracking Problems                    $(NC)"
	@echo "$(CYAN)$(BOLD)═══════════════════════════════════════════════════════════════$(NC)"
	@if [ -f $(TEST_DIR)/test2_input.txt ] && [ -f $(TEST_DIR)/test2_expected.txt ]; then \
		./exercise2 < $(TEST_DIR)/test2_input.txt > /tmp/test2_output.txt 2>&1; \
		if diff -q /tmp/test2_output.txt $(TEST_DIR)/test2_expected.txt > /dev/null 2>&1; then \
			echo "$(GREEN)  ✓ Test 2 PASSED$(NC)"; \
		else \
			echo "$(RED)  ✗ Test 2 FAILED$(NC)"; \
			echo "$(YELLOW)  Expected:$(NC)"; \
			cat $(TEST_DIR)/test2_expected.txt | head -10; \
			echo "$(YELLOW)  Got:$(NC)"; \
			cat /tmp/test2_output.txt | head -10; \
		fi; \
	else \
		echo "$(YELLOW)  ⚠ Test files not found, running basic check...$(NC)"; \
		./exercise2 && echo "$(GREEN)  ✓ Compilation and basic execution OK$(NC)"; \
	fi

# =============================================================================
# MEMORY CHECK TARGETS
# =============================================================================

valgrind: valgrind-example
	@echo ""
	@echo "$(GREEN)$(BOLD)✓ Memory check completed$(NC)"

valgrind-example: $(EXAMPLE)
	@echo ""
	@echo "$(PURPLE)$(BOLD)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(PURPLE)$(BOLD)  Valgrind Memory Check: example1                              $(NC)"
	@echo "$(PURPLE)$(BOLD)═══════════════════════════════════════════════════════════════$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--error-exitcode=1 ./$(EXAMPLE) 2>&1 | tail -20

valgrind-ex1: exercise1
	@echo ""
	@echo "$(PURPLE)$(BOLD)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(PURPLE)$(BOLD)  Valgrind Memory Check: exercise1                             $(NC)"
	@echo "$(PURPLE)$(BOLD)═══════════════════════════════════════════════════════════════$(NC)"
	@if [ -f $(DATA_DIR)/graph_mst.txt ]; then \
		valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
			./exercise1 < $(DATA_DIR)/graph_mst.txt 2>&1 | tail -20; \
	else \
		valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
			./exercise1 2>&1 | tail -20; \
	fi

valgrind-ex2: exercise2
	@echo ""
	@echo "$(PURPLE)$(BOLD)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(PURPLE)$(BOLD)  Valgrind Memory Check: exercise2                             $(NC)"
	@echo "$(PURPLE)$(BOLD)═══════════════════════════════════════════════════════════════$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		./exercise2 2>&1 | tail -20

# =============================================================================
# UTILITY TARGETS
# =============================================================================

clean:
	@echo "$(YELLOW)Cleaning build artifacts...$(NC)"
	@rm -f $(EXAMPLE) $(EXERCISES) $(SOLUTIONS)
	@rm -f *.o *.out
	@rm -f /tmp/test*_output.txt
	@echo "$(GREEN)✓ Clean complete!$(NC)"

info:
	@echo ""
	@echo "$(CYAN)$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)$(BOLD)║     WEEK 16: ADVANCED GRAPH ALGORITHMS & BACKTRACKING         ║$(NC)"
	@echo "$(CYAN)$(BOLD)╠═══════════════════════════════════════════════════════════════╣$(NC)"
	@echo "$(CYAN)$(BOLD)║  Topics covered:                                              ║$(NC)"
	@echo "$(CYAN)$(BOLD)║    • Minimum Spanning Trees (Kruskal, Prim)                   ║$(NC)"
	@echo "$(CYAN)$(BOLD)║    • Union-Find Data Structure                                ║$(NC)"
	@echo "$(CYAN)$(BOLD)║    • Topological Sort (DFS, Kahn's)                           ║$(NC)"
	@echo "$(CYAN)$(BOLD)║    • Strongly Connected Components                            ║$(NC)"
	@echo "$(CYAN)$(BOLD)║    • Backtracking (N-Queens, Sudoku, Graph Coloring)          ║$(NC)"
	@echo "$(CYAN)$(BOLD)║                                                               ║$(NC)"
	@echo "$(CYAN)$(BOLD)║  Files:                                                       ║$(NC)"
	@echo "$(CYAN)$(BOLD)║    • example1.c  - Complete demonstration                     ║$(NC)"
	@echo "$(CYAN)$(BOLD)║    • exercise1.c - MST & Topological Sort                     ║$(NC)"
	@echo "$(CYAN)$(BOLD)║    • exercise2.c - Backtracking problems                      ║$(NC)"
	@echo "$(CYAN)$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""

debug: CFLAGS += -DDEBUG -fsanitize=address -fsanitize=undefined
debug: clean all
	@echo ""
	@echo "$(YELLOW)$(BOLD)⚠ Built with debug flags (AddressSanitizer enabled)$(NC)"

help:
	@echo ""
	@echo "$(CYAN)$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)$(BOLD)║     WEEK 16 MAKEFILE HELP                                     ║$(NC)"
	@echo "$(CYAN)$(BOLD)╠═══════════════════════════════════════════════════════════════╣$(NC)"
	@echo "$(CYAN)$(BOLD)║  Build targets:                                               ║$(NC)"
	@echo "$(CYAN)$(BOLD)║    make          - Build all programs                         ║$(NC)"
	@echo "$(CYAN)$(BOLD)║    make solutions- Build instructor solutions                 ║$(NC)"
	@echo "$(CYAN)$(BOLD)║    make debug    - Build with sanitizers                      ║$(NC)"
	@echo "$(CYAN)$(BOLD)║                                                               ║$(NC)"
	@echo "$(CYAN)$(BOLD)║  Run targets:                                                 ║$(NC)"
	@echo "$(CYAN)$(BOLD)║    make run      - Run complete demonstration                 ║$(NC)"
	@echo "$(CYAN)$(BOLD)║    make run-ex1  - Run exercise 1 (MST)                       ║$(NC)"
	@echo "$(CYAN)$(BOLD)║    make run-ex2  - Run exercise 2 (Backtracking)              ║$(NC)"
	@echo "$(CYAN)$(BOLD)║                                                               ║$(NC)"
	@echo "$(CYAN)$(BOLD)║  Test targets:                                                ║$(NC)"
	@echo "$(CYAN)$(BOLD)║    make test     - Run all tests                              ║$(NC)"
	@echo "$(CYAN)$(BOLD)║    make test-ex1 - Test exercise 1                            ║$(NC)"
	@echo "$(CYAN)$(BOLD)║    make test-ex2 - Test exercise 2                            ║$(NC)"
	@echo "$(CYAN)$(BOLD)║                                                               ║$(NC)"
	@echo "$(CYAN)$(BOLD)║  Memory check:                                                ║$(NC)"
	@echo "$(CYAN)$(BOLD)║    make valgrind - Check for memory leaks                     ║$(NC)"
	@echo "$(CYAN)$(BOLD)║                                                               ║$(NC)"
	@echo "$(CYAN)$(BOLD)║  Utility:                                                     ║$(NC)"
	@echo "$(CYAN)$(BOLD)║    make clean    - Remove build artifacts                     ║$(NC)"
	@echo "$(CYAN)$(BOLD)║    make info     - Display week information                   ║$(NC)"
	@echo "$(CYAN)$(BOLD)║    make help     - Show this help message                     ║$(NC)"
	@echo "$(CYAN)$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""

# =============================================================================
# ADDITIONAL CONVENIENCE TARGETS
# =============================================================================

# Run all solutions for verification
run-solutions: solutions
	@echo ""
	@echo "$(PURPLE)$(BOLD)Running all solutions...$(NC)"
	@echo ""
	@echo "$(PURPLE)─── Exercise 1 Solution ───$(NC)"
	@./exercise1_sol
	@echo ""
	@echo "$(PURPLE)─── Exercise 2 Solution ───$(NC)"
	@./exercise2_sol

# Quick test of example
quick: $(EXAMPLE)
	@./$(EXAMPLE)

# Count lines of code
loc:
	@echo ""
	@echo "$(CYAN)Lines of code:$(NC)"
	@wc -l $(SRC_DIR)/*.c $(SOL_DIR)/*.c 2>/dev/null || wc -l $(SRC_DIR)/*.c
	@echo ""

# Create submission archive
submit:
	@echo "$(YELLOW)Creating submission archive...$(NC)"
	@tar -czvf week16_submission.tar.gz $(SRC_DIR)/exercise1.c $(SRC_DIR)/exercise2.c
	@echo "$(GREEN)✓ Created week16_submission.tar.gz$(NC)"
