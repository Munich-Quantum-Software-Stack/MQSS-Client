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
from mqss.client import CircuitJobRequest, HamiltonianJobRequest

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
def client_with_mode(request):
    url_or_queue, is_hpc = (
        (os.getenv("MQSS_HPC_QUEUENAME"), True)
        if request.param == "hpc"
        else (MQSS_API_URL, False)
    )
    return MQSSClient(
        token=os.getenv("MQSS_API_TOKEN"), url_or_queue=url_or_queue, is_hpc=is_hpc
    ), is_hpc


@pytest.fixture
def circuit_job():
    return CircuitJobRequest(
        TEST_CIRCUIT,
        "qasm",
        "QLM",
        100,
        0,
        0,
    )


def test_client_submit_job(client_with_mode, circuit_job):
    client, is_hpc = client_with_mode
    job_id = client.submit_job(circuit_job)
    assert job_id is not None


def test_client_submit_hamiltonian_job(client_with_mode):
    client, is_hpc = client_with_mode
    if is_hpc:
        pytest.skip("Hamiltonian job not supported in HPC mode")

    job = HamiltonianJobRequest(
        "QLM",
        "0 1; 1 2; 0 2; 0 3;",
        "0.5 0.1 0.8 1;",
    )

    job_id = client.submit_job(job)
    assert job_id is not None


def test_client_check_job_status(client_with_mode, circuit_job):
    client, is_hpc = client_with_mode
    job_id = client.submit_job(circuit_job)
    assert job_id is not None

    status = client.job_status(circuit_job)
    assert status != ""


def test_client_check_job_setter_and_getter():
    job = CircuitJobRequest()

    circuit_format = "qasm"
    resource_name = "AQT20"
    shots = 10
    is_queued = False
    is_no_modify = False

    job.circuit = TEST_CIRCUIT
    assert job.circuit == TEST_CIRCUIT

    job.circuit_format = circuit_format
    assert job.circuit_format == circuit_format

    job.resource_name = resource_name
    assert job.resource_name == resource_name

    job.shots = shots
    assert job.shots == shots

    job.no_modify = is_no_modify
    assert job.no_modify == is_no_modify

    job.queued = is_queued
    assert job.queued == is_queued


def test_client_check_hamiltonian_job_setter_and_getter():
    job = HamiltonianJobRequest()

    coefficients = "0.5 0.1 0.8 1;"
    interaction = "0 1; 1 2; 0 2; 0 3;"

    job.coefficients = coefficients
    assert job.coefficients == coefficients

    job.interaction = interaction
    assert job.interaction == interaction


def test_client_wait_for_result(client_with_mode, circuit_job):
    client, is_hpc = client_with_mode
    if is_hpc:
        pytest.skip("Wait for result not supported in HPC mode")

    job_id = client.submit_job(circuit_job)
    assert job_id is not None

    result = client.job_results(circuit_job, wait=True, timeout=50)

    assert result is not None
    assert len(result.results) != 0


def test_client_get_num_pending_jobs(client_with_mode, circuit_job):
    client, is_hpc = client_with_mode
    job_id = client.submit_job(circuit_job)
    assert job_id is not None

    n_jobs = client.pending_job_count("QLM")
    assert n_jobs >= 0
