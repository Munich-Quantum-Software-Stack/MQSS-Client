"""Job module for MQSS Client"""

from abc import ABC
from dataclasses import dataclass
from datetime import datetime
from enum import Enum
from typing import Dict, Optional


class JobStatus(str, Enum):
    """Status enumeration for the job status"""

    PENDING = "PENDING"
    # NOTE: We need an intermediate status before COMPLETED for the job runner to work
    WAITING = "WAITING"
    FAILED = "FAILED"
    COMPLETED = "COMPLETED"
    CANCELLED = "CANCELLED"


class JobRequest(ABC):  # pylint: disable=too-few-public-methods
    """Base class for job requests"""

    def to_json_dict(self) -> dict:
        """Convert JobRequest to JSON dictionary"""
        raise NotImplementedError("Subclasses must implement this method")


@dataclass
class CircuitJobRequest(JobRequest):
    """Class to hold job information"""

    circuits: str
    circuit_format: str
    resource_name: str
    shots: int
    no_modify: bool
    queued: bool

    def to_json_dict(self) -> dict:
        """Convert CircuitJobRequest to JSON dictionary"""
        return {
            "circuit": self.circuits,
            "circuit_format": self.circuit_format,
            "resource_name": self.resource_name,
            "shots": self.shots,
            "no_modify": self.no_modify,
            "queued": self.queued,
        }


@dataclass
class HamiltonianJobRequest(JobRequest):
    """Class to hold Hamiltonian job information"""

    resource_name: str
    interaction_str: str
    coefficients_str: str

    def to_json_dict(self) -> dict:
        """Convert HamiltonianJobRequest to JSON dictionary"""
        return {
            "resource_name": self.resource_name,
            "interaction_str": self.interaction_str,
            "coefficients_str": self.coefficients_str,
        }


@dataclass
class Result:
    """Result Class to hold counts"""

    counts: Dict[str, int]
    timestamp_submitted: datetime
    timestamp_scheduled: datetime
    timestamp_completed: Optional[datetime] = None
