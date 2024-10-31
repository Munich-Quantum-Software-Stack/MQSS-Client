"""Module for ResourceInfo class"""

import ast
from dataclasses import dataclass
from typing import Any, Dict, List, Optional, Tuple


@dataclass
class ResourceInfo:
    """Hold information about resource needed for transpilation"""

    qubits: int
    connectivity: Optional[List[List[int]]]
    instructions: Optional[List[Tuple[str, Optional[Dict[Any, Any]]]]]

    def __eq__(self, other) -> bool:
        return isinstance(other, ResourceInfo) and self.qubits == other.qubits

    @classmethod
    def from_json_dict(cls, resource_json: dict):
        """Return ResourceInfo object from json string"""
        _connectivity = None
        _instructions = None
        try:
            _connectivity = ast.literal_eval(resource_json["connectivity"])
        except (
            ValueError,
            TypeError,
            SyntaxError,
            MemoryError,
            RecursionError,
            KeyError,
        ):
            pass
        try:
            _instructions = ast.literal_eval(resource_json["instructions"])
        except (
            ValueError,
            TypeError,
            SyntaxError,
            MemoryError,
            RecursionError,
            KeyError,
        ):
            pass

        return cls(
            qubits=resource_json["qubits"],
            connectivity=_connectivity,
            instructions=_instructions,
        )
