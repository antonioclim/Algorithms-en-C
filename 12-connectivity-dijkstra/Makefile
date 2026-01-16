# ==============================================================================
# WEEK 12: GRAPH FUNDAMENTALS - MAKEFILE
# ==============================================================================
# 
# Targets:
#   all       - Build all executables
#   clean     - Remove build artifacts
#   run       - Run the example demonstration
#   test      - Run automated tests
#   valgrind  - Check for memory leaks
#   help      - Display this help message
#
# Usage:
#   make              # Build all
#   make run          # Run demonstration
#   make test         # Run tests
#   make valgrind     # Memory check
#   make clean        # Clean up
#
# ==============================================================================

# Compiler settings
CC       := gcc
CFLAGS   := -std=c11 -Wall -Wextra -Werror -pedantic -g
LDFLAGS  := 

# Directories
SRC_DIR  := src
DATA_DIR := data
TEST_DIR := tests
SOL_DIR  := solution
OUT_DIR  := build

# Source files
EXAMPLE   := $(SRC_DIR)/example1.c
EXERCISE1 := $(SRC_DIR)/exercise1.c
EXERCISE2 := $(SRC_DIR)/exercise2.c

# Executables
EXAMPLE_BIN   := $(OUT_DIR)/example1
EXERCISE1_BIN := $(OUT_DIR)/exercise1
EXERCISE2_BIN := $(OUT_DIR)/exercise2

# Colour definitions
COLOUR_RESET  := \033[0m
COLOUR_GREEN  := \033[1;32m
COLOUR_YELLOW := \033[1;33m
COLOUR_BLUE   := \033[1;34m
COLOUR_RED    := \033[1;31m
COLOUR_CYAN   := \033[1;36m
COLOUR_MAGENTA := \033[1;35m

# Box-drawing characters for pretty output
BOX_TL := ╔
BOX_TR := ╗
BOX_BL := ╚
BOX_BR := ╝
BOX_H  := ═
BOX_V  := ║

# ==============================================================================
# PHONY TARGETS
# ==============================================================================
.PHONY: all clean run run-ex1 run-ex2 test test1 test2 valgrind valgrind-ex1 \
        valgrind-ex2 help demo-all solutions check-solutions

# ==============================================================================
# DEFAULT TARGET
# ==============================================================================

all: $(OUT_DIR) $(EXAMPLE_BIN) $(EXERCISE1_BIN) $(EXERCISE2_BIN)
	@echo ""
	@echo "$(COLOUR_GREEN)$(BOX_TL)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_TR)$(COLOUR_RESET)"
	@echo "$(COLOUR_GREEN)$(BOX_V)$(COLOUR_RESET)  $(COLOUR_CYAN)BUILD SUCCESSFUL$(COLOUR_RESET)                                  $(COLOUR_GREEN)$(BOX_V)$(COLOUR_RESET)"
	@echo "$(COLOUR_GREEN)$(BOX_V)$(COLOUR_RESET)  Week 12: Graph Fundamentals                       $(COLOUR_GREEN)$(BOX_V)$(COLOUR_RESET)"
	@echo "$(COLOUR_GREEN)$(BOX_BL)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_BR)$(COLOUR_RESET)"
	@echo ""
	@echo "$(COLOUR_YELLOW)Executables:$(COLOUR_RESET)"
	@echo "  $(COLOUR_BLUE)→$(COLOUR_RESET) $(EXAMPLE_BIN)"
	@echo "  $(COLOUR_BLUE)→$(COLOUR_RESET) $(EXERCISE1_BIN)"
	@echo "  $(COLOUR_BLUE)→$(COLOUR_RESET) $(EXERCISE2_BIN)"
	@echo ""
	@echo "$(COLOUR_YELLOW)Next steps:$(COLOUR_RESET)"
	@echo "  $(COLOUR_MAGENTA)make run$(COLOUR_RESET)      - Run example demonstration"
	@echo "  $(COLOUR_MAGENTA)make test$(COLOUR_RESET)     - Run automated tests"
	@echo "  $(COLOUR_MAGENTA)make valgrind$(COLOUR_RESET) - Check for memory leaks"
	@echo ""

# ==============================================================================
# BUILD DIRECTORY
# ==============================================================================

$(OUT_DIR):
	@mkdir -p $(OUT_DIR)
	@echo "$(COLOUR_BLUE)[MKDIR]$(COLOUR_RESET) Created build directory"

# ==============================================================================
# COMPILATION RULES
# ==============================================================================

$(EXAMPLE_BIN): $(EXAMPLE) | $(OUT_DIR)
	@echo "$(COLOUR_BLUE)[CC]$(COLOUR_RESET) Compiling example1.c..."
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(COLOUR_GREEN)[OK]$(COLOUR_RESET) Built $@"

$(EXERCISE1_BIN): $(EXERCISE1) | $(OUT_DIR)
	@echo "$(COLOUR_BLUE)[CC]$(COLOUR_RESET) Compiling exercise1.c..."
	@$(CC) -std=c11 -Wall -Wextra -g -o $@ $< $(LDFLAGS)
	@echo "$(COLOUR_GREEN)[OK]$(COLOUR_RESET) Built $@"

$(EXERCISE2_BIN): $(EXERCISE2) | $(OUT_DIR)
	@echo "$(COLOUR_BLUE)[CC]$(COLOUR_RESET) Compiling exercise2.c..."
	@$(CC) -std=c11 -Wall -Wextra -g -o $@ $< $(LDFLAGS)
	@echo "$(COLOUR_GREEN)[OK]$(COLOUR_RESET) Built $@"

# ==============================================================================
# RUN TARGETS
# ==============================================================================

run: $(EXAMPLE_BIN)
	@echo ""
	@echo "$(COLOUR_CYAN)$(BOX_TL)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_TR)$(COLOUR_RESET)"
	@echo "$(COLOUR_CYAN)$(BOX_V)$(COLOUR_RESET)  RUNNING: Graph Fundamentals Example               $(COLOUR_CYAN)$(BOX_V)$(COLOUR_RESET)"
	@echo "$(COLOUR_CYAN)$(BOX_BL)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_BR)$(COLOUR_RESET)"
	@echo ""
	@./$(EXAMPLE_BIN)

run-ex1: $(EXERCISE1_BIN)
	@echo ""
	@echo "$(COLOUR_YELLOW)[RUN]$(COLOUR_RESET) Running Exercise 1 with small graph..."
	@echo ""
	@./$(EXERCISE1_BIN) < $(TEST_DIR)/test1_input.txt

run-ex2: $(EXERCISE2_BIN)
	@echo ""
	@echo "$(COLOUR_YELLOW)[RUN]$(COLOUR_RESET) Running Exercise 2 with small graph..."
	@echo ""
	@./$(EXERCISE2_BIN) < $(TEST_DIR)/test2_input.txt

demo-all: $(EXAMPLE_BIN) $(EXERCISE1_BIN) $(EXERCISE2_BIN)
	@echo ""
	@echo "$(COLOUR_MAGENTA)══════════════════════════════════════════════════════════$(COLOUR_RESET)"
	@echo "$(COLOUR_MAGENTA)  WEEK 12 DEMONSTRATION: ALL PROGRAMS$(COLOUR_RESET)"
	@echo "$(COLOUR_MAGENTA)══════════════════════════════════════════════════════════$(COLOUR_RESET)"
	@echo ""
	@echo "$(COLOUR_CYAN)[1/3]$(COLOUR_RESET) Example Program"
	@echo "$(COLOUR_CYAN)──────────────────────────────────────────────────────────$(COLOUR_RESET)"
	@./$(EXAMPLE_BIN)
	@echo ""
	@echo "$(COLOUR_CYAN)[2/3]$(COLOUR_RESET) Exercise 1: BFS"
	@echo "$(COLOUR_CYAN)──────────────────────────────────────────────────────────$(COLOUR_RESET)"
	@./$(EXERCISE1_BIN) < $(TEST_DIR)/test1_input.txt || true
	@echo ""
	@echo "$(COLOUR_CYAN)[3/3]$(COLOUR_RESET) Exercise 2: DFS"
	@echo "$(COLOUR_CYAN)──────────────────────────────────────────────────────────$(COLOUR_RESET)"
	@./$(EXERCISE2_BIN) < $(TEST_DIR)/test2_input.txt || true
	@echo ""
	@echo "$(COLOUR_GREEN)══════════════════════════════════════════════════════════$(COLOUR_RESET)"
	@echo "$(COLOUR_GREEN)  DEMONSTRATION COMPLETE$(COLOUR_RESET)"
	@echo "$(COLOUR_GREEN)══════════════════════════════════════════════════════════$(COLOUR_RESET)"

# ==============================================================================
# TEST TARGETS
# ==============================================================================

test: test1 test2
	@echo ""
	@echo "$(COLOUR_GREEN)$(BOX_TL)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_TR)$(COLOUR_RESET)"
	@echo "$(COLOUR_GREEN)$(BOX_V)$(COLOUR_RESET)  ALL TESTS EXECUTED                                $(COLOUR_GREEN)$(BOX_V)$(COLOUR_RESET)"
	@echo "$(COLOUR_GREEN)$(BOX_BL)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_BR)$(COLOUR_RESET)"

test1: $(EXERCISE1_BIN)
	@echo ""
	@echo "$(COLOUR_YELLOW)[TEST 1]$(COLOUR_RESET) BFS Shortest Paths"
	@echo "$(COLOUR_BLUE)────────────────────────────────────────$(COLOUR_RESET)"
	@./$(EXERCISE1_BIN) < $(TEST_DIR)/test1_input.txt > $(OUT_DIR)/test1_actual.txt 2> $(OUT_DIR)/test1_stderr.txt
	@if diff -q $(TEST_DIR)/test1_expected.txt $(OUT_DIR)/test1_actual.txt >/dev/null; then \
		echo "$(COLOUR_GREEN)[PASS]$(COLOUR_RESET) Output matches $(TEST_DIR)/test1_expected.txt"; \
	else \
		echo "$(COLOUR_RED)[FAIL]$(COLOUR_RESET) Output mismatch for Exercise 1"; \
		echo "$(COLOUR_CYAN)Diff (expected vs actual):$(COLOUR_RESET)"; \
		diff -u $(TEST_DIR)/test1_expected.txt $(OUT_DIR)/test1_actual.txt || true; \
		exit 1; \
	fi

test2: $(EXERCISE2_BIN)
	@echo ""
	@echo "$(COLOUR_YELLOW)[TEST 2]$(COLOUR_RESET) DFS Topological Sort"
	@echo "$(COLOUR_BLUE)────────────────────────────────────────$(COLOUR_RESET)"
	@./$(EXERCISE2_BIN) < $(TEST_DIR)/test2_input.txt > $(OUT_DIR)/test2_actual.txt 2> $(OUT_DIR)/test2_stderr.txt
	@if diff -q $(TEST_DIR)/test2_expected.txt $(OUT_DIR)/test2_actual.txt >/dev/null; then \
		echo "$(COLOUR_GREEN)[PASS]$(COLOUR_RESET) Output matches $(TEST_DIR)/test2_expected.txt"; \
	else \
		echo "$(COLOUR_RED)[FAIL]$(COLOUR_RESET) Output mismatch for Exercise 2"; \
		echo "$(COLOUR_CYAN)Diff (expected vs actual):$(COLOUR_RESET)"; \
		diff -u $(TEST_DIR)/test2_expected.txt $(OUT_DIR)/test2_actual.txt || true; \
		exit 1; \
	fi

# ==============================================================================
# VALGRIND MEMORY CHECK
# ==============================================================================

valgrind: valgrind-example valgrind-ex1 valgrind-ex2
	@echo ""
	@echo "$(COLOUR_GREEN)[VALGRIND]$(COLOUR_RESET) All memory checks complete"

valgrind-example: $(EXAMPLE_BIN)
	@echo ""
	@echo "$(COLOUR_MAGENTA)[VALGRIND]$(COLOUR_RESET) Checking example1 for memory leaks..."
	@echo "$(COLOUR_BLUE)────────────────────────────────────────$(COLOUR_RESET)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--error-exitcode=1 ./$(EXAMPLE_BIN) 2>&1 | tail -20 || \
		echo "$(COLOUR_RED)[LEAK]$(COLOUR_RESET) Memory issues detected"
	@echo ""

valgrind-ex1: $(EXERCISE1_BIN)
	@echo ""
	@echo "$(COLOUR_MAGENTA)[VALGRIND]$(COLOUR_RESET) Checking exercise1 for memory leaks..."
	@echo "$(COLOUR_BLUE)────────────────────────────────────────$(COLOUR_RESET)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		./$(EXERCISE1_BIN) < $(TEST_DIR)/test1_input.txt 2>&1 | tail -20 || \
		echo "$(COLOUR_YELLOW)[INFO]$(COLOUR_RESET) Complete TODOs and re-run valgrind"
	@echo ""

valgrind-ex2: $(EXERCISE2_BIN)
	@echo ""
	@echo "$(COLOUR_MAGENTA)[VALGRIND]$(COLOUR_RESET) Checking exercise2 for memory leaks..."
	@echo "$(COLOUR_BLUE)────────────────────────────────────────$(COLOUR_RESET)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		./$(EXERCISE2_BIN) < $(TEST_DIR)/test2_input.txt 2>&1 | tail -20 || \
		echo "$(COLOUR_YELLOW)[INFO]$(COLOUR_RESET) Complete TODOs and re-run valgrind"
	@echo ""

# ==============================================================================
# SOLUTION BUILDING
# ==============================================================================

solutions: $(OUT_DIR)
	@echo ""
	@echo "$(COLOUR_MAGENTA)[BUILD]$(COLOUR_RESET) Compiling solution files..."
	@$(CC) $(CFLAGS) -o $(OUT_DIR)/exercise1_sol $(SOL_DIR)/exercise1_sol.c $(LDFLAGS) && \
		echo "$(COLOUR_GREEN)[OK]$(COLOUR_RESET) exercise1_sol" || \
		echo "$(COLOUR_RED)[FAIL]$(COLOUR_RESET) exercise1_sol"
	@$(CC) $(CFLAGS) -o $(OUT_DIR)/exercise2_sol $(SOL_DIR)/exercise2_sol.c $(LDFLAGS) && \
		echo "$(COLOUR_GREEN)[OK]$(COLOUR_RESET) exercise2_sol" || \
		echo "$(COLOUR_RED)[FAIL]$(COLOUR_RESET) exercise2_sol"
	@$(CC) $(CFLAGS) -o $(OUT_DIR)/homework1_sol $(SOL_DIR)/homework1_sol.c $(LDFLAGS) && \
		echo "$(COLOUR_GREEN)[OK]$(COLOUR_RESET) homework1_sol" || \
		echo "$(COLOUR_RED)[FAIL]$(COLOUR_RESET) homework1_sol"
	@$(CC) $(CFLAGS) -o $(OUT_DIR)/homework2_sol $(SOL_DIR)/homework2_sol.c $(LDFLAGS) && \
		echo "$(COLOUR_GREEN)[OK]$(COLOUR_RESET) homework2_sol" || \
		echo "$(COLOUR_RED)[FAIL]$(COLOUR_RESET) homework2_sol"
	@echo ""
	@echo "$(COLOUR_GREEN)[DONE]$(COLOUR_RESET) All solutions compiled"

check-solutions: solutions
	@echo ""
	@echo "$(COLOUR_CYAN)[CHECK]$(COLOUR_RESET) Running solution tests..."
	@echo ""
	@echo "Exercise 1 Solution:"
	@./$(OUT_DIR)/exercise1_sol < $(DATA_DIR)/graph_small.txt 2>&1 | head -30
	@echo ""
	@echo "Exercise 2 Solution:"
	@./$(OUT_DIR)/exercise2_sol < $(DATA_DIR)/graph_small.txt 2>&1 | head -30

# ==============================================================================
# CLEAN TARGET
# ==============================================================================

clean:
	@echo ""
	@echo "$(COLOUR_YELLOW)[CLEAN]$(COLOUR_RESET) Removing build artifacts..."
	@rm -rf $(OUT_DIR)
	@rm -f *.o
	@echo "$(COLOUR_GREEN)[OK]$(COLOUR_RESET) Clean complete"
	@echo ""

# ==============================================================================
# HELP TARGET
# ==============================================================================

help:
	@echo ""
	@echo "$(COLOUR_CYAN)$(BOX_TL)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_TR)$(COLOUR_RESET)"
	@echo "$(COLOUR_CYAN)$(BOX_V)$(COLOUR_RESET)  WEEK 12: GRAPH FUNDAMENTALS                       $(COLOUR_CYAN)$(BOX_V)$(COLOUR_RESET)"
	@echo "$(COLOUR_CYAN)$(BOX_V)$(COLOUR_RESET)  Available Makefile Targets                        $(COLOUR_CYAN)$(BOX_V)$(COLOUR_RESET)"
	@echo "$(COLOUR_CYAN)$(BOX_BL)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_H)$(BOX_BR)$(COLOUR_RESET)"
	@echo ""
	@echo "$(COLOUR_YELLOW)Build Targets:$(COLOUR_RESET)"
	@echo "  $(COLOUR_GREEN)make$(COLOUR_RESET)              Build all executables"
	@echo "  $(COLOUR_GREEN)make clean$(COLOUR_RESET)        Remove build artifacts"
	@echo "  $(COLOUR_GREEN)make solutions$(COLOUR_RESET)    Build solution files"
	@echo ""
	@echo "$(COLOUR_YELLOW)Run Targets:$(COLOUR_RESET)"
	@echo "  $(COLOUR_GREEN)make run$(COLOUR_RESET)          Run example demonstration"
	@echo "  $(COLOUR_GREEN)make run-ex1$(COLOUR_RESET)      Run exercise 1 (BFS)"
	@echo "  $(COLOUR_GREEN)make run-ex2$(COLOUR_RESET)      Run exercise 2 (DFS)"
	@echo "  $(COLOUR_GREEN)make demo-all$(COLOUR_RESET)     Run all programs"
	@echo ""
	@echo "$(COLOUR_YELLOW)Test Targets:$(COLOUR_RESET)"
	@echo "  $(COLOUR_GREEN)make test$(COLOUR_RESET)         Run all tests"
	@echo "  $(COLOUR_GREEN)make test1$(COLOUR_RESET)        Run BFS test"
	@echo "  $(COLOUR_GREEN)make test2$(COLOUR_RESET)        Run DFS test"
	@echo ""
	@echo "$(COLOUR_YELLOW)Debug Targets:$(COLOUR_RESET)"
	@echo "  $(COLOUR_GREEN)make valgrind$(COLOUR_RESET)     Check all programs for leaks"
	@echo "  $(COLOUR_GREEN)make valgrind-ex1$(COLOUR_RESET) Check exercise 1"
	@echo "  $(COLOUR_GREEN)make valgrind-ex2$(COLOUR_RESET) Check exercise 2"
	@echo ""
	@echo "$(COLOUR_YELLOW)Workflow:$(COLOUR_RESET)"
	@echo "  1. $(COLOUR_BLUE)make$(COLOUR_RESET)            - Build all programs"
	@echo "  2. $(COLOUR_BLUE)make run$(COLOUR_RESET)        - See example output"
	@echo "  3. Edit exercise files, complete TODOs"
	@echo "  4. $(COLOUR_BLUE)make test$(COLOUR_RESET)       - Verify your solutions"
	@echo "  5. $(COLOUR_BLUE)make valgrind$(COLOUR_RESET)   - Check memory management"
	@echo ""
