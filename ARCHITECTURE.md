# Architecture

This project implements a neural-bus framework on top of Deephaven Core:

- Physical Transport: gRPC bidi streams for table deltas and control.
- Data Fabric: Dynamic tables (state, memory, planning, messages) with DAG reactivity.
- Delta+: Row-level diffs with lineage tokens and schema versioning.
- Agent Interface: Namespaced tables per agent for isolation and coordination.
- Cross-Agent Mediation: Planning graph + voting → derived “live” plans.
- Persistence & Replay: Snapshots and streaming replays by cycle/watermark.
- Security: Token-scoped table access, row-level filtering, namespace enforcement.
- Observability: Lineage, metrics, backpressure signals.

See `docs/SECURITY_MODEL.md` and `docs/API_ERRORS.md` for detailed models.

## Core Tables

- `agents.registry(agent_id, type, capabilities, version, status, heartbeat)`
- `agents.state.<agent_id>.{core,working_memory,episodic_log,tool_invocations}`
- `bus.channels(channel_id, topic, schema_signature, retention_policy, subscribers)`
- `bus.messages.<channel_id>(msg_id, sender_agent_id, payload, ts, tags, embedding)`
- `planning.graph(node_id, node_type, parent_id, owner_agent_id, status, score)`
- `consensus.votes(proposal_id, agent_id, vote, confidence, ts)`
- `embeddings.index(reference_id, space, vector, origin_table, ts)`
- `lineage.edges(source_table, target_table, transform_type, latency_ms, ts)`
- `telemetry.metrics(metric_name, agent_id, value, ts, window)`

## Delta Propagation (Delta+)

- Envelope includes: schema_version, cycle_id, watermark_ts, lineage_token.
- Payload supports Arrow batches + optional ZSTD compression.
- Optional embedding sidecar for vector-heavy streams.
- Server-side projection: columns, semantic tags, filters, time-slicing.

## Planning & Consensus

- Agents propose tasks into `planning.graph`.
- Votes in `consensus.votes`; derived view aggregates confidence → status updates.
- Agents subscribe to `planning.graph_live` to adapt in real time.

## Embeddings

- Staging requests → embedding pipeline → `embeddings.index`.
- Similarity joins via UDF/callouts; streaming top-K subscriptions.

## MCP Bridge

- Resources: `mcp://neuralbus/tables/<name>`, `mcp://agent/<id>/memory`, etc.
- Tools: publish_message, push_state, propose_plan, vote_proposal, query_similarity.

## Performance

- Columnar Arrow + ZSTD; micro-batching; per-subscriber backpressure; sharding.
- Metrics: delta_bytes_per_cycle, sub_lag_ms, mutation_rate, convergence_time.

## Reliability

- Periodic Parquet snapshots; delta logs; replay cursor by cycle/watermark.
- Idempotent mutations with revision tokens; schema evolution with mappers.

## Multi-Tenancy & Security

- Namespace isolation: `agents.state.<agent_id>` is agent-private by default.
- Row-level filters by tags, policies, and token scopes.
- AuthN: mTLS and/or JWT; AuthZ via capability-based policies.