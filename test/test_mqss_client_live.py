import pytest

from mqss_client.mqss_client import MQSSClient

from .config import CURRENT_RESOURCES, TOKEN, URL
from .mqss_client_tests_base import (
    BaseMQSSClientTests,
    circuit_job_request,
    hamiltonian_job_request,
    resource_name,
)

__all__ = ["circuit_job_request", "hamiltonian_job_request", "resource_name"]


def online_resource_name(client):
    """Get the name of an online resource"""
    online_resources_names = [
        res
        for res in list(CURRENT_RESOURCES.keys())
        if client.get_resource_info(res).online
    ]
    return online_resources_names[0] if online_resources_names else None


@pytest.mark.live
@pytest.mark.parametrize(
    "client_type",
    [
        "MQP",
    ],
    ids=["MQP"],
)
class TestMQSSClientLive(BaseMQSSClientTests):
    """Test class for MQSSClient with live REST and HPC clients."""

    @pytest.fixture(autouse=True)
    def setup_client(self, client_type):
        """Setup the proper client based on the parameterized client type."""
        self.client_type = client_type

        yield

    @pytest.fixture
    def client_args(self) -> tuple:
        """Fixture to provide client arguments for tests."""
        return TOKEN, URL, "HPC" in self.client_type

    def test_submit_circuit_job(self, client: MQSSClient, circuit_job_request) -> None:
        """Test submitting a circuit job."""
        circuit_job_request.resource_name = online_resource_name(client)
        return super().test_submit_circuit_job(client, circuit_job_request)

    def test_submit_hamiltonian_job(
        self, client: MQSSClient, hamiltonian_job_request
    ) -> None:
        """Test submitting a Hamiltonian job."""
        hamiltonian_job_request.resource_name = online_resource_name(client)
        return super().test_submit_hamiltonian_job(client, hamiltonian_job_request)

    def test_submit_pennylane_job(
        self, client: MQSSClient, pennylane_job_request
    ) -> None:
        """Test submitting a Pennylane job."""
        pennylane_job_request.resource_name = online_resource_name(client)
        return super().test_submit_pennylane_job(client, pennylane_job_request)

    def test_job_status(
        self, client: MQSSClient, circuit_job_request, monkeypatch
    ) -> None:
        """Test job status."""
        circuit_job_request.resource_name = online_resource_name(client)
        return super().test_job_status(client, circuit_job_request, monkeypatch)

    def test_wait_for_job_result(
        self, client: MQSSClient, circuit_job_request, monkeypatch
    ) -> None:
        """Test waiting for job result."""
        circuit_job_request.resource_name = online_resource_name(client)
        return super().test_wait_for_job_result(
            client, circuit_job_request, monkeypatch
        )
