# ==============================================================================
# MAKEFILE - Week 19: Algorithms for IoT and Stream Processing
# ATP Course - ASE-CSIE
# ==============================================================================
#
# Usage:
#   make              - Compile all sources
#   make run          - Run all executables
#   make run-example  - Run example programme only
#   make run-ex1      - Run exercise 1 with test data
#   make run-ex2      - Run exercise 2 simulation
#   make test         - Run automated tests
#   make valgrind     - Check for memory leaks
#   make docker       - Build and run in Docker
#   make solutions    - Compile solution files
#   make clean        - Remove compiled files
#   make help         - Show this help message
#   make info         - Show project information
#
# ==============================================================================

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pedantic -g -O2
LDFLAGS = -lm

# Directories
SRC_DIR = src
TEST_DIR = tests
DATA_DIR = data
SOL_DIR = solution

# Source files
EXAMPLE = example1
EXERCISE1 = exercise1
EXERCISE2 = exercise2
EXECUTABLES = $(EXAMPLE) $(EXERCISE1) $(EXERCISE2)

# Solution files
SOL_SOURCES = $(wildcard $(SOL_DIR)/*.c)
SOL_EXECUTABLES = $(patsubst $(SOL_DIR)/%.c,$(SOL_DIR)/%,$(SOL_SOURCES))

# Colours for terminal output
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
BLUE = \033[0;34m
CYAN = \033[0;36m
PURPLE = \033[0;35m
NC = \033[0m

# Week information
WEEK_NUM = 19
WEEK_TITLE = Algorithms for IoT and Stream Processing

# ==============================================================================
# MAIN TARGETS
# ==============================================================================

.PHONY: all run run-example run-ex1 run-ex2 test valgrind clean help info solutions docker

all: $(EXECUTABLES)
	@echo "$(GREEN)✓ All sources compiled successfully!$(NC)"

# Compile example
$(EXAMPLE): $(SRC_DIR)/$(EXAMPLE).c
	@echo "$(YELLOW)Compiling $<...$(NC)"
	@$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	@echo "$(GREEN)  ✓ $(EXAMPLE) ready$(NC)"

# Compile exercises
$(EXERCISE1): $(SRC_DIR)/$(EXERCISE1).c
	@echo "$(YELLOW)Compiling $<...$(NC)"
	@$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	@echo "$(GREEN)  ✓ $(EXERCISE1) ready$(NC)"

$(EXERCISE2): $(SRC_DIR)/$(EXERCISE2).c
	@echo "$(YELLOW)Compiling $<...$(NC)"
	@$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	@echo "$(GREEN)  ✓ $(EXERCISE2) ready$(NC)"

# ==============================================================================
# RUN TARGETS
# ==============================================================================

run: all
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)          WEEK $(WEEK_NUM): $(WEEK_TITLE)$(NC)"
	@echo "$(CYAN)                    Running All Programmes$(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(YELLOW)▶ Running $(EXAMPLE)...$(NC)"
	@echo "───────────────────────────────────────────────────────────────"
	@./$(EXAMPLE)
	@echo ""

run-example: $(EXAMPLE)
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)          Running Complete Example$(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@./$(EXAMPLE)

run-ex1: $(EXERCISE1)
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)          Running Exercise 1: Stream Processing Pipeline$(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(YELLOW)▶ With generated data:$(NC)"
	@./$(EXERCISE1) -w 10 -a 0.2 -t 2.5 -n 30
	@echo ""
	@if [ -f "$(DATA_DIR)/sensor_stream.txt" ]; then \
		echo "$(YELLOW)▶ With sensor_stream.txt:$(NC)"; \
		./$(EXERCISE1) -w 10 -a 0.2 -t 2.5 -stdin < $(DATA_DIR)/sensor_stream.txt; \
	fi

run-ex2: $(EXERCISE2)
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)          Running Exercise 2: MQTT Broker Simulation$(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@./$(EXERCISE2) 10

# ==============================================================================
# TESTING
# ==============================================================================

test: all
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)                    RUNNING AUTOMATED TESTS$(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@passed=0; failed=0; \
	echo "$(YELLOW)Testing exercise1...$(NC)"; \
	if [ -x "./$(EXERCISE1)" ] && [ -f "$(TEST_DIR)/test1_input.txt" ]; then \
		./$(EXERCISE1) -stdin < $(TEST_DIR)/test1_input.txt > /tmp/test1_output.txt 2>&1; \
		if grep -q "Stream Processing Pipeline" /tmp/test1_output.txt; then \
			echo "$(GREEN)  ✓ exercise1: PASSED (output generated)$(NC)"; \
			passed=$$((passed + 1)); \
		else \
			echo "$(RED)  ✗ exercise1: FAILED$(NC)"; \
			failed=$$((failed + 1)); \
		fi; \
	else \
		echo "$(YELLOW)  ⚠ exercise1: SKIPPED (missing files)$(NC)"; \
	fi; \
	echo ""; \
	echo "$(YELLOW)Testing exercise2...$(NC)"; \
	if [ -x "./$(EXERCISE2)" ] && [ -f "$(TEST_DIR)/test2_input.txt" ]; then \
		./$(EXERCISE2) < $(TEST_DIR)/test2_input.txt > /tmp/test2_output.txt 2>&1; \
		if grep -q "IoT Simulation" /tmp/test2_output.txt; then \
			echo "$(GREEN)  ✓ exercise2: PASSED (output generated)$(NC)"; \
			passed=$$((passed + 1)); \
		else \
			echo "$(RED)  ✗ exercise2: FAILED$(NC)"; \
			failed=$$((failed + 1)); \
		fi; \
	else \
		echo "$(YELLOW)  ⚠ exercise2: SKIPPED (missing files)$(NC)"; \
	fi; \
	echo ""; \
	echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"; \
	echo "Results: $(GREEN)$$passed passed$(NC), $(RED)$$failed failed$(NC)"

# ==============================================================================
# MEMORY CHECKING
# ==============================================================================

valgrind: all
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)              CHECKING FOR MEMORY LEAKS$(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@for exe in $(EXECUTABLES); do \
		if [ -x "./$$exe" ]; then \
			echo "$(YELLOW)▶ Checking $$exe...$(NC)"; \
			if [ "$$exe" = "exercise1" ]; then \
				valgrind --leak-check=full --show-leak-kinds=all \
					--error-exitcode=1 ./$$exe -n 20 2>&1 | \
					grep -E "(ERROR SUMMARY|definitely lost|indirectly lost|possibly lost)" || true; \
			elif [ "$$exe" = "exercise2" ]; then \
				valgrind --leak-check=full --show-leak-kinds=all \
					--error-exitcode=1 ./$$exe 5 2>&1 | \
					grep -E "(ERROR SUMMARY|definitely lost|indirectly lost|possibly lost)" || true; \
			else \
				valgrind --leak-check=full --show-leak-kinds=all \
					--error-exitcode=1 ./$$exe 2>&1 | \
					grep -E "(ERROR SUMMARY|definitely lost|indirectly lost|possibly lost)" || true; \
			fi; \
			echo ""; \
		fi; \
	done

# ==============================================================================
# DOCKER
# ==============================================================================

docker:
	@echo "$(CYAN)Building Docker image...$(NC)"
	@docker build -t week-19-iot .
	@echo "$(GREEN)✓ Docker image built$(NC)"
	@echo ""
	@echo "$(CYAN)Running in Docker...$(NC)"
	@docker run --rm week-19-iot

docker-interactive:
	@echo "$(CYAN)Building Docker image...$(NC)"
	@docker build -t week-19-iot .
	@echo "$(GREEN)✓ Starting interactive shell...$(NC)"
	@docker run --rm -it week-19-iot bash

# ==============================================================================
# SOLUTIONS
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
	@rm -f vgcore.*
	@echo "$(GREEN)✓ Clean complete!$(NC)"

# ==============================================================================
# HELP AND INFO
# ==============================================================================

help:
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)     WEEK $(WEEK_NUM): $(WEEK_TITLE)$(NC)"
	@echo "$(CYAN)                    MAKEFILE HELP$(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "  $(PURPLE)Compilation:$(NC)"
	@echo "    $(YELLOW)make$(NC)              - Compile all source files"
	@echo "    $(YELLOW)make solutions$(NC)    - Compile solution files"
	@echo ""
	@echo "  $(PURPLE)Execution:$(NC)"
	@echo "    $(YELLOW)make run$(NC)          - Run all programmes"
	@echo "    $(YELLOW)make run-example$(NC)  - Run complete example"
	@echo "    $(YELLOW)make run-ex1$(NC)      - Run exercise 1 (stream pipeline)"
	@echo "    $(YELLOW)make run-ex2$(NC)      - Run exercise 2 (MQTT broker)"
	@echo ""
	@echo "  $(PURPLE)Testing:$(NC)"
	@echo "    $(YELLOW)make test$(NC)         - Run automated tests"
	@echo "    $(YELLOW)make valgrind$(NC)     - Check for memory leaks"
	@echo ""
	@echo "  $(PURPLE)Docker:$(NC)"
	@echo "    $(YELLOW)make docker$(NC)       - Build and run in Docker"
	@echo "    $(YELLOW)make docker-interactive$(NC) - Start interactive Docker shell"
	@echo ""
	@echo "  $(PURPLE)Utilities:$(NC)"
	@echo "    $(YELLOW)make clean$(NC)        - Remove compiled files"
	@echo "    $(YELLOW)make help$(NC)         - Show this help message"
	@echo "    $(YELLOW)make info$(NC)         - Show project information"
	@echo ""

info:
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)     WEEK $(WEEK_NUM): $(WEEK_TITLE)$(NC)"
	@echo "$(CYAN)                    PROJECT INFORMATION$(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "  $(PURPLE)Topics Covered:$(NC)"
	@echo "    • Circular buffers (ring buffers) for sliding windows"
	@echo "    • Exponential Moving Average (EMA) filtering"
	@echo "    • Welford's online algorithm for statistics"
	@echo "    • Z-score anomaly detection"
	@echo "    • MQTT-style publish-subscribe messaging"
	@echo "    • Complete stream processing pipelines"
	@echo ""
	@echo "  $(PURPLE)Source Files:$(NC)"
	@for src in $(SRC_DIR)/*.c; do echo "    - $$src"; done
	@echo ""
	@echo "  $(PURPLE)Data Files:$(NC)"
	@if [ -d "$(DATA_DIR)" ]; then \
		for data in $(DATA_DIR)/*; do echo "    - $$data"; done; \
	fi
	@echo ""
	@echo "  $(PURPLE)Test Files:$(NC)"
	@if [ -d "$(TEST_DIR)" ]; then \
		for test in $(TEST_DIR)/*; do echo "    - $$test"; done; \
	fi
	@echo ""

# ==============================================================================
# PHONY TARGETS
# ==============================================================================

.PRECIOUS: $(EXECUTABLES) $(SOL_EXECUTABLES)
