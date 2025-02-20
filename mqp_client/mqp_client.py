"""MQP REST API Client"""

import json
import time
from dataclasses import dataclass
from datetime import datetime
from enum import Enum
from typing import Dict, Optional

import requests  # type: ignore

from .base_client import BaseClient
from .resource_info import ResourceInfo


class JobStatus(str, Enum):
    """Status enumeration for thr job status"""

    PENDING = "PENDING"
    # NOTE: We need an intermediate status before COMPLETED for the job runner to work
    WAITING = "WAITING"
    FAILED = "FAILED"
    COMPLETED = "COMPLETED"
    CANCELLED = "CANCELLED"


@dataclass
class Result:
    """Result Class to hold counts"""

    counts: Dict[str, int]
    timestamp_submitted: datetime
    timestamp_scheduled: datetime
    timestamp_completed: Optional[datetime] = None


class MQPClient(BaseClient):
    """MQP REST API Client class"""

    # pylint: disable=fixme
    # TODO Deprecate in future version
    # pylint: enable=fixme
    def resources(self) -> Optional[Dict[str, ResourceInfo]]:
        """Get resource info about all resources"""

        return self.get_all_resources()

    def get_all_resources(self) -> Optional[Dict[str, ResourceInfo]]:
        """Get resource info about all resources"""
        try:
            rsp_json = self._get("resources")
        except (
            requests.exceptions.RequestException,
            requests.exceptions.JSONDecodeError,
        ):
            return None
        _resources = {}
        for name, resource_json in rsp_json.items():
            _resources[name] = ResourceInfo.from_json_dict(resource_json)

        return _resources

    # pylint: disable=fixme
    # TODO Deprecate in future version
    # pylint: enable=fixme
    def resource_info(self, resource_name: str) -> Optional[ResourceInfo]:
        """Get resource info about specific resource"""

        return self.get_resource_info(resource_name)

    def get_resource_info(self, resource_name: str) -> Optional[ResourceInfo]:
        """Get resource info about specific resource"""
        try:
            rsp_json = self._get(f"resources/{resource_name}")
        except (
            requests.exceptions.RequestException,
            requests.exceptions.JSONDecodeError,
        ):
            return None

        return ResourceInfo.from_json_dict(rsp_json)

    # pylint: disable=too-many-arguments
    def submit_job(
        self,
        resource_name: str,
        circuit: str,
        circuit_format: str,
        shots: int,
        no_modify: bool = False,
        queued: bool = False,
    ) -> str:
        """Submit a circuit job to BQP API"""
        rsp_json = self._post(
            "job",
            {
                "resource_name": resource_name,
                "circuit": circuit,
                "circuit_format": circuit_format,
                "shots": shots,
                "no_modify": no_modify,
                "queued": queued,
            },
        )
        return rsp_json["uuid"]

    def submit_hamiltonian_job(
        self, resource_name: str, interaction_str: str, coefficients_str: str
    ) -> str:
        """Submit a Hamiltonian Job to BQP API"""
        rsp_json = self._post(
            "hamiltonian_job",
            {
                "resource_name": resource_name,
                "interaction_str": interaction_str,
                "coefficients_str": coefficients_str,
            },
        )
        return rsp_json["uuid"]

    def cancel(self, uuid: str, hamiltonian_job: bool = False) -> None:
        """Cancel a job"""
        table_name = "job"
        if hamiltonian_job:
            table_name = "hamiltonian_job"
        self._delete(f"{table_name}/{uuid}")

    def status(self, uuid: str, hamiltonian_job: bool = False) -> JobStatus:
        """Get job status"""
        table_name = "job"
        if hamiltonian_job:
            table_name = "hamiltonian_job"
        rsp_json = self._get(f"{table_name}/{uuid}/status")
        return JobStatus(rsp_json["status"])

    def result(self, uuid: str, hamiltonian_job: bool = False) -> Optional[Result]:
        """Retrieve results of a job if completed otherwise return None"""
        table_name = "job"
        if hamiltonian_job:
            table_name = "hamiltonian_job"

        result_json = self._get(f"{table_name}/{uuid}/result")
        if result_json["result"] in {None, "None", ""}:
            return None
        return Result(
            counts=json.loads(result_json["result"]),
            # timestamp_completed=datetime.strptime(
            #     result_json.get(
            #         "timestamp_completed",
            #         datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f"),
            #     ),
            #     "%Y-%m-%d %H:%M:%S.%f",
            # ),
            timestamp_completed=(
                datetime.strptime(
                    result_json["timestamp_completed"], "%Y-%m-%d %H:%M:%S.%f"
                )
                if result_json["timestamp_completed"] != ""
                else None
            ),
            timestamp_submitted=datetime.strptime(
                result_json["timestamp_submitted"], "%Y-%m-%d %H:%M:%S.%f"
            ),
            timestamp_scheduled=datetime.strptime(
                result_json["timestamp_scheduled"], "%Y-%m-%d %H:%M:%S.%f"
            ),
        )

    def wait_for_result(
        self, uuid: str, timeout_interval: float = 2.0, hamiltonian_job=False
    ) -> Result:
        """Wait for the results to finish and get the results"""
        end_status = self.status(uuid, hamiltonian_job)
        while end_status in (JobStatus.PENDING, JobStatus.WAITING):
            time.sleep(timeout_interval)
            end_status = self.status(uuid, hamiltonian_job)
        if end_status == JobStatus.COMPLETED:
            # NOTE: Unpacking this dict includes the key in the result (JE)
            # return Result(**rsp_json)

            # NOTE: This way I get only the results (JE)
            _result = self.result(uuid, hamiltonian_job)
            assert _result is not None
            return _result
        if end_status == JobStatus.FAILED:
            raise RuntimeError("Job failed")

        if end_status == JobStatus.CANCELLED:
            table_name = "job"
            if hamiltonian_job:
                table_name = "hamiltonian_job"
            rsp_json = self._get(f"{table_name}/{uuid}/cancel_reason")
            rsp_reason = rsp_json["cancel_reason"]
            raise RuntimeError(f"Job cancelled: {rsp_reason}")

        raise RuntimeError("Unknown status")

    def get_device_num_pending_jobs(self, resource_name: str) -> int:
        """get number of queued jobs on the device"""
        rsp_json = self._get(f"resources/{resource_name}/num_pending_jobs")
        return rsp_json["num_pending_jobs"]
