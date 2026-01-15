# =============================================================================
# WEEK 03: BINARY FILES IN C - Makefile
# =============================================================================
# 
# Build automation for the Binary Files laboratory exercises.
# Supports compilation, testing, memory checking and code formatting.
#
# Usage:
#   make          - Build all targets
#   make run      - Run all examples
#   make test     - Run automated tests
#   make valgrind - Check for memory leaks
#   make clean    - Remove build artefacts
#   make help     - Display this help message
#
# =============================================================================

# -----------------------------------------------------------------------------
# Compiler Configuration
# -----------------------------------------------------------------------------

CC          = gcc
CFLAGS      = -Wall -Wextra -Wpedantic -std=c11 -g
CFLAGS_OPT  = -Wall -Wextra -Wpedantic -std=c11 -O2
LDFLAGS     = -lm

# -----------------------------------------------------------------------------
# Directory Configuration
# -----------------------------------------------------------------------------

SRC_DIR     = src
DATA_DIR    = data
TEST_DIR    = tests
SOL_DIR     = solution
BUILD_DIR   = build

# -----------------------------------------------------------------------------
# Colour Definitions (ANSI Escape Codes)
# -----------------------------------------------------------------------------

# Text colours
BLACK       = \033[0;30m
RED         = \033[0;31m
GREEN       = \033[0;32m
YELLOW      = \033[0;33m
BLUE        = \033[0;34m
MAGENTA     = \033[0;35m
CYAN        = \033[0;36m
WHITE       = \033[0;37m

# Bold colours
BOLD_RED    = \033[1;31m
BOLD_GREEN  = \033[1;32m
BOLD_YELLOW = \033[1;33m
BOLD_BLUE   = \033[1;34m
BOLD_CYAN   = \033[1;36m
BOLD_WHITE  = \033[1;37m

# Reset
NC          = \033[0m

# Status symbols
CHECK       = ✓
CROSS       = ✗
ARROW       = →
BULLET      = •
GEAR        = ⚙

# -----------------------------------------------------------------------------
# Target Definitions
# -----------------------------------------------------------------------------

# Main executables
TARGETS     = example1 exercise1 exercise2

# Solution executables (instructor only)
SOL_TARGETS = exercise1_sol exercise2_sol homework1_sol homework2_sol

# Binary data files (generated during execution)
DATA_FILES  = students.bin products.bin products.idx

# -----------------------------------------------------------------------------
# Phony Targets
# -----------------------------------------------------------------------------

.PHONY: all clean run run-example run-exercises test valgrind \
        solutions clean-data format check help info

# -----------------------------------------------------------------------------
# Default Target
# -----------------------------------------------------------------------------

all: $(TARGETS)
	@echo ""
	@echo "$(BOLD_GREEN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(BOLD_GREEN)  $(CHECK) All targets built successfully$(NC)"
	@echo "$(BOLD_GREEN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(CYAN)  Available executables:$(NC)"
	@for target in $(TARGETS); do \
		echo "    $(BULLET) ./$$target"; \
	done
	@echo ""
	@echo "$(YELLOW)  Next steps:$(NC)"
	@echo "    $(ARROW) make run      $(WHITE)- Execute all examples$(NC)"
	@echo "    $(ARROW) make test     $(WHITE)- Run automated tests$(NC)"
	@echo "    $(ARROW) make valgrind $(WHITE)- Check for memory leaks$(NC)"
	@echo ""

# -----------------------------------------------------------------------------
# Compilation Rules
# -----------------------------------------------------------------------------

example1: $(SRC_DIR)/example1.c
	@echo "$(CYAN)$(GEAR) Building example1...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) example1 compiled successfully$(NC)"

exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(CYAN)$(GEAR) Building exercise1...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) exercise1 compiled successfully$(NC)"

exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(CYAN)$(GEAR) Building exercise2...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) exercise2 compiled successfully$(NC)"

# Solution builds (for instructors)
exercise1_sol: $(SOL_DIR)/exercise1_sol.c
	@echo "$(MAGENTA)$(GEAR) Building exercise1 solution...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) exercise1_sol compiled$(NC)"

exercise2_sol: $(SOL_DIR)/exercise2_sol.c
	@echo "$(MAGENTA)$(GEAR) Building exercise2 solution...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) exercise2_sol compiled$(NC)"

homework1_sol: $(SOL_DIR)/homework1_sol.c
	@echo "$(MAGENTA)$(GEAR) Building homework1 solution...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) homework1_sol compiled$(NC)"

homework2_sol: $(SOL_DIR)/homework2_sol.c
	@echo "$(MAGENTA)$(GEAR) Building homework2 solution...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) homework2_sol compiled$(NC)"

solutions: $(SOL_TARGETS)
	@echo ""
	@echo "$(BOLD_MAGENTA)  $(CHECK) All solutions built$(NC)"
	@echo ""

# -----------------------------------------------------------------------------
# Execution Rules
# -----------------------------------------------------------------------------

run: all
	@echo ""
	@echo "$(BOLD_YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(BOLD_YELLOW)  Running All Examples$(NC)"
	@echo "$(BOLD_YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@./example1
	@echo ""

run-example: example1
	@echo ""
	@echo "$(BOLD_CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(BOLD_CYAN)  Running Complete Example$(NC)"
	@echo "$(BOLD_CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@./example1

run-exercises: exercise1 exercise2
	@echo ""
	@echo "$(BOLD_CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(BOLD_CYAN)  Running Exercises$(NC)"
	@echo "$(BOLD_CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(YELLOW)$(ARROW) Exercise 1: Student Records$(NC)"
	@./exercise1 < $(TEST_DIR)/test1_input.txt
	@echo ""
	@echo "$(YELLOW)$(ARROW) Exercise 2: Indexed File System$(NC)"
	@./exercise2 < $(TEST_DIR)/test2_input.txt

# -----------------------------------------------------------------------------
# Testing Rules
# -----------------------------------------------------------------------------

test: exercise1 exercise2
	@echo ""
	@echo "$(BOLD_YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(BOLD_YELLOW)  Running Automated Tests$(NC)"
	@echo "$(BOLD_YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@PASS=0; FAIL=0; \
	echo "$(CYAN)$(ARROW) Test 1: Student Records Database$(NC)"; \
	rm -f students.bin; \
	./exercise1 < $(TEST_DIR)/test1_input.txt > test1_output.txt 2>&1; \
	if diff -q test1_output.txt $(TEST_DIR)/test1_expected.txt > /dev/null 2>&1; then \
		echo "$(GREEN)  $(CHECK) Test 1 PASSED$(NC)"; \
		PASS=$$((PASS + 1)); \
	else \
		echo "$(RED)  $(CROSS) Test 1 FAILED$(NC)"; \
		echo "$(YELLOW)    Differences:$(NC)"; \
		diff test1_output.txt $(TEST_DIR)/test1_expected.txt | head -10; \
		FAIL=$$((FAIL + 1)); \
	fi; \
	echo ""; \
	echo "$(CYAN)$(ARROW) Test 2: Indexed File System$(NC)"; \
	rm -f products.bin products.idx; \
	./exercise2 < $(TEST_DIR)/test2_input.txt > test2_output.txt 2>&1; \
	if diff -q test2_output.txt $(TEST_DIR)/test2_expected.txt > /dev/null 2>&1; then \
		echo "$(GREEN)  $(CHECK) Test 2 PASSED$(NC)"; \
		PASS=$$((PASS + 1)); \
	else \
		echo "$(RED)  $(CROSS) Test 2 FAILED$(NC)"; \
		echo "$(YELLOW)    Differences:$(NC)"; \
		diff test2_output.txt $(TEST_DIR)/test2_expected.txt | head -10; \
		FAIL=$$((FAIL + 1)); \
	fi; \
	echo ""; \
	echo "$(BOLD_WHITE)═══════════════════════════════════════════════════════════════$(NC)"; \
	echo "  Test Summary: $(GREEN)$$PASS passed$(NC), $(RED)$$FAIL failed$(NC)"; \
	echo "$(BOLD_WHITE)═══════════════════════════════════════════════════════════════$(NC)"; \
	rm -f test1_output.txt test2_output.txt

test-solutions: solutions
	@echo ""
	@echo "$(BOLD_MAGENTA)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(BOLD_MAGENTA)  Testing Solution Files$(NC)"
	@echo "$(BOLD_MAGENTA)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@rm -f students.bin products.bin products.idx
	@./exercise1_sol < $(TEST_DIR)/test1_input.txt > /dev/null
	@echo "$(GREEN)  $(CHECK) exercise1_sol executes correctly$(NC)"
	@./exercise2_sol < $(TEST_DIR)/test2_input.txt > /dev/null
	@echo "$(GREEN)  $(CHECK) exercise2_sol executes correctly$(NC)"

# -----------------------------------------------------------------------------
# Memory Checking Rules
# -----------------------------------------------------------------------------

valgrind: all
	@echo ""
	@echo "$(BOLD_YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(BOLD_YELLOW)  Running Valgrind Memory Check$(NC)"
	@echo "$(BOLD_YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(CYAN)$(ARROW) Checking example1...$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 \
		./example1 2>&1 | grep -E "(ERROR SUMMARY|definitely lost|indirectly lost|possibly lost)" || true
	@echo ""
	@echo "$(CYAN)$(ARROW) Checking exercise1...$(NC)"
	@rm -f students.bin
	@valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 \
		./exercise1 < $(TEST_DIR)/test1_input.txt 2>&1 | \
		grep -E "(ERROR SUMMARY|definitely lost|indirectly lost|possibly lost)" || true
	@echo ""
	@echo "$(CYAN)$(ARROW) Checking exercise2...$(NC)"
	@rm -f products.bin products.idx
	@valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 \
		./exercise2 < $(TEST_DIR)/test2_input.txt 2>&1 | \
		grep -E "(ERROR SUMMARY|definitely lost|indirectly lost|possibly lost)" || true
	@echo ""
	@echo "$(GREEN)  $(CHECK) Memory check complete$(NC)"

valgrind-full: example1
	@echo ""
	@echo "$(BOLD_YELLOW)  Full Valgrind Report for example1$(NC)"
	@echo ""
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./example1

# -----------------------------------------------------------------------------
# Utility Rules
# -----------------------------------------------------------------------------

clean:
	@echo ""
	@echo "$(YELLOW)$(GEAR) Cleaning build artefacts...$(NC)"
	@rm -f $(TARGETS) $(SOL_TARGETS)
	@rm -f *.o *.bin *.idx *.dat *.bak
	@rm -f test*_output.txt
	@rm -f core vgcore.*
	@echo "$(GREEN)  $(CHECK) Clean complete$(NC)"
	@echo ""

clean-data:
	@echo "$(YELLOW)$(GEAR) Removing generated data files...$(NC)"
	@rm -f *.bin *.idx *.dat
	@echo "$(GREEN)  $(CHECK) Data files removed$(NC)"

format:
	@echo "$(CYAN)$(GEAR) Formatting source files...$(NC)"
	@if command -v clang-format > /dev/null 2>&1; then \
		find $(SRC_DIR) $(SOL_DIR) -name "*.c" -exec clang-format -i {} \; ; \
		echo "$(GREEN)  $(CHECK) Source files formatted$(NC)"; \
	else \
		echo "$(YELLOW)  $(CROSS) clang-format not found$(NC)"; \
	fi

check:
	@echo "$(CYAN)$(GEAR) Static analysis with cppcheck...$(NC)"
	@if command -v cppcheck > /dev/null 2>&1; then \
		cppcheck --enable=all --std=c11 $(SRC_DIR)/*.c 2>&1 | head -20; \
	else \
		echo "$(YELLOW)  $(CROSS) cppcheck not found$(NC)"; \
	fi

# -----------------------------------------------------------------------------
# Information Rules
# -----------------------------------------------------------------------------

info:
	@echo ""
	@echo "$(BOLD_CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(BOLD_CYAN)  Week 03: Binary Files in C$(NC)"
	@echo "$(BOLD_CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(WHITE)  Compiler:     $(CYAN)$(CC)$(NC)"
	@echo "$(WHITE)  Flags:        $(CYAN)$(CFLAGS)$(NC)"
	@echo "$(WHITE)  Source Dir:   $(CYAN)$(SRC_DIR)/$(NC)"
	@echo "$(WHITE)  Test Dir:     $(CYAN)$(TEST_DIR)/$(NC)"
	@echo ""
	@echo "$(WHITE)  Source Files:$(NC)"
	@ls -la $(SRC_DIR)/*.c 2>/dev/null | awk '{print "    " $$NF}' || echo "    (none)"
	@echo ""

help:
	@echo ""
	@echo "$(BOLD_WHITE)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(BOLD_WHITE)  Week 03: Binary Files - Available Commands$(NC)"
	@echo "$(BOLD_WHITE)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(BOLD_CYAN)  Build Commands:$(NC)"
	@echo "    make              $(WHITE)Build all student executables$(NC)"
	@echo "    make example1     $(WHITE)Build the complete example$(NC)"
	@echo "    make exercise1    $(WHITE)Build exercise 1 (student records)$(NC)"
	@echo "    make exercise2    $(WHITE)Build exercise 2 (indexed files)$(NC)"
	@echo "    make solutions    $(WHITE)Build solution files (instructor)$(NC)"
	@echo ""
	@echo "$(BOLD_CYAN)  Execution Commands:$(NC)"
	@echo "    make run          $(WHITE)Run all examples$(NC)"
	@echo "    make run-example  $(WHITE)Run complete example only$(NC)"
	@echo "    make run-exercises$(WHITE)Run exercises with test input$(NC)"
	@echo ""
	@echo "$(BOLD_CYAN)  Testing Commands:$(NC)"
	@echo "    make test         $(WHITE)Run automated tests with diff$(NC)"
	@echo "    make valgrind     $(WHITE)Check for memory leaks$(NC)"
	@echo "    make valgrind-full$(WHITE)Full Valgrind report$(NC)"
	@echo ""
	@echo "$(BOLD_CYAN)  Utility Commands:$(NC)"
	@echo "    make clean        $(WHITE)Remove all build artefacts$(NC)"
	@echo "    make clean-data   $(WHITE)Remove generated data files$(NC)"
	@echo "    make format       $(WHITE)Format code with clang-format$(NC)"
	@echo "    make check        $(WHITE)Static analysis with cppcheck$(NC)"
	@echo "    make info         $(WHITE)Display build configuration$(NC)"
	@echo "    make help         $(WHITE)Show this help message$(NC)"
	@echo ""
	@echo "$(BOLD_WHITE)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""

# -----------------------------------------------------------------------------
# End of Makefile
# -----------------------------------------------------------------------------
