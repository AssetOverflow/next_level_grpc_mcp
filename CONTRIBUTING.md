# Contributing to gMCP

Thank you for your interest in contributing to gMCP! This document provides guidelines for contributing to the project.

## Development Setup

1. **Fork and Clone**
   ```bash
   git clone https://github.com/yourusername/next_level_grpc_mcp.git
   cd next_level_grpc_mcp
   ```

2. **Install Dependencies**
   - See README.md for platform-specific dependency installation

3. **Build the Project**
   ```bash
   ./build.sh
   ```

## Code Style

- **C++ Standard**: C++20
- **Formatting**: Follow existing code style
  - 4 spaces for indentation (no tabs)
  - Snake_case for variables and functions
  - PascalCase for classes and structs
  - Opening braces on same line for functions/classes
  
- **Comments**: Use clear, concise comments where necessary
  - Document public APIs with Doxygen-style comments
  - Explain "why" rather than "what" in implementation comments

## Making Changes

1. **Create a Branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Make Your Changes**
   - Write clean, maintainable code
   - Add tests if applicable
   - Update documentation

3. **Test Your Changes**
   ```bash
   ./build.sh
   ./build/gmcp_server &
   ./build/gmcp_client
   ```

4. **Commit Your Changes**
   ```bash
   git add .
   git commit -m "Brief description of changes"
   ```

5. **Push and Create PR**
   ```bash
   git push origin feature/your-feature-name
   ```

## Areas for Contribution

- **Performance Optimizations**: Improve latency and throughput
- **Additional Memory Backends**: Redis, PostgreSQL, Vector DBs
- **Client Libraries**: Python, Go, Java, JavaScript implementations
- **Tool Plugins**: More example tools and use cases
- **Documentation**: Tutorials, examples, API documentation
- **Testing**: Unit tests, integration tests, benchmarks
- **Security**: Authentication, authorization, encryption
- **Monitoring**: Metrics, logging, observability

## Code Review Process

1. All submissions require review
2. We aim to review PRs within 48 hours
3. Address reviewer feedback promptly
4. Once approved, maintainers will merge

## Reporting Issues

Use GitHub Issues to report:
- Bugs (with reproduction steps)
- Feature requests (with use cases)
- Documentation improvements

## License

By contributing, you agree that your contributions will be licensed under the same license as the project.
