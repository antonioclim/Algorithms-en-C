# ==============================================================================
# DOCKERFILE - Week 18: Machine Learning Fundamentals in C
# ATP Course - ASE-CSIE
# ==============================================================================
#
# Build:  docker build -t week-18-ml .
# Run:    docker run --rm week-18-ml
# Shell:  docker run -it --rm week-18-ml /bin/bash
#
# ==============================================================================

FROM ubuntu:24.04

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Set locale for proper character handling
ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8

# Install required packages
RUN apt-get update && apt-get install -y \
    gcc \
    gdb \
    make \
    valgrind \
    python3 \
    python3-pip \
    python3-numpy \
    python3-scipy \
    bc \
    time \
    && rm -rf /var/lib/apt/lists/*

# Create working directory
WORKDIR /app

# Copy all source files
COPY . /app/

# Ensure proper line endings (in case of Windows development)
RUN find /app -type f -name "*.c" -exec sed -i 's/\r$//' {} \; 2>/dev/null || true
RUN find /app -type f -name "*.h" -exec sed -i 's/\r$//' {} \; 2>/dev/null || true
RUN find /app -type f -name "Makefile" -exec sed -i 's/\r$//' {} \; 2>/dev/null || true

# Compile all sources
RUN make all

# Default command: run the example
CMD ["make", "run-example"]

# ==============================================================================
# USAGE INSTRUCTIONS
# ==============================================================================
#
# Build the Docker image:
#   docker build -t week-18-ml .
#
# Run the demonstration:
#   docker run --rm week-18-ml
#
# Run with custom command:
#   docker run --rm week-18-ml make test
#   docker run --rm week-18-ml make valgrind
#   docker run --rm week-18-ml ./exercise1
#
# Interactive shell:
#   docker run -it --rm week-18-ml /bin/bash
#
# Mount local directory for development:
#   docker run -it --rm -v $(pwd):/app week-18-ml /bin/bash
#
# ==============================================================================
