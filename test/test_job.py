import pytest
from mqp_client import MQPClient, JobStatus

from .config import URL, TOKEN, CURRENT_RESOURCES, get_qasm


@pytest.mark.skipif(TOKEN is None, reason="MQP_TOKEN not provided")
def test_job():
    client = MQPClient(url=URL, token=TOKEN)
    some_resource_name = list(CURRENT_RESOURCES.keys())[0]
    uuid = client.submit_job(some_resource_name, get_qasm(), "qasm", 1000)
    assert (
        client.status(uuid) == JobStatus.PENDING
        or client.status(uuid) == JobStatus.WAITING
    )
    client.cancel(uuid)
    assert client.status(uuid) == JobStatus.CANCELLED
    assert client.result(uuid) is None
