"""Client for HPCQC Offloading
It needs to interact with the following API endpoints:
- `resources`: Get information about all resources.
- `resources/{resource_name}`: Get information about a specific resource.
- `resources/{resource_name}/num_pending_jobs`: Get the number of pending jobs for a resource.
- `job`: Submit a job.
- `job/{uuid}/status`: Get the status of a job.
- `job/{uuid}/result`: Get the result of a job.
- `job/{uuid}/cancel_reason`: Get the cancel reason of a job.
- `hamiltonian_job`: Submit a Hamiltonian job.
- `hamiltonian_job/{uuid}/status`: Get the status of a Hamiltonian job.
- `hamiltonian_job/{uuid}/result`: Get the result of a Hamiltonian job.
- `hamiltonian_job/{uuid}/cancel_reason`: Get the cancel reason of a Hamiltonian job.
The client can be initialized with a token and a base URL.
It supports both REST and HPC Offload modes.
"""

import json
import socket
import uuid
from dataclasses import dataclass

from .base_client import BaseClient
from .comm.rmq_client import RabbitMQClient

HOSTNAME = socket.gethostname().replace(" ", "_")


class HPCOffloadClient(BaseClient):
    """HPC Client for HPC Offload Listener"""

    def __init__(
        self,
        token: str,
        offload_listener_queue_name: str = f"qoffload_api_request_reception_queue_{HOSTNAME}",
    ) -> None:
        """Initialize the HPC Offload Client"""
        self.token = token
        self.offload_listener_queue_name = offload_listener_queue_name
        unique_id = str(uuid.uuid4())[:8]
        self.response_queue_name = f"response_queue_{HOSTNAME}_{unique_id}"

    def get(self, path: str) -> dict:
        """GET request to the specified path"""
        # Implement GET request logic here
        with RabbitMQClient(self.token) as client:
            request = OffloadRequest(
                authorization=self.token,
                method="GET",
                request=path,
                data={},
                response_queue=self.response_queue_name,
            )
            client.declare_queue(self.response_queue_name)
            client.send(request.request_str(), self.offload_listener_queue_name)
            response = client.receive(request.response_queue)
            client.delete_queue(self.response_queue_name)
            if response is not None:
                return json.loads(response)
        return {}

    def post(self, path: str, data: dict) -> dict:
        """POST request to the specified path"""
        # Implement POST request logic here
        with RabbitMQClient(self.token) as client:
            request = OffloadRequest(
                authorization=self.token,
                method="POST",
                request=path,
                data=data,
                response_queue=self.response_queue_name,
            )
            client.declare_queue(self.response_queue_name)
            client.send(request.request_str(), self.offload_listener_queue_name)
            response = client.receive(request.response_queue)
            client.delete_queue(self.response_queue_name)
            if response is not None:
                return json.loads(response)
        return {}

    def delete(self, path: str) -> None:
        """DELETE request to the specified path"""
        # Implement DELETE request logic here
        with RabbitMQClient(self.token) as client:
            request = OffloadRequest(
                authorization=self.token,
                method="DELETE",
                request=path,
                data={},
                response_queue="",
            )
            client.send(request.request_str(), self.offload_listener_queue_name)


@dataclass
class OffloadRequest:
    """Data class for offload requests"""

    authorization: str
    method: str
    request: str
    data: dict
    response_queue: str

    def request_str(self) -> str:
        """Convert the request to a JSON string"""
        return json.dumps(self.__dict__)
