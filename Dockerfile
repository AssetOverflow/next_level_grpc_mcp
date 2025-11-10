# Dockerfile for gMCP - Multi-stage build for optimized image

# Stage 1: Build environment
FROM ubuntu:22.04 as builder

ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    autoconf \
    automake \
    libtool \
    curl \
    make \
    g++ \
    unzip \
    && rm -rf /var/lib/apt/lists/*

# Install gRPC and protobuf
RUN apt-get update && apt-get install -y \
    libgrpc++-dev \
    libprotobuf-dev \
    protobuf-compiler-grpc \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source files
COPY . .

# Build gMCP
RUN mkdir -p build && cd build && \
    cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_CXX_STANDARD=20 && \
    make -j$(nproc)

# Stage 2: Runtime environment
FROM ubuntu:22.04

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libgrpc++1.51 \
    libprotobuf23 \
    && rm -rf /var/lib/apt/lists/*

# Copy built binaries
COPY --from=builder /app/build/gmcp_server /usr/local/bin/
COPY --from=builder /app/build/gmcp_client /usr/local/bin/

# Expose gRPC port
EXPOSE 50051

# Run server by default
CMD ["gmcp_server"]
