"""Test module for Pennylane job submission with mocked RabbitMQ."""

import json

import pytest

from mqss_client import JobStatus, MQSSClient, Result
from mqss_client.job import CircuitJobRequest

from .config import get_qasm
from .mocks import MOCK_JOB_DATA, patch_mqss_rest_client, patch_rabbitmq_client

# Add Pennylane mock data to existing mock data
MOCK_JOB_DATA.update(
    {
        "job": {"jobs": ["mock-uuid-12345"]},
        "job/mock-uuid-12345/status": {"status": "COMPLETED"},
        "job/mock-uuid-12345/result": {
            "result": '{"00": 500, "11": 500}',
            "timestamp_completed": "2023-04-14 10:15:30.123456",
            "timestamp_submitted": "2023-04-14 10:00:00.123456",
            "timestamp_scheduled": "2023-04-14 10:05:00.123456",
        },
    }
)


@pytest.fixture
def pennylane_job_request():
    """Create a mock Pennylane job request using QASM format."""
    return CircuitJobRequest(
        resource_name="mock-backend",
        circuits=get_qasm(),  # Using real QASM circuit definition, later we will use MLIR circuit definition
        circuit_format="qasm",  # Using QASM format
        shots=1000,
        no_modify=True,
        queued=False,
    )


@pytest.mark.mock
@pytest.mark.parametrize(
    "client_type",
    [
        "MQP",
        "HPC",
    ],
    ids=["MQP", "HPC"],
)
class TestPennylaneJobMock:
    """Test class for Pennylane job submission with mocked clients."""

    @pytest.fixture(autouse=True)
    def setup_client(self, client_type):
        """Setup the proper client patch based on the parameterized client type."""
        self.client_type = client_type

        # Start patch based on client type
        if "HPC" in client_type:
            self.patch = patch_rabbitmq_client()
            self.mock_rmq = self.patch.start()
        else:
            self.patch = patch_mqss_rest_client()
            self.patch.start()

        yield

        # Stop the patch after test
        self.patch.stop()

    @pytest.fixture
    def client_args(self) -> tuple:
        return "mock-token", "http://mock-url", "HPC" in self.client_type

    @pytest.fixture
    def client(self, client_args) -> MQSSClient:
        """Create a mock MQSS client."""
        token, base_url, is_hpc = client_args
        return MQSSClient(token, base_url, is_hpc)

    def test_pennylane_job_submission(self, client: MQSSClient, pennylane_job_request):
        """Test submitting a Pennylane job."""
        # Submit the job
        job_id = client.submit_job(pennylane_job_request)

        # Check status
        status = client.job_status(job_id, pennylane_job_request)
        assert status == JobStatus.COMPLETED

        # Get result
        result = client.job_result(job_id, pennylane_job_request)
        assert isinstance(result, Result)
        assert result.counts == {"00": 500, "11": 500}

    def test_pennylane_job_cancellation(
        self, client: MQSSClient, pennylane_job_request
    ):
        """Test cancelling a Pennylane job."""
        # Submit the job
        job_id = client.submit_job(pennylane_job_request)

        # Cancel the job
        client.cancel_job(job_id, pennylane_job_request)

        # Verify the cancel request was made
        if "HPC" in self.client_type:
            # For HPC client, verify that a message was sent with the delete request
            assert self.mock_rmq.return_value.last_message is not None
            message_data = json.loads(self.mock_rmq.return_value.last_message)
            assert message_data.get("request") == f"job/{job_id}"
            assert message_data.get("method") == "DELETE"
        else:
            # For MQP client, verify that delete was called with the correct path
            client.client.delete.assert_called_with(f"job/{job_id}")  # type: ignore[attr-defined]

    def test_pennylane_job_wait_for_result(
        self, client: MQSSClient, pennylane_job_request, monkeypatch
    ):
        """Test waiting for a Pennylane job result."""
        # Submit the job
        job_id = client.submit_job(pennylane_job_request)

        # Mock sleep to speed up test
        monkeypatch.setattr("time.sleep", lambda x: None)

        # Wait for result
        result = client.wait_for_job_result(job_id, pennylane_job_request)

        # Verify result
        assert isinstance(result, Result)
        assert result.counts == {"00": 500, "11": 500}
