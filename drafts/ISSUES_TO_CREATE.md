# Issues to Create

1) Bootstrap project scaffolding and Python package
- Labels: enhancement, infra, p1
- Description: Initialize repository structure (src/, protos/, docs/, scripts/), add pyproject.toml, .gitignore, LICENSE, README, ARCHITECTURE docs.
- Acceptance:
  - Project installs via `pip install -e .`
  - Lint/test placeholders run

2) Deephaven session wrapper
- Labels: enhancement, core
- Description: Implement DeephavenSession lifecycle management, connect/start graph, shutdown.
- Acceptance:
  - Connect/shutdown works in local environment
  - Emits health/heartbeat logs

3) TableRegistry and AgentRegistration tables
- Labels: enhancement, core, schema
- Description: Implement registry for tables; define and create `agents.registry` with columns and indexes.
- Acceptance:
  - Register/list APIs tested
  - Snapshot of registry works

4) Define protobuf contracts (common, neural_bus, agent_service, embedding_service)
- Labels: api, schema, p1
- Description: Author and compile protos with CI step; generate stubs for Python (and optionally Java/Go).
- Acceptance:
  - `protos/*.proto` compile successfully
  - Generated stubs used by a sample script

5) DeltaBuilder built from Deephaven update listener
- Labels: core, performance
- Description: Convert DH update cycles to `DeltaEnvelope` with Arrow batches and scalar fallback.
- Acceptance:
  - Unit tests verify added/modified/removed extraction
  - Compressible payload option

6) gRPC server scaffold and Subscribe for single table
- Labels: api, p1
- Description: Implement NeuralBusService.Subscribe for one table subscription, streaming deltas.
- Acceptance:
  - Demo agent receives deltas for a synthetic table
  - Backpressure basic handling

7) AgentService.PushState for working_memory
- Labels: api, core
- Description: Implement per-agent `working_memory` table; allow UPSERT/DELETE mutations; stream deltas.
- Acceptance:
  - Demo agent mutates and receives its own + derived deltas
  - Idempotency via revision_token tested

8) Planning graph schema and propose flow
- Labels: planning, schema
- Description: Create `planning.graph` and derived `planning.graph_live`; implement propose tool.
- Acceptance:
  - Proposals appear with status=PROPOSED
  - Derived view computes initial scores

9) Consensus voting and scoring aggregation
- Labels: consensus, planning
- Description: Implement `consensus.votes`, aggregate confidence, change status → APPROVED/REJECTED.
- Acceptance:
  - Votes adjust scores in `planning.graph_live`
  - Threshold transitions verified

10) MCP bridge resources and tools (list_resources/read_resource)
- Labels: mcp, api
- Description: Expose neuralbus tables as MCP resources; implement basic tools (publish_message, propose, vote).
- Acceptance:
  - MCP client can list and stream a resource
  - Tool calls mutate bus/tables

11) bus.channels management and publish/subscribe
- Labels: messaging, core
- Description: Implement `bus.channels` and `bus.messages.<channel_id>`; add PublishMessage RPC.
- Acceptance:
  - Channel creation/listing works
  - Messages stream to subscribers

12) Embedding pipeline and similarity join
- Labels: embeddings, ml
- Description: Staging table → embedding generation stub → `embeddings.index`; similarity join UDF and query RPCs.
- Acceptance:
  - EnqueueEmbedding + NearestNeighbors working with stub vectors
  - SubscribeNeighbors streams changes

13) Working memory decay and archival
- Labels: memory, lifecycle
- Description: Implement decay scoring and archival to episodic_log; optional embedding capture.
- Acceptance:
  - Decay thresholding works
  - Archival integrity check

14) Observability metrics and lineage
- Labels: observability
- Description: Populate `telemetry.metrics` and `lineage.edges`; export metrics to logs.
- Acceptance:
  - Emit sub_lag_ms, delta_bytes_per_cycle
  - Lineage tokens round-trip

15) Snapshot and replay service
- Labels: reliability
- Description: Periodic Parquet snapshots; Replay RPC from cycle/watermark range.
- Acceptance:
  - Replay reproduces state hash
  - Integration test passes

16) Compression, batching, and backpressure controls
- Labels: performance
- Description: Add ZSTD compression, micro-batching, slow-consumer handling and throttle hints.
- Acceptance:
  - Configurable via env/policy
  - E2E test with artificial slow client

17) Security enforcement (namespaces, RBAC, RLS)
- Labels: security, p1
- Description: Enforce namespace write boundaries; token scopes; row-level tag filters at subscribe-time.
- Acceptance:
  - Unauthorized writes denied
  - Tag filters applied server-side

18) Multi-tenant partitioning and sharding
- Labels: scalability, tenancy
- Description: Workspace prefixes, table partitioning strategy; optional multi-node plan.
- Acceptance:
  - Workspace isolation tests
  - Partition-aware subscriptions

19) CI setup and tests
- Labels: ci, testing
- Description: Linting, unit, integration, load, and fault injection suites with GitHub Actions.
- Acceptance:
  - CI green across jobs
  - Basic load sim passes thresholds

20) Documentation and examples
- Labels: docs
- Description: Expand architecture, API errors, security, examples of clients (Python).
- Acceptance:
  - Docs published and linked
  - Example agent compiles and runs
