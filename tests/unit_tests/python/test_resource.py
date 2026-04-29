# ------------------------------------------------------------------------------
# Copyright 2024 Munich Quantum Software Stack Project
#
# Licensed under the Apache License, Version 2.0 with LLVM Exceptions (the
# "License"); you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/LICENSE
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# ------------------------------------------------------------------------------

from mqss.client import MQSSClient
import pytest
import os


MQSS_API_URL = "https://portal.quantum.lrz.de:4000/v1/"

TEST_CIRCUIT = """
OPENQASM 2.0;
include "qelib1.inc";
qreg q[2];
creg c[2];
h q[0];
cx q[0], q[1];
measure q -> c;
"""


@pytest.fixture(params=["hpc", "api"])
def client(request):
    url_or_queue, is_hpc = (
        (os.getenv("MQSS_HPC_QUEUENAME"), True)
        if request.param == "hpc"
        else (MQSS_API_URL, False)
    )
    return MQSSClient(
        token=os.getenv("MQSS_API_TOKEN"), url_or_queue=url_or_queue, is_hpc=is_hpc
    )


def test_client_get_all_resources(client):
    resources = client.resources
    assert len(resources) >= 0


@pytest.mark.parametrize(
    "resource_name",
    ["QLM", "Q5", "Q20", "AQT20", "QExa20"],
)
def test_client_get_a_resource(client, resource_name):
    resource = client.resource(resource_name)
    assert resource is not None


# ---------------------------------------------------------------------
# 3️⃣ ClientGetAResourceFalse
# ---------------------------------------------------------------------
@pytest.mark.parametrize(
    "resource_name",
    ["Eviden", "IQM5", "IQM20", "AQT", "QExa120"],
)
def test_client_get_a_resource_false(client, resource_name):
    resource = client.resource(resource_name)
    assert resource is None


def test_client_check_resource_name(client):
    golden_resource_name = "QLM"

    resource = client.resource(golden_resource_name)
    assert resource is not None
    assert resource.name == golden_resource_name


def test_client_check_qubit_count(client):
    resource_name = "Q5"

    resource = client.resource(resource_name)
    assert resource is not None
    assert resource.qubit_count >= 0


def test_client_check_if_online(client):
    """
    If the resource is under maintenance, this test might fail.
    """
    resource_name = "AQT20"

    resource = client.resource(resource_name)
    assert resource is not None
    assert resource.online is True


def test_client_check_coupling_map(client):
    resource_name = "AQT20"

    resource = client.resource(resource_name)
    assert resource is not None
    assert len(resource.coupling_map) >= 0


def test_client_check_native_gateset(client):
    resource_name = "Q20"

    resource = client.resource(resource_name)
    assert resource is not None
    assert len(resource.native_gateset) >= 0
