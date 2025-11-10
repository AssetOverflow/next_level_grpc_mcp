# Security Model

This document outlines threat model, controls, and policy constructs for the neural-bus.

## Threat Model
- Network: MITM, replay, tampering
- Auth: Token theft, confused deputy
- DoS: Resource exhaustion, slow consumers
- Data: Cross-tenant leakage, schema exfiltration, PII mishandling
- Integrity: Multi-writer conflicts, stale lineage replays
- Execution: UDF sandbox breakout

## Controls

### Authentication
- mTLS for intra-cluster server-to-server communication
- JWT/OIDC for agents/clients; short-lived tokens; audience-bound
- Optional HMAC signature on delta envelopes for integrity

### Authorization
- Namespace isolation: Agents can only mutate `agents.state.<agent_id>.*`
- Capability-based RBAC:
  - Scopes: `neuralbus.read:<table>`, `neuralbus.write:<table>`, `planning.propose`, `planning.vote`, `embedding.query:<space>`
  - Policies bound to token claims (agent_id, workspace_id, capabilities)
- Row-Level Security:
  - Semantic tag filters enforced at subscription time
  - Workspace ID column-based filtering for multi-tenancy

### Rate Limiting & Backpressure
- Per-agent and per-connection quotas (QPS, throughput)
- Server-enforced micro-batching and drop policies for slow consumers
- Errors include `BACKPRESSURE` and `RATE_LIMITED` with `retry_after_ms`

### Data Handling
- PII/Secret classification tags on columns/rows
- Egress filter policies preventing outbound of restricted tags
- Snapshot encryption at rest; key rotation via KMS
- No secrets stored in tables; secrets fetched via secure vault during runtime

### Integrity & Recovery
- Idempotent mutations with revision tokens and cycle IDs
- Signed lineage tokens prevent replay tampering
- Snapshots + delta logs; replay cursors validated

### UDF Safety
- UDF execution in sandboxed runtime (separate process/pod)
- Resource limits for CPU/memory/time
- Approved libraries allowlist

## Policy Model

Policies are defined in a YAML file and loaded at startup/hot-reload:
- Principals: agents, workspaces, service accounts
- Resources: tables, channels, tools
- Actions: read, write, subscribe, publish, propose, vote
- Conditions: tag filters, time windows, quotas
- Effects: allow/deny

See `policy/policy.example.yaml` for a reference configuration.