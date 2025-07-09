"""
This module provides a client for the MQP API and HPC Offload Listener.
"""

import json
import os
import socket
import time
from datetime import datetime
from typing import Dict, Optional, Union

from .hpc_client import HPCOffloadClient
from .job import CircuitJobRequest, HamiltonianJobRequest, JobRequest, JobStatus, Result
from .resource_info import ResourceInfo
from .rest_client import RESTClient


class MQSSClient:
    """MQSS Client class for interacting with MQP REST API / HPC Offload Listener"""

    def __init__(self, token: str, base_url: str, is_hpc: bool = False):
        self.token = token
        self.base_url = base_url
        self.client: Union[HPCOffloadClient, RESTClient]
        if is_hpc:
            HOSTNAME = socket.gethostname().replace(" ", "_")
            _offload_queue_name = os.environ.get(
                "MQSS_OFFLOAD_LISTENER_QUEUE_NAME",
                f"qoffload_api_request_reception_queue_{HOSTNAME}",
            )
            self.client = HPCOffloadClient(
                token, offload_listener_queue_name=_offload_queue_name
            )
        else:
            self.client = RESTClient(token, base_url)

    def get_all_resources(self) -> Dict[str, ResourceInfo]:
        """Get resource info about all resources"""

        _resources = {}
        rsp_json = self.client.get("resources")
        for name, resource_json in rsp_json.items():
            _resources[name] = ResourceInfo.from_json_dict(resource_json)

        return _resources

    def get_resource_info(self, resource_name: str) -> Optional[ResourceInfo]:
        """Get resource info about specific resource"""
        rsp_json = self.client.get(f"resources/{resource_name}")

        if not rsp_json:
            return None

        return ResourceInfo.from_json_dict(rsp_json)

    def submit_job(self, job_request: JobRequest) -> str:
        """Submit a circuit job"""
        if isinstance(job_request, CircuitJobRequest):
            rsp_json = self.client.post(
                "job",
                job_request.to_json_dict(),
            )
        elif isinstance(job_request, HamiltonianJobRequest):
            rsp_json = self.client.post(
                "hamiltonian_job",
                job_request.to_json_dict(),
            )
        else:
            raise ValueError("Invalid job request type")

        if not rsp_json:
            raise ValueError("Invalid response from server")

        if "uuid" not in rsp_json:
            raise ValueError("UUID not found in response")

        return rsp_json["uuid"]

    def cancel_job(self, uuid: str, job_type: JobRequest) -> None:
        """Cancel a job"""
        if isinstance(job_type, CircuitJobRequest):
            self.client.delete(f"job/{uuid}")
        elif isinstance(job_type, HamiltonianJobRequest):
            self.client.delete(f"hamiltonian_job/{uuid}")
        else:
            raise ValueError("Invalid job request type")

    def job_status(self, uuid: str, job_type: JobRequest) -> JobStatus:
        """Get job status"""
        if isinstance(job_type, CircuitJobRequest):
            rsp_json = self.client.get(f"job/{uuid}/status")
        elif isinstance(job_type, HamiltonianJobRequest):
            rsp_json = self.client.get(f"hamiltonian_job/{uuid}/status")
        else:
            raise ValueError("Invalid job request type")

        if not rsp_json:
            raise ValueError("Invalid response from server")
        if "status" not in rsp_json:
            raise ValueError("Status not found in response")

        return JobStatus(rsp_json["status"])

    def job_result(self, uuid: str, job_type: JobRequest) -> Optional[Result]:
        """Get job result as JSON"""
        if isinstance(job_type, CircuitJobRequest):
            result_json = self.client.get(f"job/{uuid}/result")
        elif isinstance(job_type, HamiltonianJobRequest):
            result_json = self.client.get(f"hamiltonian_job/{uuid}/result")
        else:
            raise ValueError("Invalid job request type")

        if not result_json:
            return None

        return Result(
            counts=json.loads(result_json["result"]),
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

    def wait_for_job_result(self, uuid: str, job_type: JobRequest) -> Optional[Result]:
        """Wait for a job to complete and return the result"""
        timeout = 2.0
        end_status = self.job_status(uuid, job_type)
        while end_status in (JobStatus.PENDING, JobStatus.WAITING):
            time.sleep(timeout)
            end_status = self.job_status(uuid, job_type)

        if end_status == JobStatus.COMPLETED:
            return self.job_result(uuid, job_type)
        if end_status == JobStatus.FAILED:
            raise RuntimeError("Job failed")
        if end_status == JobStatus.CANCELLED:
            if isinstance(job_type, CircuitJobRequest):
                cancel_reason = self.client.get(f"job/{uuid}/cancel_reason")
            else:
                cancel_reason = self.client.get(f"hamiltonian_job/{uuid}/cancel_reason")
            raise RuntimeError(f"Job cancelled: {cancel_reason}")

        raise RuntimeError("Unknown status")

    def get_num_pending_jobs(self, resource_name: str) -> int:
        """Get the number of pending jobs for a resource"""
        rsp_json = self.client.get(f"resources/{resource_name}/num_pending_jobs")

        if not rsp_json or "num_pending_jobs" not in rsp_json:
            raise ValueError("Invalid response from server")

        return rsp_json["num_pending_jobs"]
