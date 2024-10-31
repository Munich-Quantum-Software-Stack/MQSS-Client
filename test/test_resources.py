import pytest
from mqp_client import MQPClient

from .config import URL, TOKEN, CURRENT_RESOURCES


@pytest.mark.skipif(TOKEN is None, reason="MQP_TOKEN not provided")
def test_get_specific_resource():
    client = MQPClient(url=URL, token=TOKEN)
    resource_names = CURRENT_RESOURCES.keys()
    for name in resource_names:
        info = client.get_resource_info(name)
        assert info == CURRENT_RESOURCES[name]


@pytest.mark.skipif(TOKEN is None, reason="MQP_TOKEN not provided")
def test_get_all_resources():
    client = MQPClient(url=URL, token=TOKEN)
    resources = client.get_all_resources()
    assert all(x in resources.keys() for x in CURRENT_RESOURCES.keys())


@pytest.mark.skipif(TOKEN is None, reason="MQP_TOKEN not provided")
def test_get_non_existing_resource():
    client = MQPClient(url=URL, token=TOKEN)
    try:
        client.get_resource_info("non_existing")
        assert False
    except Exception as e:
        assert True
