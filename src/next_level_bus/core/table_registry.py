from __future__ import annotations

from dataclasses import dataclass, field
from typing import Dict, Any, Optional

@dataclass
class TableHandle:
    name: str
    schema_version: str
    metadata: Dict[str, Any] = field(default_factory=dict)

class TableRegistry:
    """
    Resolves logical table refs to concrete Deephaven tables and their schemas.
    """
    def __init__(self):
        self._tables: Dict[str, TableHandle] = {}

    def register(self, name: str, schema_version: str, metadata: Optional[Dict[str, Any]] = None) -> TableHandle:
        handle = TableHandle(name=name, schema_version=schema_version, metadata=metadata or {})
        self._tables[name] = handle
        return handle

    def get(self, name: str) -> Optional[TableHandle]:
        return self._tables.get(name)

    def list(self, prefix: str = "") -> Dict[str, TableHandle]:
        return {k: v for k, v in self._tables.items() if k.startswith(prefix)}