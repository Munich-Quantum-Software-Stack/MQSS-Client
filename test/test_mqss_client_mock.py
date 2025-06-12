import time

import pytest

from mqss_client import JobStatus, MQSSClient, Result

from .mocks import patch_mqss_rest_client, patch_rabbitmq_client
from .mqss_client_tests_base import (
    BaseMQSSClientTests,
    circuit_job_request,
    hamiltonian_job_request,
    resource_name,
)

__all__ = ["circuit_job_request", "hamiltonian_job_request", "resource_name"]


@pytest.mark.mock
@pytest.mark.parametrize(
    "client_type",
    [
        "MQP",
        "HPC",
    ],
    ids=["MQP", "HPC"],
)
class TestMQSSClientMock(BaseMQSSClientTests):
    """Test class for MQSSClient with mocked REST and HPC clients."""

    @pytest.fixture(autouse=True)
    def setup_client(self, client_type):
        """Setup the proper client patch based on the parameterized client type."""
        self.client_type = client_type

        # Start patch based on client type
        if "HPC" in client_type:
            self.patch = patch_rabbitmq_client()
        else:
            self.patch = patch_mqss_rest_client()
        self.patch.start()

        yield

        # Stop the patch after test
        self.patch.stop()

    @pytest.fixture
    def client_args(self) -> tuple:
        return "mock-token", "http://mock-url", "HPC" in self.client_type

    def test_job_status(
        self, client: MQSSClient, circuit_job_request, monkeypatch
    ) -> None:
        """Test job status."""
        job_id = client.submit_job(circuit_job_request)

        # First mock to ensure initial status is PENDING
        def mock_initial_status(job_id, job_request):
            """Mock job status to return PENDING initially."""
            return JobStatus.PENDING

        monkeypatch.setattr(client, "job_status", mock_initial_status)
        status = client.job_status(job_id, circuit_job_request)
        assert status in [JobStatus.PENDING, JobStatus.WAITING]

        def mock_job_status(job_id, job_request):
            """Mock job status to return CANCELLED."""
            return JobStatus.CANCELLED

        monkeypatch.setattr(client, "job_status", mock_job_status)
        client.cancel_job(job_id, circuit_job_request)
        status = client.job_status(job_id, circuit_job_request)
        assert status == JobStatus.CANCELLED

    def test_wait_for_job_result(
        self, client: MQSSClient, circuit_job_request, monkeypatch
    ) -> None:
        """Test waiting for job result."""
        job_id = client.submit_job(circuit_job_request)

        def mock_job_status(job_id, job_request):
            """Mock job status to return a predefined status."""
            return JobStatus.COMPLETED



        monkeypatch.setattr(client, "job_status", mock_job_status)
        monkeypatch.setattr(time, "sleep", lambda x: None)

        result = client.wait_for_job_result(job_id, circuit_job_request)
        assert result is not None
        assert isinstance(result, Result)
        assert result.counts == {"00": 500, "11": 500}
