# ==============================================================================
# MAKEFILE - Week 01: Pointers to Functions and Callbacks
# ATP Course - ASE-CSIE
# ==============================================================================
#
# Usage:
#   make              - Compile all sources
#   make run          - Run all executables
#   make test         - Run automated tests
#   make valgrind     - Check for memory leaks
#   make clean        - Remove compiled files
#   make help         - Show this help message
#
# ==============================================================================

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O0
LDFLAGS = -lm

# Directories
SRC_DIR = src
TEST_DIR = tests
DATA_DIR = data
SOL_DIR = solution

# Source files and executables
SOURCES = $(wildcard $(SRC_DIR)/*.c)
EXECUTABLES = $(patsubst $(SRC_DIR)/%.c,%,$(SOURCES))

# Solution files (compiled separately)
SOL_SOURCES = $(wildcard $(SOL_DIR)/*.c)
SOL_EXECUTABLES = $(patsubst $(SOL_DIR)/%.c,$(SOL_DIR)/%,$(SOL_SOURCES))

# Colors for terminal output
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
BLUE = \033[0;34m
CYAN = \033[0;36m
NC = \033[0m

# ==============================================================================
# MAIN TARGETS
# ==============================================================================

.PHONY: all run test valgrind clean help solutions

all: $(EXECUTABLES)
	@echo "$(GREEN)✓ All sources compiled successfully!$(NC)"

# Generic compilation rule
%: $(SRC_DIR)/%.c
	@echo "$(YELLOW)Compiling $<...$(NC)"
	@$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# ==============================================================================
# RUN TARGETS
# ==============================================================================

run: all
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)                    RUNNING PROGRAMS                        $(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════$(NC)"
	@for exe in $(EXECUTABLES); do \
		if [ -x "./$$exe" ]; then \
			echo ""; \
			echo "$(YELLOW)▶ Running $$exe...$(NC)"; \
			echo "─────────────────────────────────────────────────────────────"; \
			./$$exe; \
			echo ""; \
		fi; \
	done

run-example: example1
	@echo "$(YELLOW)▶ Running example1...$(NC)"
	@./example1

run-exercise1: exercise1
	@echo "$(YELLOW)▶ Running exercise1 (interactive)...$(NC)"
	@./exercise1

run-exercise2: exercise2
	@echo "$(YELLOW)▶ Running exercise2 with sample data...$(NC)"
	@./exercise2 $(DATA_DIR)/students.txt

# ==============================================================================
# TESTING
# ==============================================================================

test: all
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)                    RUNNING TESTS                           $(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════$(NC)"
	@passed=0; failed=0; \
	echo ""; \
	echo "$(YELLOW)Testing exercise1 (calculator)...$(NC)"; \
	if [ -x "./exercise1" ] && [ -f "$(TEST_DIR)/test1_input.txt" ]; then \
		./exercise1 < $(TEST_DIR)/test1_input.txt > /tmp/test1_output.txt 2>&1; \
		if diff -q /tmp/test1_output.txt $(TEST_DIR)/test1_expected.txt > /dev/null 2>&1; then \
			echo "$(GREEN)  ✓ exercise1: PASSED$(NC)"; \
			passed=$$((passed + 1)); \
		else \
			echo "$(RED)  ✗ exercise1: FAILED$(NC)"; \
			echo "  Expected vs Got:"; \
			diff $(TEST_DIR)/test1_expected.txt /tmp/test1_output.txt | head -20; \
			failed=$$((failed + 1)); \
		fi; \
	else \
		echo "$(YELLOW)  ⚠ exercise1: SKIPPED (not compiled or test file missing)$(NC)"; \
	fi; \
	echo ""; \
	echo "$(YELLOW)Testing exercise2 (student database)...$(NC)"; \
	if [ -x "./exercise2" ] && [ -f "$(TEST_DIR)/test2_input.txt" ]; then \
		./exercise2 $(TEST_DIR)/test2_input.txt > /tmp/test2_output.txt 2>&1; \
		if grep -q "Loaded 3 students" /tmp/test2_output.txt; then \
			echo "$(GREEN)  ✓ exercise2: PASSED (basic load test)$(NC)"; \
			passed=$$((passed + 1)); \
		else \
			echo "$(RED)  ✗ exercise2: FAILED$(NC)"; \
			failed=$$((failed + 1)); \
		fi; \
	else \
		echo "$(YELLOW)  ⚠ exercise2: SKIPPED$(NC)"; \
	fi; \
	echo ""; \
	echo "$(CYAN)═══════════════════════════════════════════════════════════$(NC)"; \
	echo "Results: $(GREEN)$$passed passed$(NC), $(RED)$$failed failed$(NC)"

# ==============================================================================
# MEMORY CHECKING
# ==============================================================================

valgrind: all
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)              CHECKING MEMORY LEAKS                         $(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════$(NC)"
	@for exe in $(EXECUTABLES); do \
		if [ -x "./$$exe" ]; then \
			echo ""; \
			echo "$(YELLOW)▶ Valgrind $$exe...$(NC)"; \
			if [ "$$exe" = "exercise1" ]; then \
				echo "5 + 3" | valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./$$exe 2>&1 | grep -E "(ERROR SUMMARY|definitely lost|indirectly lost|possibly lost)"; \
			elif [ "$$exe" = "exercise2" ]; then \
				valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./$$exe $(DATA_DIR)/students.txt 2>&1 | grep -E "(ERROR SUMMARY|definitely lost|indirectly lost|possibly lost)"; \
			else \
				valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./$$exe 2>&1 | grep -E "(ERROR SUMMARY|definitely lost|indirectly lost|possibly lost)"; \
			fi; \
		fi; \
	done

# ==============================================================================
# SOLUTIONS (Instructor only)
# ==============================================================================

solutions: $(SOL_EXECUTABLES)
	@echo "$(GREEN)✓ Solutions compiled$(NC)"

$(SOL_DIR)/%: $(SOL_DIR)/%.c
	@echo "$(YELLOW)Compiling solution $<...$(NC)"
	@$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# ==============================================================================
# CLEANUP
# ==============================================================================

clean:
	@echo "$(YELLOW)Cleaning build artifacts...$(NC)"
	@rm -f $(EXECUTABLES)
	@rm -f $(SOL_EXECUTABLES)
	@rm -f /tmp/test*_output.txt
	@rm -f *.o
	@rm -f core
	@echo "$(GREEN)✓ Clean complete!$(NC)"

# ==============================================================================
# HELP
# ==============================================================================

help:
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)     WEEK 01: POINTERS TO FUNCTIONS - MAKEFILE HELP        $(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "  $(YELLOW)make$(NC)              - Compile all source files"
	@echo "  $(YELLOW)make run$(NC)          - Run all compiled programs"
	@echo "  $(YELLOW)make run-example$(NC)  - Run the example program only"
	@echo "  $(YELLOW)make run-exercise1$(NC)- Run exercise1 (interactive)"
	@echo "  $(YELLOW)make run-exercise2$(NC)- Run exercise2 with sample data"
	@echo "  $(YELLOW)make test$(NC)         - Run automated tests"
	@echo "  $(YELLOW)make valgrind$(NC)     - Check for memory leaks"
	@echo "  $(YELLOW)make clean$(NC)        - Remove compiled files"
	@echo "  $(YELLOW)make help$(NC)         - Show this help message"
	@echo ""
	@echo "$(CYAN)Source files found:$(NC)"
	@for src in $(SOURCES); do echo "  - $$src"; done
	@echo ""
	@echo "$(CYAN)Executables:$(NC)"
	@for exe in $(EXECUTABLES); do echo "  - $$exe"; done
	@echo ""

# ==============================================================================
# SPECIAL TARGETS
# ==============================================================================

# Prevent make from deleting intermediate files
.PRECIOUS: $(EXECUTABLES)

# Debug info
debug:
	@echo "SOURCES: $(SOURCES)"
	@echo "EXECUTABLES: $(EXECUTABLES)"
	@echo "SOL_SOURCES: $(SOL_SOURCES)"
