# next_level_grpc_mcp

A next-level neural-bus framework that connects agents and MCP servers using Deephaven Core’s dynamic tables, native gRPC, delta-based propagation, and DAG-driven reactivity. This project implements a gMCP (graph/global MCP) fabric for low-latency, incremental state sharing across agentic systems.

Key concepts:
- Neural Bus: Canonical state fabric built on dynamic tables and delta streaming.
- Agent LTBs (Living Table Bundles): Agents read/write namespaced tables for state, memory, plans.
- Delta+: Efficient, lineage-aware streaming of incremental updates (Arrow + compression).
- Planning + Consensus: Shared planning graph + voting with derived “live” views.
- MCP Bridge: Resources and tools over MCP, backed by the neural bus.

## Getting Started

Prerequisites:
- Python 3.10+
- Deephaven (core + Python APIs)
- grpcio / protobuf
- grpcio-tools (for generating stubs)

Install (dev):
```bash
python -m pip install --upgrade pip
pip install -e .[dev]
```

Generate gRPC stubs:
```bash
make proto
```

Run server (local):
```bash
python scripts/run_server.py
```

Run demo agent:
```bash
python scripts/demo_agent.py
```

## Structure
- `protos/`: Protobuf contracts (common, neural_bus, agent_service, embedding_service).
- `src/next_level_bus/`: Server, core, gRPC impls, MCP bridge.
- `docs/`: Architecture, security, error models.
- `policy/`: Example security policy configuration.
- `.github/workflows/`: CI for proto compilation, linting, typing, and tests.
- `drafts/`: Issue definitions used to create GitHub issues.

## Status
Early bootstrap; interfaces and schemas are stabilized first. See Issues for the implementation roadmap.

## License
Apache-2.0 (see LICENSE).