# =============================================================================
# WEEK 11: HASH TABLES - Makefile
# =============================================================================
# 
# Algorithms and Programming Techniques
# Academy of Economic Studies, Bucharest
#
# Build automation for hash table laboratory exercises
#
# =============================================================================

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O2
LDFLAGS = -lm

# Directory structure
SRC_DIR = src
DATA_DIR = data
TEST_DIR = tests
SOL_DIR = solution

# ANSI colour codes for formatted output
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
CYAN = \033[0;36m
MAGENTA = \033[0;35m
BLUE = \033[0;34m
BOLD = \033[1m
NC = \033[0m

# Executable targets
TARGETS = example1 exercise1 exercise2
SOLUTIONS = exercise1_sol exercise2_sol homework1_sol homework2_sol

# Phony targets (not files)
.PHONY: all clean run run-example run-ex1 run-ex2 test test1 test2 \
        valgrind valgrind-ex1 valgrind-ex2 solutions help info

# =============================================================================
# PRIMARY TARGETS
# =============================================================================

all: $(TARGETS)
	@echo ""
	@echo "$(GREEN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(GREEN)║$(NC)  $(BOLD)✓ All targets built successfully$(NC)                            $(GREEN)║$(NC)"
	@echo "$(GREEN)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Available executables:$(NC)"
	@echo "  ./example1   - Complete hash table demonstration"
	@echo "  ./exercise1  - Chained hash table exercise"
	@echo "  ./exercise2  - Open addressing exercise"
	@echo ""
	@echo "$(YELLOW)Run 'make help' for available commands$(NC)"

# =============================================================================
# INDIVIDUAL BUILD TARGETS
# =============================================================================

example1: $(SRC_DIR)/example1.c
	@echo "$(CYAN)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│$(NC) Building $(BOLD)example1$(NC) - Hash Table Demonstration                     $(CYAN)│$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ example1 compiled successfully$(NC)"

exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(CYAN)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│$(NC) Building $(BOLD)exercise1$(NC) - Chained Hash Table                          $(CYAN)│$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise1 compiled successfully$(NC)"

exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(CYAN)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│$(NC) Building $(BOLD)exercise2$(NC) - Open Addressing Hash Table                  $(CYAN)│$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise2 compiled successfully$(NC)"

# =============================================================================
# SOLUTION BUILDS (Instructor Only)
# =============================================================================

solutions: $(SOLUTIONS)
	@echo "$(GREEN)  ✓ All solutions built$(NC)"

exercise1_sol: $(SOL_DIR)/exercise1_sol.c
	@echo "$(MAGENTA)Building solution: exercise1_sol$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

exercise2_sol: $(SOL_DIR)/exercise2_sol.c
	@echo "$(MAGENTA)Building solution: exercise2_sol$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

homework1_sol: $(SOL_DIR)/homework1_sol.c
	@echo "$(MAGENTA)Building solution: homework1_sol$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

homework2_sol: $(SOL_DIR)/homework2_sol.c
	@echo "$(MAGENTA)Building solution: homework2_sol$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# =============================================================================
# RUN TARGETS
# =============================================================================

run: all run-example
	@echo ""

run-example: example1
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║$(NC)  $(BOLD)Running: Complete Hash Table Demonstration$(NC)                   $(YELLOW)║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./example1

run-ex1: exercise1
	@echo ""
	@echo "$(YELLOW)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(YELLOW)│$(NC)  Running: Exercise 1 - Chained Hash Table                       $(YELLOW)│$(NC)"
	@echo "$(YELLOW)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@./exercise1 < $(DATA_DIR)/students.txt

run-ex2: exercise2
	@echo ""
	@echo "$(YELLOW)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(YELLOW)│$(NC)  Running: Exercise 2 - Open Addressing                          $(YELLOW)│$(NC)"
	@echo "$(YELLOW)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@./exercise2 $(DATA_DIR)/text_sample.txt

# =============================================================================
# TEST TARGETS
# =============================================================================

test: test1 test2
	@echo ""
	@echo "$(GREEN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(GREEN)║$(NC)  $(BOLD)✓ All tests completed$(NC)                                        $(GREEN)║$(NC)"
	@echo "$(GREEN)╚═══════════════════════════════════════════════════════════════╝$(NC)"

test1: exercise1
	@echo ""
	@echo "$(YELLOW)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(YELLOW)│$(NC)  Testing Exercise 1: Chained Hash Table                         $(YELLOW)│$(NC)"
	@echo "$(YELLOW)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@./exercise1 < $(TEST_DIR)/test1_input.txt > /tmp/test1_output.txt 2>&1
	@if diff -q /tmp/test1_output.txt $(TEST_DIR)/test1_expected.txt > /dev/null 2>&1; then \
		echo "$(GREEN)  ✓ Test 1 PASSED$(NC)"; \
	else \
		echo "$(RED)  ✗ Test 1 FAILED$(NC)"; \
		echo "$(YELLOW)  Expected:$(NC)"; \
		head -10 $(TEST_DIR)/test1_expected.txt | sed 's/^/    /'; \
		echo "$(YELLOW)  Got:$(NC)"; \
		head -10 /tmp/test1_output.txt | sed 's/^/    /'; \
	fi

test2: exercise2
	@echo ""
	@echo "$(YELLOW)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(YELLOW)│$(NC)  Testing Exercise 2: Open Addressing Hash Table                 $(YELLOW)│$(NC)"
	@echo "$(YELLOW)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@./exercise2 $(TEST_DIR)/test2_input.txt > /tmp/test2_output.txt 2>&1
	@if diff -q /tmp/test2_output.txt $(TEST_DIR)/test2_expected.txt > /dev/null 2>&1; then \
		echo "$(GREEN)  ✓ Test 2 PASSED$(NC)"; \
	else \
		echo "$(RED)  ✗ Test 2 FAILED$(NC)"; \
		echo "$(YELLOW)  Expected:$(NC)"; \
		head -10 $(TEST_DIR)/test2_expected.txt | sed 's/^/    /'; \
		echo "$(YELLOW)  Got:$(NC)"; \
		head -10 /tmp/test2_output.txt | sed 's/^/    /'; \
	fi

# =============================================================================
# MEMORY CHECKING WITH VALGRIND
# =============================================================================

valgrind: valgrind-example valgrind-ex1 valgrind-ex2
	@echo ""
	@echo "$(GREEN)  ✓ Memory checking complete$(NC)"

valgrind-example: example1
	@echo ""
	@echo "$(MAGENTA)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(MAGENTA)│$(NC)  Valgrind: Checking example1 for memory leaks                   $(MAGENTA)│$(NC)"
	@echo "$(MAGENTA)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--error-exitcode=1 ./example1 2>&1 | tail -20

valgrind-ex1: exercise1
	@echo ""
	@echo "$(MAGENTA)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(MAGENTA)│$(NC)  Valgrind: Checking exercise1 for memory leaks                  $(MAGENTA)│$(NC)"
	@echo "$(MAGENTA)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--error-exitcode=1 ./exercise1 < $(DATA_DIR)/students.txt 2>&1 | tail -20

valgrind-ex2: exercise2
	@echo ""
	@echo "$(MAGENTA)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(MAGENTA)│$(NC)  Valgrind: Checking exercise2 for memory leaks                  $(MAGENTA)│$(NC)"
	@echo "$(MAGENTA)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--error-exitcode=1 ./exercise2 $(DATA_DIR)/text_sample.txt 2>&1 | tail -20

# =============================================================================
# CLEANUP
# =============================================================================

clean:
	@echo ""
	@echo "$(YELLOW)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(YELLOW)│$(NC)  Cleaning build artefacts...                                    $(YELLOW)│$(NC)"
	@echo "$(YELLOW)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@rm -f $(TARGETS) $(SOLUTIONS)
	@rm -f *.o *.dSYM
	@rm -f /tmp/test*_output.txt
	@echo "$(GREEN)  ✓ Clean complete$(NC)"
	@echo ""

# =============================================================================
# INFORMATION AND HELP
# =============================================================================

info:
	@echo ""
	@echo "$(BLUE)╔═══════════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BLUE)║$(NC)                                                                   $(BLUE)║$(NC)"
	@echo "$(BLUE)║$(NC)   $(BOLD)WEEK 11: HASH TABLES$(NC)                                           $(BLUE)║$(NC)"
	@echo "$(BLUE)║$(NC)   Algorithms and Programming Techniques                          $(BLUE)║$(NC)"
	@echo "$(BLUE)║$(NC)                                                                   $(BLUE)║$(NC)"
	@echo "$(BLUE)║$(NC)   Academy of Economic Studies, Bucharest                         $(BLUE)║$(NC)"
	@echo "$(BLUE)║$(NC)   Computer Science Faculty                                       $(BLUE)║$(NC)"
	@echo "$(BLUE)║$(NC)                                                                   $(BLUE)║$(NC)"
	@echo "$(BLUE)╚═══════════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Topics covered:$(NC)"
	@echo "  • Hash function design (djb2, FNV-1a, polynomial)"
	@echo "  • Collision resolution: chaining vs open addressing"
	@echo "  • Load factor analysis and rehashing"
	@echo "  • Memory management for dynamic hash tables"
	@echo ""

help:
	@echo ""
	@echo "$(BLUE)╔═══════════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BLUE)║$(NC)  $(BOLD)WEEK 11: HASH TABLES - Available Make Targets$(NC)                  $(BLUE)║$(NC)"
	@echo "$(BLUE)╚═══════════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(BOLD)Build Targets:$(NC)"
	@echo "  $(GREEN)make$(NC)              Build all executables"
	@echo "  $(GREEN)make example1$(NC)     Build complete demonstration"
	@echo "  $(GREEN)make exercise1$(NC)    Build chained hash table exercise"
	@echo "  $(GREEN)make exercise2$(NC)    Build open addressing exercise"
	@echo "  $(GREEN)make solutions$(NC)    Build all solutions (instructor)"
	@echo ""
	@echo "$(BOLD)Run Targets:$(NC)"
	@echo "  $(YELLOW)make run$(NC)          Run complete demonstration"
	@echo "  $(YELLOW)make run-ex1$(NC)      Run exercise 1 with sample data"
	@echo "  $(YELLOW)make run-ex2$(NC)      Run exercise 2 with sample text"
	@echo ""
	@echo "$(BOLD)Test Targets:$(NC)"
	@echo "  $(CYAN)make test$(NC)         Run all automated tests"
	@echo "  $(CYAN)make test1$(NC)        Test exercise 1 only"
	@echo "  $(CYAN)make test2$(NC)        Test exercise 2 only"
	@echo ""
	@echo "$(BOLD)Memory Checking:$(NC)"
	@echo "  $(MAGENTA)make valgrind$(NC)     Check all executables for leaks"
	@echo "  $(MAGENTA)make valgrind-ex1$(NC) Check exercise 1 for memory leaks"
	@echo "  $(MAGENTA)make valgrind-ex2$(NC) Check exercise 2 for memory leaks"
	@echo ""
	@echo "$(BOLD)Utility:$(NC)"
	@echo "  $(RED)make clean$(NC)        Remove all build artefacts"
	@echo "  $(BLUE)make info$(NC)         Display course information"
	@echo "  $(BLUE)make help$(NC)         Display this help message"
	@echo ""

# =============================================================================
# END OF MAKEFILE
# =============================================================================
