# ==============================================================================
# MAKEFILE - Week 18: Machine Learning Fundamentals in C
# ATP Course - ASE-CSIE
# ==============================================================================
#
# Usage:
#   make              - Compile all sources
#   make run          - Run all executables
#   make test         - Run automated tests
#   make valgrind     - Check for memory leaks
#   make docker       - Build and run in Docker
#   make clean        - Remove compiled files
#   make help         - Show this help message
#
# ==============================================================================

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pedantic -g -O0
LDFLAGS = -lm

# Directories
SRC_DIR = src
TEST_DIR = tests
DATA_DIR = data
SOL_DIR = solution

# Source files and executables
SOURCES = $(wildcard $(SRC_DIR)/*.c)
EXECUTABLES = $(patsubst $(SRC_DIR)/%.c,%,$(SOURCES))

# Solution files
SOL_SOURCES = $(wildcard $(SOL_DIR)/*.c)
SOL_EXECUTABLES = $(patsubst $(SOL_DIR)/%.c,$(SOL_DIR)/%,$(SOL_SOURCES))

# Colours for terminal output
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
BLUE = \033[0;34m
CYAN = \033[0;36m
MAGENTA = \033[0;35m
BOLD = \033[1m
NC = \033[0m

# ==============================================================================
# MAIN TARGETS
# ==============================================================================

.PHONY: all run test valgrind clean help solutions docker

all: $(EXECUTABLES)
	@echo "$(GREEN)✓ All sources compiled successfully!$(NC)"
	@echo "$(CYAN)  Executables: $(EXECUTABLES)$(NC)"

# Generic compilation rule
%: $(SRC_DIR)/%.c
	@echo "$(YELLOW)Compiling $<...$(NC)"
	@$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	@echo "$(GREEN)  ✓ $@ created$(NC)"

# ==============================================================================
# RUN TARGETS
# ==============================================================================

run: all
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)$(BOLD)         MACHINE LEARNING FUNDAMENTALS - RUNNING           $(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════$(NC)"
	@for exe in $(EXECUTABLES); do \
		if [ -x "./$$exe" ]; then \
			echo ""; \
			echo "$(YELLOW)▶ Running $$exe...$(NC)"; \
			echo "$(BLUE)─────────────────────────────────────────────────────────────$(NC)"; \
			./$$exe; \
			echo ""; \
		fi; \
	done

run-example: example1
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)$(BOLD)         RUNNING COMPLETE ML DEMONSTRATION                 $(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@./example1

run-exercise1: exercise1
	@echo "$(YELLOW)▶ Running exercise1 (Linear Regression)...$(NC)"
	@./exercise1

run-exercise2: exercise2
	@echo "$(YELLOW)▶ Running exercise2 (K-NN and K-Means)...$(NC)"
	@./exercise2

# ==============================================================================
# TESTING
# ==============================================================================

test: all
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)$(BOLD)                 RUNNING AUTOMATED TESTS                    $(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════$(NC)"
	@passed=0; failed=0; \
	for i in 1 2; do \
		echo ""; \
		echo "$(YELLOW)Testing exercise$$i...$(NC)"; \
		if [ -x "./exercise$$i" ] && [ -f "$(TEST_DIR)/test$${i}_input.txt" ]; then \
			./exercise$$i < $(TEST_DIR)/test$${i}_input.txt > /tmp/test$${i}_output.txt 2>&1; \
			if diff -q /tmp/test$${i}_output.txt $(TEST_DIR)/test$${i}_expected.txt > /dev/null 2>&1; then \
				echo "$(GREEN)  ✓ exercise$$i: PASSED$(NC)"; \
				passed=$$((passed + 1)); \
			else \
				echo "$(RED)  ✗ exercise$$i: FAILED$(NC)"; \
				echo "  $(YELLOW)Expected vs Got:$(NC)"; \
				diff $(TEST_DIR)/test$${i}_expected.txt /tmp/test$${i}_output.txt | head -20; \
				failed=$$((failed + 1)); \
			fi; \
		else \
			echo "$(YELLOW)  ⚠ exercise$$i: SKIPPED (not compiled or test file missing)$(NC)"; \
		fi; \
	done; \
	echo ""; \
	echo "$(CYAN)═══════════════════════════════════════════════════════════$(NC)"; \
	echo "$(BOLD)Results: $(GREEN)$$passed passed$(NC), $(RED)$$failed failed$(NC)"

# ==============================================================================
# MEMORY CHECKING
# ==============================================================================

valgrind: all
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)$(BOLD)              CHECKING MEMORY LEAKS                          $(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════$(NC)"
	@for exe in $(EXECUTABLES); do \
		if [ -x "./$$exe" ]; then \
			echo ""; \
			echo "$(YELLOW)▶ Valgrind checking $$exe...$(NC)"; \
			valgrind --leak-check=full --show-leak-kinds=all \
				--error-exitcode=1 ./$$exe 2>&1 | \
				grep -E "(ERROR SUMMARY|definitely lost|indirectly lost|possibly lost|still reachable)" || true; \
		fi; \
	done

valgrind-example: example1
	@echo "$(YELLOW)▶ Running valgrind on example1...$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./example1

# ==============================================================================
# DOCKER
# ==============================================================================

docker:
	@echo "$(CYAN)Building Docker image...$(NC)"
	@docker build -t week-18-ml .
	@echo "$(GREEN)✓ Docker image built$(NC)"
	@echo "$(CYAN)Running in Docker...$(NC)"
	@docker run --rm week-18-ml

docker-build:
	@echo "$(CYAN)Building Docker image...$(NC)"
	@docker build -t week-18-ml .
	@echo "$(GREEN)✓ Docker image built: week-18-ml$(NC)"

docker-shell:
	@echo "$(CYAN)Starting interactive Docker shell...$(NC)"
	@docker run -it --rm -v $(PWD):/app week-18-ml /bin/bash

# ==============================================================================
# SOLUTIONS
# ==============================================================================

solutions: $(SOL_EXECUTABLES)
	@echo "$(GREEN)✓ Solutions compiled$(NC)"

$(SOL_DIR)/%: $(SOL_DIR)/%.c
	@echo "$(YELLOW)Compiling solution $<...$(NC)"
	@$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

run-solutions: solutions
	@echo ""
	@echo "$(MAGENTA)═══════════════════════════════════════════════════════════$(NC)"
	@echo "$(MAGENTA)$(BOLD)              RUNNING SOLUTION FILES                        $(NC)"
	@echo "$(MAGENTA)═══════════════════════════════════════════════════════════$(NC)"
	@for exe in $(SOL_EXECUTABLES); do \
		if [ -x "$$exe" ]; then \
			echo ""; \
			echo "$(YELLOW)▶ Running $$exe...$(NC)"; \
			./$$exe; \
		fi; \
	done

# ==============================================================================
# DATA GENERATION
# ==============================================================================

generate-data:
	@echo "$(CYAN)Generating sample datasets...$(NC)"
	@mkdir -p $(DATA_DIR)
	@echo "$(GREEN)✓ Data files ready in $(DATA_DIR)/$(NC)"

# ==============================================================================
# BENCHMARKS
# ==============================================================================

benchmark: example1
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)$(BOLD)              ML ALGORITHMS BENCHMARK                        $(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(YELLOW)Running benchmark suite...$(NC)"
	@time ./example1 > /dev/null
	@echo ""
	@echo "$(GREEN)✓ Benchmark complete$(NC)"

# ==============================================================================
# PYTHON COMPARISON
# ==============================================================================

python-compare:
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)$(BOLD)           COMPARING WITH PYTHON IMPLEMENTATIONS            $(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════$(NC)"
	@if command -v python3 &> /dev/null; then \
		echo "$(YELLOW)Running Python implementations...$(NC)"; \
		for pyfile in python_comparison/*.py; do \
			if [ -f "$$pyfile" ]; then \
				echo ""; \
				echo "$(BLUE)▶ $$pyfile$(NC)"; \
				python3 "$$pyfile" || true; \
			fi; \
		done; \
	else \
		echo "$(RED)Python3 not found. Install python3 to run comparisons.$(NC)"; \
	fi

# ==============================================================================
# DOCUMENTATION
# ==============================================================================

docs:
	@echo "$(CYAN)Opening documentation...$(NC)"
	@if command -v xdg-open &> /dev/null; then \
		xdg-open README.md; \
	elif command -v open &> /dev/null; then \
		open README.md; \
	else \
		cat README.md | less; \
	fi

slides:
	@echo "$(CYAN)Opening presentation slides...$(NC)"
	@if [ -f "slides/presentation-week18.html" ]; then \
		if command -v xdg-open &> /dev/null; then \
			xdg-open slides/presentation-week18.html; \
		elif command -v open &> /dev/null; then \
			open slides/presentation-week18.html; \
		fi; \
	else \
		echo "$(RED)Slides not found$(NC)"; \
	fi

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
	@rm -f *.csv
	@rm -f *.log
	@echo "$(GREEN)✓ Clean complete!$(NC)"

distclean: clean
	@echo "$(YELLOW)Deep cleaning...$(NC)"
	@rm -rf __pycache__
	@rm -f *.pyc
	@rm -f *~
	@rm -f \#*
	@echo "$(GREEN)✓ Distribution clean complete!$(NC)"

# ==============================================================================
# HELP
# ==============================================================================

help:
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)$(BOLD)    WEEK 18: MACHINE LEARNING FUNDAMENTALS - MAKEFILE HELP  $(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(BOLD)Build Commands:$(NC)"
	@echo "  $(YELLOW)make$(NC)              - Compile all source files"
	@echo "  $(YELLOW)make example1$(NC)     - Compile only the example"
	@echo "  $(YELLOW)make exercise1$(NC)    - Compile exercise 1"
	@echo "  $(YELLOW)make exercise2$(NC)    - Compile exercise 2"
	@echo "  $(YELLOW)make solutions$(NC)    - Compile solutions (instructor)"
	@echo ""
	@echo "$(BOLD)Run Commands:$(NC)"
	@echo "  $(YELLOW)make run$(NC)          - Run all compiled programmes"
	@echo "  $(YELLOW)make run-example$(NC)  - Run the example programme only"
	@echo "  $(YELLOW)make run-exercise1$(NC)- Run exercise 1"
	@echo "  $(YELLOW)make run-exercise2$(NC)- Run exercise 2"
	@echo "  $(YELLOW)make run-solutions$(NC)- Run solution files"
	@echo ""
	@echo "$(BOLD)Testing Commands:$(NC)"
	@echo "  $(YELLOW)make test$(NC)         - Run automated tests"
	@echo "  $(YELLOW)make valgrind$(NC)     - Check for memory leaks"
	@echo "  $(YELLOW)make benchmark$(NC)    - Run performance benchmark"
	@echo ""
	@echo "$(BOLD)Docker Commands:$(NC)"
	@echo "  $(YELLOW)make docker$(NC)       - Build and run in Docker"
	@echo "  $(YELLOW)make docker-build$(NC) - Build Docker image only"
	@echo "  $(YELLOW)make docker-shell$(NC) - Start interactive shell"
	@echo ""
	@echo "$(BOLD)Other Commands:$(NC)"
	@echo "  $(YELLOW)make python-compare$(NC) - Run Python implementations"
	@echo "  $(YELLOW)make docs$(NC)         - View documentation"
	@echo "  $(YELLOW)make slides$(NC)       - Open presentation slides"
	@echo "  $(YELLOW)make clean$(NC)        - Remove compiled files"
	@echo "  $(YELLOW)make help$(NC)         - Show this help message"
	@echo ""
	@echo "$(CYAN)Source files found:$(NC)"
	@for src in $(SOURCES); do echo "  - $$src"; done
	@echo ""
	@echo "$(CYAN)Algorithms covered:$(NC)"
	@echo "  • Linear Regression (gradient descent)"
	@echo "  • Logistic Regression (binary classification)"
	@echo "  • K-Nearest Neighbours (K-NN)"
	@echo "  • K-Means Clustering"
	@echo "  • Decision Trees (Gini impurity)"
	@echo "  • Perceptron and Neural Networks"
	@echo ""

# ==============================================================================
# SPECIAL TARGETS
# ==============================================================================

.PRECIOUS: $(EXECUTABLES)

# Prevent make from deleting intermediate files
.SECONDARY:

# ==============================================================================
# INDIVIDUAL EXERCISES
# ==============================================================================

exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(YELLOW)Compiling exercise1 (Linear Regression)...$(NC)"
	@$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise1 created$(NC)"

exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(YELLOW)Compiling exercise2 (K-NN and K-Means)...$(NC)"
	@$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise2 created$(NC)"

example1: $(SRC_DIR)/example1.c
	@echo "$(YELLOW)Compiling example1 (Complete ML Demo)...$(NC)"
	@$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	@echo "$(GREEN)  ✓ example1 created$(NC)"
