# ==============================================================================
# Dockerfile - Week 19: Algorithms for IoT and Stream Processing
# ATP Course - ASE-CSIE
# ==============================================================================
#
# Build:
#   docker build -t week-19-iot .
#
# Run:
#   docker run --rm week-19-iot
#
# Interactive:
#   docker run --rm -it week-19-iot bash
#
# ==============================================================================

FROM ubuntu:22.04

LABEL maintainer="ATP Course - ASE-CSIE"
LABEL description="Week 19: Algorithms for IoT and Stream Processing"
LABEL version="1.0"

# Prevent interactive prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# Install required packages
RUN apt-get update && apt-get install -y \
    gcc \
    make \
    valgrind \
    gdb \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source files
COPY src/ ./src/
COPY data/ ./data/
COPY tests/ ./tests/
COPY Makefile ./
COPY README.md ./

# Copy solution files if they exist
COPY solution/ ./solution/ 2>/dev/null || true

# Build all targets
RUN make all

# Default command: run the example
CMD ["./example1"]
