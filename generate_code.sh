#!/bin/bash
# Generate gRPC code from Protocol Buffer definitions
# This script can be used to generate client code for different languages

set -e

PROTO_DIR="proto"
PROTO_FILE="gmcp.proto"

echo "=== gMCP Code Generator ==="

# C++ (included in CMake build, but can be run manually)
generate_cpp() {
    echo "Generating C++ code..."
    OUT_DIR="generated/cpp"
    mkdir -p $OUT_DIR
    
    protoc --cpp_out=$OUT_DIR \
           --grpc_out=$OUT_DIR \
           --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) \
           -I=$PROTO_DIR \
           $PROTO_DIR/$PROTO_FILE
    
    echo "C++ files generated in $OUT_DIR"
}

# Python
generate_python() {
    echo "Generating Python code..."
    OUT_DIR="generated/python"
    mkdir -p $OUT_DIR
    
    python -m grpc_tools.protoc \
           -I=$PROTO_DIR \
           --python_out=$OUT_DIR \
           --grpc_python_out=$OUT_DIR \
           $PROTO_DIR/$PROTO_FILE
    
    echo "Python files generated in $OUT_DIR"
}

# Go
generate_go() {
    echo "Generating Go code..."
    OUT_DIR="generated/go"
    mkdir -p $OUT_DIR
    
    protoc --go_out=$OUT_DIR \
           --go_opt=paths=source_relative \
           --go-grpc_out=$OUT_DIR \
           --go-grpc_opt=paths=source_relative \
           -I=$PROTO_DIR \
           $PROTO_DIR/$PROTO_FILE
    
    echo "Go files generated in $OUT_DIR"
}

# Java
generate_java() {
    echo "Generating Java code..."
    OUT_DIR="generated/java"
    mkdir -p $OUT_DIR
    
    protoc --java_out=$OUT_DIR \
           --grpc-java_out=$OUT_DIR \
           -I=$PROTO_DIR \
           $PROTO_DIR/$PROTO_FILE
    
    echo "Java files generated in $OUT_DIR"
}

# JavaScript/Node.js
generate_js() {
    echo "Generating JavaScript code..."
    OUT_DIR="generated/js"
    mkdir -p $OUT_DIR
    
    grpc_tools_node_protoc \
           --js_out=import_style=commonjs,binary:$OUT_DIR \
           --grpc_out=grpc_js:$OUT_DIR \
           -I=$PROTO_DIR \
           $PROTO_DIR/$PROTO_FILE
    
    echo "JavaScript files generated in $OUT_DIR"
}

# Main menu
if [ $# -eq 0 ]; then
    echo ""
    echo "Usage: $0 [language]"
    echo ""
    echo "Available languages:"
    echo "  cpp      - Generate C++ code"
    echo "  python   - Generate Python code"
    echo "  go       - Generate Go code"
    echo "  java     - Generate Java code"
    echo "  js       - Generate JavaScript code"
    echo "  all      - Generate code for all languages"
    echo ""
    exit 1
fi

case "$1" in
    cpp)
        generate_cpp
        ;;
    python)
        generate_python
        ;;
    go)
        generate_go
        ;;
    java)
        generate_java
        ;;
    js)
        generate_js
        ;;
    all)
        generate_cpp
        generate_python
        generate_go
        generate_java
        generate_js
        ;;
    *)
        echo "Unknown language: $1"
        echo "Run without arguments to see usage"
        exit 1
        ;;
esac

echo ""
echo "Code generation complete!"
