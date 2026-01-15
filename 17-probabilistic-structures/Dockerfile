# ==============================================================================
# DOCKERFILE - Week 17: Probabilistic Data Structures for Big Data
# ATP Course - ASE-CSIE Bucharest
# ==============================================================================
#
# This Dockerfile creates a self-contained environment for running the
# Week 17 laboratory exercises on probabilistic data structures.
#
# Build:   docker build -t week-17-probabilistic .
# Run:     docker run --rm week-17-probabilistic
# Shell:   docker run --rm -it week-17-probabilistic /bin/bash
#
# ==============================================================================

FROM ubuntu:22.04

# Metadata
LABEL maintainer="ATP Course - ASE-CSIE"
LABEL description="Week 17: Probabilistic Data Structures for Big Data"
LABEL version="1.0"

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install build tools and dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    gcc \
    g++ \
    make \
    gdb \
    valgrind \
    libc6-dev \
    vim \
    nano \
    less \
    man-db \
    && rm -rf /var/lib/apt/lists/* \
    && apt-get clean

# Create working directory structure
WORKDIR /app

# Copy all source files and materials
COPY . /app/

# Set appropriate permissions
RUN chmod -R 755 /app

# Build all executables
RUN make all 2>/dev/null || true

# Default command: run the demonstration
CMD ["sh", "-c", "echo '═══════════════════════════════════════════════════════════' && \
    echo '     Week 17: Probabilistic Data Structures for Big Data' && \
    echo '     ATP Course - ASE-CSIE Bucharest' && \
    echo '═══════════════════════════════════════════════════════════' && \
    echo '' && \
    if [ -x ./example1 ]; then \
        echo 'Running example1 demonstration...' && \
        echo '' && \
        ./example1; \
    else \
        echo 'Building example1...' && \
        make example1 && \
        echo '' && \
        echo 'Running example1 demonstration...' && \
        echo '' && \
        ./example1; \
    fi && \
    echo '' && \
    echo '═══════════════════════════════════════════════════════════' && \
    echo '     Demonstration complete. Use -it flag for interactive.' && \
    echo '═══════════════════════════════════════════════════════════'"]

# ==============================================================================
# USAGE INSTRUCTIONS
# ==============================================================================
#
# 1. Build the Docker image:
#    $ docker build -t week-17-probabilistic .
#
# 2. Run the demonstration:
#    $ docker run --rm week-17-probabilistic
#
# 3. Interactive shell for development:
#    $ docker run --rm -it week-17-probabilistic /bin/bash
#
# 4. Run specific exercise:
#    $ docker run --rm week-17-probabilistic ./exercise1
#
# 5. Run with Valgrind:
#    $ docker run --rm week-17-probabilistic valgrind ./example1
#
# 6. Mount local directory for development:
#    $ docker run --rm -it -v $(pwd):/app week-17-probabilistic /bin/bash
#
# ==============================================================================
