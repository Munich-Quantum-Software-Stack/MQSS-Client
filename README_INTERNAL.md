## Installation

```bash
pip install mqss-client
```

## Usage

```python
from mqss_client import MQSSClient, CircuitJobRequest

URL = "<mqp-api-url>"
TOKEN = "<your-mqp-token>"

# create a client instance
# set is_hpc=True if running on HPC Cluster
client = MQSSClient(token=TOKEN, base_url=URL, is_hpc=False)

# check out all the resources
resources = client.get_all_resources()
# get information about one specific resource
resource = client.get_resource_info(resource_name="<name-of-resource")

# qasm circuit
qasm_circuit = "QASM 2.0; ...."
#create job request
job_request = CircuitJobRequest(
        circuits=qasm_circuit,
        circuit_format="qasm",
        resource_name=resource_name,
        shots=1000,
        no_modify=False,
        queued=False,
    )
# send a job to the resource
job_id = client.submit_job(job_request)
# query the job's status
status = client.job_status(job_id, job_request)
# cancel the job
client.cancel_job(job_id, job_request)
# get the jobs result (returns None if job not finished yet)
result = client.job_result(job_id, job_request)
# wait for the result to come
result = client.wait_for_job_result(job_id, job_request)
```

## Compute Node Config for HPCQC

The following two environment variables can be set if needed

- The queue name where the Quantum Offload Listener is accepting the requests can be set using
  `MQSS_OFFLOAD_LISTENER_QUEUE_NAME`
- Path to the file where RabbitMQ connection configuration can be set using
  `MQSS_CLIENT_RMQ_CONN_CONFIG_FILE`

## Setting up development environment

- The repository uses [`uv`](https://docs.astral.sh/uv/) for python package management.
- Install pre-commit hooks using `pre-commit install`
