"""Common mock objects and fixtures for MQSS Client tests."""

import json
from unittest.mock import MagicMock, patch

from mqss_client.rest_client import RESTClient

from .config import CURRENT_RESOURCES

# =================== MOCK DATA ===================
MOCK_RESOURCES = {
    key: CURRENT_RESOURCES[key].__dict__ for key in CURRENT_RESOURCES.keys()
}

MOCK_JOB_DATA = {
    # For resource endpoints
    "resources": MOCK_RESOURCES,
    "resources/Q5": CURRENT_RESOURCES["Q5"].__dict__,
    "resources/Q5/num_pending_jobs": {"num_pending_jobs": 3},
    # For job endpoints
    "job": {"jobs": ["mock-uuid-12345"]},
    "hamiltonian_job": {"jobs": ["mock-uuid-12345"]},
    # Status endpoints
    "job/mock-uuid-12345/status": {"status": "PENDING"},
    "hamiltonian_job/mock-uuid-12345/status": {"status": "PENDING"},
    # Result endpoints
    "job/mock-uuid-12345/result": {
        "result": '{"00": 500, "11": 500}',
        "timestamp_completed": "2023-04-14 10:15:30.123456",
        "timestamp_submitted": "2023-04-14 10:00:00.123456",
        "timestamp_scheduled": "2023-04-14 10:05:00.123456",
    },
    "hamiltonian_job/mock-uuid-12345/result": {
        "result": '{"00": 500, "11": 500}',
        "timestamp_completed": "2023-04-14 10:15:30.123456",
        "timestamp_submitted": "2023-04-14 10:00:00.123456",
        "timestamp_scheduled": "2023-04-14 10:05:00.123456",
    },
}


# =================== REST CLIENT MOCKS ===================
def create_rest_mock():
    """Create and configure a mock REST client"""
    mock = MagicMock(spec=RESTClient)
    mock.post.return_value = {"uuid": "mock-uuid-12345"}
    mock.get.side_effect = lambda path: MOCK_JOB_DATA.get(path, {})
    return mock


# =================== RABBITMQ CLIENT MOCKS ===================
def create_rabbitmq_mock():
    """Create and configure a mock RabbitMQ client"""
    mock = MagicMock()
    mock.__enter__.return_value = mock
    mock.__exit__.return_value = None
    mock.last_message = "{}"

    def mock_receive(queue_name):
        try:
            message_data = json.loads(mock.last_message)
            request_path = message_data.get("request", "")
            method = message_data.get("method", "")

            # Handle POST requests for job creation
            if (request_path in ["job", "hamiltonian_job"]) and method == "POST":
                return json.dumps({"uuid": "mock-uuid-12345"})

            # Handle GET requests using the shared data
            if method == "GET" and request_path in MOCK_JOB_DATA:
                return json.dumps(MOCK_JOB_DATA[request_path])

            # Special handling for resource paths with names
            if "resources/" in request_path and "/num_pending_jobs" not in request_path:
                resource_name = request_path.split("/")[-1]
                if resource_name in CURRENT_RESOURCES:
                    return json.dumps(CURRENT_RESOURCES[resource_name].__dict__)
        except Exception as e:
            print(f"Error in mock_receive: {e}")
        return None

    def mock_send(message, queue):
        mock.last_message = message
        return True

    mock.receive.side_effect = mock_receive
    mock.send.side_effect = mock_send
    return mock


# =================== PATCHING HELPERS ===================
def patch_mqss_rest_client():
    """Patch the RESTClient within MQSSClient with a mock"""
    return patch("mqss_client.mqss_client.RESTClient", return_value=create_rest_mock())


def patch_rabbitmq_client():
    """Patch the RabbitMQClient with a mock"""
    return patch(
        "mqss_client.hpc_client.RabbitMQClient", return_value=create_rabbitmq_mock()
    )
