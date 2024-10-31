## Installation

```bash
pip install mqp-client
```

## Usage

```python
from mqp_client import MQPClient

URL = "<mqp-api-url>"
TOKEN = "<your-mqp-token>"

# create a client instance
client = MQPClient(token=TOKEN, url=URL)

# check out all the resources
resources = client.get_all_resources()
# get information about one specific resource
resource = client.get_resource_info(resource_name="<name-of-resource")

# send a job to the resource
qasm_circuit = "QASM 2.0; ...."
job_id = client.submit_job(resource_name="...", circuit=qasm_circuit, circuit_format="qasm", shots=1000)
# query the job's status
status = client.status(job_id)
# cancel the job
client.cancel(job_id)
# get the jobs result (returns None if job not finished yet)
result = client.result(job_id)
# wait for the result to come
result = client.wait_for_result(job_id)
```

## Setting up development environment

- The repository uses [`pdm`](https://pdm-project.org/en/latest/) for python package management.
- Install pre-commit hooks using `pre-commit install`
