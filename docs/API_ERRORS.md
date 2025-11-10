# API Errors and Status Model

All RPCs may return structured errors with machine-parsable codes and retry hints.

## StatusCode (subset)
- OK
- INVALID_ARGUMENT
- NOT_FOUND
- ALREADY_EXISTS
- PERMISSION_DENIED
- UNAUTHENTICATED
- RESOURCE_EXHAUSTED
- FAILED_PRECONDITION
- ABORTED
- OUT_OF_RANGE
- UNIMPLEMENTED
- INTERNAL
- UNAVAILABLE
- DATA_LOSS
- DEADLINE_EXCEEDED
- SCHEMA_MISMATCH
- SUBSCRIPTION_CONFLICT
- CONFLICT
- RATE_LIMITED
- BACKPRESSURE

## Error Schema
- `code`: StatusCode
- `message`: Human-readable message
- `correlation_id`: For tracing
- `details`: List of typed payloads (JSON blobs, field violations, etc.)
- `retry_after_ms` (hint)
- `throttle_ms` (hint)
- `quota_remaining` (hint)
- `lineage_token` (when applicable)
- `schema_version` (when SCHEMA_MISMATCH)

Examples:
- SCHEMA_MISMATCH on Subscribe with wrong column projections.
- SUBSCRIPTION_CONFLICT when trying to double-subscribe with incompatible filters.
- BACKPRESSURE when stream queues exceed limits; include `throttle_ms`.