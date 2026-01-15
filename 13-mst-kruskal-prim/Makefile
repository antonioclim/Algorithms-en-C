# =============================================================================
# WEEK 13: GRAPH ALGORITHMS - SHORTEST PATH
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
	@echo "$(CYAN)$(BOLD)║     WEEK 13: GRAPH ALGORITHMS - SHORTEST PATH                 ║$(NC)"
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
	@echo "$(YELLOW)▸ Running Exercise 1: Dijkstra's Algorithm$(NC)"
	@echo ""
	@if [ -f $(DATA_DIR)/graph1.txt ]; then \
		./exercise1 < $(DATA_DIR)/graph1.txt; \
	else \
		./exercise1; \
	fi

run-ex2: exercise2
	@echo ""
	@echo "$(YELLOW)▸ Running Exercise 2: Bellman-Ford Algorithm$(NC)"
	@echo ""
	@if [ -f $(DATA_DIR)/currencies.txt ]; then \
		./exercise2 < $(DATA_DIR)/currencies.txt; \
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
	@echo "$(CYAN)$(BOLD)  Testing Exercise 1: Dijkstra's Algorithm                     $(NC)"
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
	@echo "$(CYAN)$(BOLD)  Testing Exercise 2: Bellman-Ford Algorithm                   $(NC)"
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
	@echo "$(PURPLE)$(BOLD)  Valgrind Memory Check: Example                              $(NC)"
	@echo "$(PURPLE)$(BOLD)═══════════════════════════════════════════════════════════════$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--error-exitcode=1 ./$(EXAMPLE) 2>&1 | head -50

valgrind-ex1: exercise1
	@echo ""
	@echo "$(PURPLE)▸ Valgrind check for Exercise 1...$(NC)"
	@if [ -f $(DATA_DIR)/graph1.txt ]; then \
		valgrind --leak-check=full --show-leak-kinds=all \
			./exercise1 < $(DATA_DIR)/graph1.txt 2>&1 | tail -20; \
	else \
		valgrind --leak-check=full ./exercise1 2>&1 | tail -20; \
	fi

valgrind-ex2: exercise2
	@echo ""
	@echo "$(PURPLE)▸ Valgrind check for Exercise 2...$(NC)"
	@if [ -f $(DATA_DIR)/currencies.txt ]; then \
		valgrind --leak-check=full --show-leak-kinds=all \
			./exercise2 < $(DATA_DIR)/currencies.txt 2>&1 | tail -20; \
	else \
		valgrind --leak-check=full ./exercise2 2>&1 | tail -20; \
	fi

# =============================================================================
# DEBUG TARGET
# =============================================================================

debug: CFLAGS += -DDEBUG -g3 -O0
debug: clean all
	@echo ""
	@echo "$(PURPLE)$(BOLD)✓ Debug build complete (with DEBUG flag and symbols)$(NC)"

# =============================================================================
# UTILITY TARGETS
# =============================================================================

clean:
	@echo ""
	@echo "$(YELLOW)▸ Cleaning build artifacts...$(NC)"
	@rm -f $(ALL_TARGETS) $(SOLUTIONS)
	@rm -f *.o *.out core
	@rm -f /tmp/test*_output.txt
	@echo "$(GREEN)  ✓ Clean complete$(NC)"
	@echo ""

info:
	@echo ""
	@echo "$(CYAN)$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)$(BOLD)║     PROJECT INFORMATION                                       ║$(NC)"
	@echo "$(CYAN)$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "  $(BOLD)Course:$(NC)      Algorithms and Programming Techniques"
	@echo "  $(BOLD)Week:$(NC)        13 - Graph Algorithms (Shortest Path)"
	@echo "  $(BOLD)Compiler:$(NC)    $(CC)"
	@echo "  $(BOLD)Flags:$(NC)       $(CFLAGS)"
	@echo ""
	@echo "  $(BOLD)Source files:$(NC)"
	@ls -la $(SRC_DIR)/*.c 2>/dev/null || echo "    (none found)"
	@echo ""
	@echo "  $(BOLD)Data files:$(NC)"
	@ls -la $(DATA_DIR)/* 2>/dev/null || echo "    (none found)"
	@echo ""

help:
	@echo ""
	@echo "$(CYAN)$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)$(BOLD)║     WEEK 13: GRAPH ALGORITHMS - AVAILABLE TARGETS             ║$(NC)"
	@echo "$(CYAN)$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "  $(BOLD)Build Targets:$(NC)"
	@echo "    $(GREEN)make$(NC)              Build all student targets"
	@echo "    $(GREEN)make all$(NC)          Same as 'make'"
	@echo "    $(GREEN)make example1$(NC)     Build complete demonstration"
	@echo "    $(GREEN)make exercise1$(NC)    Build Dijkstra exercise"
	@echo "    $(GREEN)make exercise2$(NC)    Build Bellman-Ford exercise"
	@echo "    $(GREEN)make solutions$(NC)    Build all solutions (instructor)"
	@echo "    $(GREEN)make debug$(NC)        Build with debug symbols"
	@echo ""
	@echo "  $(BOLD)Run Targets:$(NC)"
	@echo "    $(YELLOW)make run$(NC)          Run complete demonstration"
	@echo "    $(YELLOW)make run-example$(NC)  Run example only"
	@echo "    $(YELLOW)make run-ex1$(NC)      Run Exercise 1"
	@echo "    $(YELLOW)make run-ex2$(NC)      Run Exercise 2"
	@echo ""
	@echo "  $(BOLD)Test Targets:$(NC)"
	@echo "    $(CYAN)make test$(NC)         Run all automated tests"
	@echo "    $(CYAN)make test-ex1$(NC)     Test Exercise 1 only"
	@echo "    $(CYAN)make test-ex2$(NC)     Test Exercise 2 only"
	@echo ""
	@echo "  $(BOLD)Memory Check:$(NC)"
	@echo "    $(PURPLE)make valgrind$(NC)     Check example for memory leaks"
	@echo "    $(PURPLE)make valgrind-ex1$(NC) Check Exercise 1 for leaks"
	@echo "    $(PURPLE)make valgrind-ex2$(NC) Check Exercise 2 for leaks"
	@echo ""
	@echo "  $(BOLD)Utility:$(NC)"
	@echo "    $(RED)make clean$(NC)        Remove all build artifacts"
	@echo "    $(BLUE)make info$(NC)         Display project information"
	@echo "    $(BLUE)make help$(NC)         Display this help message"
	@echo ""

# =============================================================================
# END OF MAKEFILE
# =============================================================================
