from __future__ import annotations

from dataclasses import dataclass
from typing import List, Dict, Any

@dataclass
class DeltaRow:
    scalar_values: Dict[str, str]
    arrow_record_batch: bytes | None = None

@dataclass
class DeltaEnvelope:
    table_name: str
    schema_version: str
    cycle_id: int
    watermark_ts: int
    lineage_token: str
    added: List[DeltaRow]
    removed_keys: List[str]
    modified: List[DeltaRow]
    embedding_sidecar: bytes | None = None
    compression: str = "NONE"

class DeltaBuilder:
    """
    Builds delta envelopes from Deephaven update notifications.
    Placeholder; integrate with Deephaven TableListener in implementation.
    """
    def __init__(self, table_name: str, schema_version: str):
        self.table_name = table_name
        self.schema_version = schema_version

    def from_update(self, update: Any, is_snapshot: bool) -> DeltaEnvelope:
        # TODO: convert DH update -> Arrow + scalar rows
        return DeltaEnvelope(
            table_name=self.table_name,
            schema_version=self.schema_version,
            cycle_id=0,
            watermark_ts=0,
            lineage_token="",
            added=[],
            removed_keys=[],
            modified=[],
            embedding_sidecar=None,
            compression="NONE",
        )