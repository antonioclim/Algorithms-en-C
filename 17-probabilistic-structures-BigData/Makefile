# =============================================================================
# WEEK 17: PROBABILISTIC DATA STRUCTURES FOR BIG DATA
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
        solutions help debug info docker benchmark

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
	@echo "$(CYAN)$(BOLD)║     WEEK 17: PROBABILISTIC DATA STRUCTURES                    ║$(NC)"
	@echo "$(CYAN)$(BOLD)║     Bloom Filter • Count-Min Sketch • HyperLogLog • Skip List ║$(NC)"
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
	@echo "$(YELLOW)▸ Running Exercise 1: Bloom Filter Spell Checker$(NC)"
	@echo "$(YELLOW)  Loading dictionary and checking words...$(NC)"
	@echo ""
	@./exercise1 $(DATA_DIR)/dictionary.txt $(DATA_DIR)/test_words.txt

run-ex2: exercise2
	@echo ""
	@echo "$(YELLOW)▸ Running Exercise 2: Network Traffic Analysis$(NC)"
	@echo "$(YELLOW)  Processing traffic log with CMS and HyperLogLog...$(NC)"
	@echo ""
	@./exercise2 $(DATA_DIR)/traffic_log.txt

# =============================================================================
# TESTING
# =============================================================================

test: all test-ex1 test-ex2
	@echo ""
	@echo "$(GREEN)$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(GREEN)$(BOLD)║     All tests completed                                       ║$(NC)"
	@echo "$(GREEN)$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"

test-ex1: exercise1
	@echo ""
	@echo "$(CYAN)▸ Testing Exercise 1: Bloom Filter Spell Checker$(NC)"
	@echo "────────────────────────────────────────────────────────────────"
	@if ./exercise1 $(TEST_DIR)/test1_input.txt $(TEST_DIR)/test1_input.txt 2>/dev/null | \
		diff -q - $(TEST_DIR)/test1_expected.txt > /dev/null 2>&1; then \
		echo "  $(GREEN)✓$(NC) Exercise 1: PASSED"; \
	else \
		echo "  $(RED)✗$(NC) Exercise 1: FAILED"; \
		echo "  $(YELLOW)Note: Probabilistic tests may have variance$(NC)"; \
	fi

test-ex2: exercise2
	@echo ""
	@echo "$(CYAN)▸ Testing Exercise 2: Traffic Analysis$(NC)"
	@echo "────────────────────────────────────────────────────────────────"
	@if ./exercise2 $(TEST_DIR)/test2_input.txt 2>/dev/null | \
		diff -q - $(TEST_DIR)/test2_expected.txt > /dev/null 2>&1; then \
		echo "  $(GREEN)✓$(NC) Exercise 2: PASSED"; \
	else \
		echo "  $(RED)✗$(NC) Exercise 2: FAILED"; \
		echo "  $(YELLOW)Note: Probabilistic tests may have variance$(NC)"; \
	fi

# =============================================================================
# MEMORY CHECKING
# =============================================================================

valgrind: valgrind-example valgrind-ex1 valgrind-ex2
	@echo ""
	@echo "$(GREEN)$(BOLD)✓ Memory checks completed$(NC)"

valgrind-example: $(EXAMPLE)
	@echo ""
	@echo "$(CYAN)▸ Valgrind: example1$(NC)"
	@echo "────────────────────────────────────────────────────────────────"
	@valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 \
		./$(EXAMPLE) 2>&1 | grep -E "(ERROR SUMMARY|definitely lost|indirectly lost|possibly lost)" || true

valgrind-ex1: exercise1
	@echo ""
	@echo "$(CYAN)▸ Valgrind: exercise1$(NC)"
	@echo "────────────────────────────────────────────────────────────────"
	@valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 \
		./exercise1 $(DATA_DIR)/dictionary.txt $(DATA_DIR)/test_words.txt 2>&1 | \
		grep -E "(ERROR SUMMARY|definitely lost|indirectly lost|possibly lost)" || true

valgrind-ex2: exercise2
	@echo ""
	@echo "$(CYAN)▸ Valgrind: exercise2$(NC)"
	@echo "────────────────────────────────────────────────────────────────"
	@valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 \
		./exercise2 $(DATA_DIR)/traffic_log.txt 2>&1 | \
		grep -E "(ERROR SUMMARY|definitely lost|indirectly lost|possibly lost)" || true

# =============================================================================
# DOCKER
# =============================================================================

docker:
	@echo ""
	@echo "$(CYAN)$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)$(BOLD)║     Building Docker Image                                     ║$(NC)"
	@echo "$(CYAN)$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@docker build -t week17-probabilistic .
	@echo ""
	@echo "$(GREEN)✓$(NC) Docker image built successfully"
	@echo ""
	@echo "$(YELLOW)▸ Running in Docker...$(NC)"
	@echo ""
	@docker run --rm week17-probabilistic

docker-interactive:
	@docker build -t week17-probabilistic .
	@docker run -it --rm week17-probabilistic /bin/bash

# =============================================================================
# BENCHMARKS
# =============================================================================

benchmark: $(EXAMPLE)
	@echo ""
	@echo "$(CYAN)$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)$(BOLD)║     Running Benchmarks                                        ║$(NC)"
	@echo "$(CYAN)$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(YELLOW)▸ Bloom Filter: varying element count...$(NC)"
	@./$(EXAMPLE) --benchmark-bloom
	@echo ""
	@echo "$(YELLOW)▸ Count-Min Sketch: varying stream size...$(NC)"
	@./$(EXAMPLE) --benchmark-cms
	@echo ""
	@echo "$(YELLOW)▸ HyperLogLog: cardinality estimation accuracy...$(NC)"
	@./$(EXAMPLE) --benchmark-hll

# =============================================================================
# CLEANUP
# =============================================================================

clean:
	@echo ""
	@echo "$(YELLOW)▸ Cleaning build artefacts...$(NC)"
	@rm -f $(EXAMPLE) $(EXERCISES) $(SOLUTIONS)
	@rm -f *.o core
	@rm -f /tmp/test*_output.txt
	@echo "$(GREEN)✓$(NC) Clean complete"
	@echo ""

distclean: clean
	@echo "$(YELLOW)▸ Deep clean...$(NC)"
	@rm -rf __pycache__ *.pyc
	@docker rmi week17-probabilistic 2>/dev/null || true
	@echo "$(GREEN)✓$(NC) Dist clean complete"

# =============================================================================
# HELP
# =============================================================================

help:
	@echo ""
	@echo "$(CYAN)$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)$(BOLD)║     WEEK 17: PROBABILISTIC DATA STRUCTURES — HELP             ║$(NC)"
	@echo "$(CYAN)$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "  $(BOLD)Build Commands:$(NC)"
	@echo "    $(YELLOW)make$(NC)              Build all executables"
	@echo "    $(YELLOW)make solutions$(NC)    Build solution files (instructor)"
	@echo "    $(YELLOW)make clean$(NC)        Remove compiled files"
	@echo ""
	@echo "  $(BOLD)Run Commands:$(NC)"
	@echo "    $(YELLOW)make run$(NC)          Run complete demonstration"
	@echo "    $(YELLOW)make run-example$(NC)  Run example programme only"
	@echo "    $(YELLOW)make run-ex1$(NC)      Run Exercise 1 (Spell Checker)"
	@echo "    $(YELLOW)make run-ex2$(NC)      Run Exercise 2 (Traffic Analysis)"
	@echo ""
	@echo "  $(BOLD)Test Commands:$(NC)"
	@echo "    $(YELLOW)make test$(NC)         Run all automated tests"
	@echo "    $(YELLOW)make test-ex1$(NC)     Test Exercise 1 only"
	@echo "    $(YELLOW)make test-ex2$(NC)     Test Exercise 2 only"
	@echo ""
	@echo "  $(BOLD)Debug Commands:$(NC)"
	@echo "    $(YELLOW)make valgrind$(NC)     Check all programmes for memory leaks"
	@echo "    $(YELLOW)make benchmark$(NC)    Run performance benchmarks"
	@echo ""
	@echo "  $(BOLD)Docker Commands:$(NC)"
	@echo "    $(YELLOW)make docker$(NC)       Build and run in Docker"
	@echo "    $(YELLOW)make docker-interactive$(NC)  Interactive Docker shell"
	@echo ""
	@echo "  $(BOLD)Data Structures Covered:$(NC)"
	@echo "    • Bloom Filter        — Membership testing with false positives"
	@echo "    • Count-Min Sketch    — Frequency estimation in streams"
	@echo "    • HyperLogLog         — Cardinality estimation"
	@echo "    • Skip List           — Probabilistic sorted structure"
	@echo ""
	@echo "  $(BOLD)Source Files:$(NC)"
	@echo "    $(SRC_DIR)/example1.c     Complete demonstration"
	@echo "    $(SRC_DIR)/exercise1.c    Spell checker exercise"
	@echo "    $(SRC_DIR)/exercise2.c    Traffic analysis exercise"
	@echo ""

# =============================================================================
# DEBUG INFO
# =============================================================================

info:
	@echo ""
	@echo "$(CYAN)Build Configuration:$(NC)"
	@echo "  CC      = $(CC)"
	@echo "  CFLAGS  = $(CFLAGS)"
	@echo "  LDFLAGS = $(LDFLAGS)"
	@echo ""
	@echo "$(CYAN)Directories:$(NC)"
	@echo "  SRC_DIR  = $(SRC_DIR)"
	@echo "  DATA_DIR = $(DATA_DIR)"
	@echo "  TEST_DIR = $(TEST_DIR)"
	@echo "  SOL_DIR  = $(SOL_DIR)"
	@echo ""
	@echo "$(CYAN)Targets:$(NC)"
	@echo "  EXAMPLE   = $(EXAMPLE)"
	@echo "  EXERCISES = $(EXERCISES)"
	@echo "  SOLUTIONS = $(SOLUTIONS)"
	@echo ""

debug: CFLAGS += -DDEBUG -g3 -O0
debug: clean all
	@echo "$(YELLOW)Debug build complete$(NC)"

# Prevent removal of intermediate files
.PRECIOUS: $(ALL_TARGETS) $(SOLUTIONS)
