# =============================================================================
# DOCKERFILE - Week 20: Parallel and Concurrent Programming
# =============================================================================
# 
# Algorithms and Programming Techniques
# Academy of Economic Studies, Bucharest
#
# Build: docker build -t week20-parallel .
# Run:   docker run --rm week20-parallel
# Shell: docker run --rm -it week20-parallel /bin/bash
#
# =============================================================================

FROM ubuntu:22.04

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install build tools and debugging utilities
RUN apt-get update && apt-get install -y \
    gcc \
    make \
    valgrind \
    gdb \
    time \
    htop \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy all source files
COPY . .

# Build all targets
RUN make all

# Default command: run the demonstration
CMD ["./example1"]

# =============================================================================
# Usage Examples:
# =============================================================================
#
# Build the image:
#   docker build -t week20-parallel .
#
# Run the demonstration:
#   docker run --rm week20-parallel
#
# Run a specific exercise:
#   docker run --rm week20-parallel ./exercise1 data/access.log
#   docker run --rm week20-parallel ./exercise2 1000000
#
# Run with benchmarking:
#   docker run --rm week20-parallel make benchmark
#
# Interactive shell for debugging:
#   docker run --rm -it week20-parallel /bin/bash
#
# Run with Valgrind:
#   docker run --rm week20-parallel make valgrind
#
# =============================================================================
