"""Module for ResourceInfo class"""

import ast
from dataclasses import dataclass
from typing import Any, Dict, List, Optional, Tuple


@dataclass
class ResourceInfo:
    """Hold information about resource needed for transpilation"""

    name: str
    qubits: int
    online: bool = False
    connectivity: Optional[List[List[int]]] = None
    instructions: Optional[List[Tuple[str, Optional[Dict[Any, Any]]]]] = None

    def __eq__(self, other) -> bool:
        return isinstance(other, ResourceInfo) and self.qubits == other.qubits

    @classmethod
    def from_json_dict(cls, resource_json: dict):
        """Return ResourceInfo object from json string"""

        if "name" not in resource_json:
            raise ValueError("Resource name not found")
        _name = resource_json["name"]

        _online = False
        _connectivity = None
        _instructions = None

        if "online" in resource_json:
            _online = resource_json["online"]
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
            name=_name,
            qubits=resource_json["qubits"],
            online=_online,
            connectivity=_connectivity,
            instructions=_instructions,
        )
