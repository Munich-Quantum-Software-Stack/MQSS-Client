import pytest

from mqss_client import (
    CircuitJobRequest,
    HamiltonianJobRequest,
    JobStatus,
    MQSSClient,
    PennylaneJobRequest,
    ResourceInfo,
    Result,
)

from .config import CURRENT_RESOURCES, TOKEN, URL, get_qasm


@pytest.fixture
def resource_name():
    """Fixture to provide a valid resource name for tests"""
    return list(CURRENT_RESOURCES.keys())[0]


@pytest.fixture
def circuit_job_request(resource_name):
    """Fixture to create a circuit job request"""
    return CircuitJobRequest(
        circuits=get_qasm(),
        circuit_format="qasm",
        resource_name=resource_name,
        shots=1000,
        no_modify=False,
        queued=False,
    )


@pytest.fixture
def hamiltonian_job_request(resource_name):
    """Fixture to create a hamiltonian job request"""
    # Simple example Hamiltonian
    interaction_str = "Z0 Z1\nX0 X1"
    coefficients_str = "0.5\n0.3"
    return HamiltonianJobRequest(
        resource_name=resource_name,
        interaction_str=interaction_str,
        coefficients_str=coefficients_str,
    )


@pytest.fixture
def pennylane_job_request(resource_name):
    """Fixture to create a pennylane job request"""
    return PennylaneJobRequest(
        resource_name=resource_name,
        circuits=get_qasm(),
        circuit_format="qasm",
        shots=1000,
        no_modify=False,
    )


class BaseMQSSClientTests:
    """Base class for MQSS client tests."""

    @pytest.fixture
    def client_args(self) -> tuple:
        """Fixture to provide client arguments for tests."""
        return TOKEN, URL, False

    @pytest.fixture
    def client(self, client_args) -> MQSSClient:
        """Get the MQSS client instance."""
        return MQSSClient(
            token=client_args[0], base_url=client_args[1], is_hpc=client_args[2]
        )

    def test_get_all_resources(self, client: MQSSClient) -> None:
        """Test getting all resources."""
        resources = client.get_all_resources()
        assert isinstance(resources, dict)
        assert len(resources) > 0
        assert all(
            isinstance(resource_info, ResourceInfo)
            for resource_info in resources.values()
        )
        assert any(
            resource_name in list(CURRENT_RESOURCES.keys())
            for resource_name in resources.keys()
        )

    def test_get_resource_info(self, client: MQSSClient) -> None:
        """Test getting resource info."""
        resource_name = list(CURRENT_RESOURCES.keys())[0]
        resource_info = client.get_resource_info(resource_name)
        assert isinstance(resource_info, ResourceInfo)
        assert resource_info.name == resource_name

    def test_get_resource_info_invalid(self, client: MQSSClient) -> None:
        """Test getting resource info with invalid resource name."""
        resource_info = client.get_resource_info("invalid_resource")
        assert resource_info is None

    def test_get_num_pending_jobs(self, client: MQSSClient, resource_name) -> None:
        """Test getting the number of pending jobs."""
        num_pending_jobs = client.get_num_pending_jobs(resource_name)
        assert isinstance(num_pending_jobs, int)
        assert num_pending_jobs >= 0

    def test_submit_circuit_job(self, client: MQSSClient, circuit_job_request) -> None:
        """Test submitting a circuit job."""
        job_id = client.submit_job(circuit_job_request)
        assert job_id is not None
        client.cancel_job(job_id, circuit_job_request)

    def test_submit_hamiltonian_job(
        self, client: MQSSClient, hamiltonian_job_request
    ) -> None:
        """Test submitting a Hamiltonian job."""
        job_id = client.submit_job(hamiltonian_job_request)
        assert job_id is not None
        client.cancel_job(job_id, hamiltonian_job_request)

    def test_submit_pennylane_job(
        self, client: MQSSClient, pennylane_job_request
    ) -> None:
        """Test submitting a Pennylane job."""
        job_id = client.submit_job(pennylane_job_request)
        assert job_id is not None
        client.cancel_job(job_id, pennylane_job_request)

    def test_job_status(
        self, client: MQSSClient, circuit_job_request, monkeypatch
    ) -> None:
        """Test getting job status."""
        job_id = client.submit_job(circuit_job_request)
        status = client.job_status(job_id, circuit_job_request)
        assert status in [JobStatus.PENDING, JobStatus.WAITING]
        client.cancel_job(job_id, circuit_job_request)
        status = client.job_status(job_id, circuit_job_request)
        assert status == JobStatus.CANCELLED

    def test_wait_for_job_result(
        self, client: MQSSClient, circuit_job_request, monkeypatch
    ) -> None:
        """Test waiting for job result."""
        job_id = client.submit_job(circuit_job_request)
        result = client.wait_for_job_result(job_id, circuit_job_request)
        assert result is not None
        assert isinstance(result, Result)
        assert result.counts["00"] > 0
        assert result.counts["11"] > 0
