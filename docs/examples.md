# Examples

<!-- IMPORTANT: Keep the line above as the first line. -->

<!-- This file is a static page and included in the CMakeLists.txt file. -->

<!-- Include the content of README.md between the pair of markers DOXYGEN FAQ. -->

This page contains usage examples for the **MQSS Client**.

\tableofcontents

## MQSS Client Declaration

Before using the **MQSS Client**, an instance needs to be created. The client can operate in two
modes: HPC mode (using RabbitMQ) or remote API mode.

In remote API mode, provide an absolute `http://` or `https://` API base URL,
including its version path (for example, `https://host/v1/`). A trailing slash
is added if missing. Credentials, queries, and fragments in the URL are rejected
with `std::invalid_argument`.

For subdomains of `quantum.lrz.de` and `quantumpathway.eu`, HTTPS URLs with an omitted
port or port `4000`, and an omitted path, `/`, `/v1`, or `/v1/`, are normalized to
`https://<host>:4000/v1/` before connecting. For example,
`https://test.quantumpathway.eu/` becomes `https://test.quantumpathway.eu:4000/v1/`.
The supplied hostname is preserved. Domain suffixes are matched on complete DNS
labels, ignoring case. Custom hosts, explicit alternative
ports, and other paths keep their configured endpoint, with a trailing slash added
if needed.

Construction sends an authenticated `GET <base URL>/resources` request. It throws
`std::runtime_error` if the connection fails, the server returns a non-2xx status
(including authentication errors and redirects), or the check exceeds five seconds.
The connection timeout is three seconds. The C API returns `NULL` on construction
failure.

All REST requests use the same three-second connection timeout and five-second
overall timeout, and accept only HTTP 2xx responses. Failed GET and POST requests
return an empty response, which the client maps to its existing empty, optional,
or error-count return values. Failed cancellation requests throw
`std::runtime_error` in C++ (`RuntimeError` in Python). The void C cancellation
function contains these errors but cannot return a failure status.

When supplying a custom `MQSSBaseClient`, `MQSSClient` defaults to REST job
serialization. Pass `true` as the second constructor argument for HPC serialization.
Blocking job-result polling sleeps for up to two seconds between status checks,
limited by the remaining timeout. The timeout counts time spent sleeping;
network requests add to the total duration and retain their separate REST limits.

```cpp
#include <mqss/client.h>

// Authentication and connection setup
std::string token = "<your-token>";
std::string url_or_queue = "<api-url>";  // or "<rabbitmq-queue-name>"
bool isHPC = true;  // true for HPC mode, false for remote API mode

// Create the client
auto client = MQSSClient(token, url_or_queue, isHPC);
```

### Parameters

| Parameter      | Type          | Description                                                   |
| -------------- | ------------- | ------------------------------------------------------------- |
| `token`        | `std::string` | Authentication key for the underlying compiler infrastructure |
| `url_or_queue` | `std::string` | API URL (remote mode) or RabbitMQ queue name (HPC mode)       |
| `isHPC`        | `bool`        | `true` for HPC mode, `false` for remote API mode              |

### Configuration Examples

**HPC Mode (RabbitMQ):**

```cpp
std::string token = "your-hpc-token";
std::string url_or_queue = "mqss-quantum-queue";
bool isHPC = true;
auto client = MQSSClient(token, url_or_queue, isHPC);
```

**Remote API Mode:**

```cpp
std::string token = "your-api-token";
std::string url_or_queue = "https://api.mqss.example.com";
bool isHPC = false;
auto client = MQSSClient(token, url_or_queue, isHPC);
```

---

## Resource Interface Usage {#resource-interface-usage}

The **MQSS Client**'s resource interface dynamically abstracts the characteristics and capabilities
of available quantum computation resources.

### Available Methods

The resource interface provides the following methods:

- **`getCouplingMap()`** - Returns the coupling map defining qubit connectivity
- **`getQubitCount()`** - Returns the number of qubits
- **`getNativeGateset()`** - Returns the native gateset supported by the device
- **`isOnline()`** - Returns `true` if the device is currently online
- **`getName()`** - Returns the name of the device

### Example: Querying Online Resources

This example demonstrates how to retrieve all available resources and filter for online devices:

```cpp
#include <mqss/client.h>
#include <vector>

// Get all available resources
std::vector<mqss::client::Resource> resources = client.getAllResources();

// Iterate through resources and query online devices
for (auto& resource : resources) {
    if (resource.isOnline()) {
        std::string name = resource.getName();
        auto couplingMap = resource.getCouplingMap();
        auto qubitCount = resource.getQubitCount();
        auto nativeGates = resource.getNativeGateset();

        // Use the resource information
        std::cout << "Online device: " << name
                  << " with " << qubitCount << " qubits" << std::endl;
    }
}
```

---

## Job Interface Usage {#job-interface-usage}

The **MQSS Client**'s job interface encapsulates the logic for quantum task definition and
execution.

### Job Interface Components

The job interface consists of highly customizable classes:

**Job Request Types:**

- **`JobRequest`** - Base class defining the quantum task to be offloaded to the underlying stack
  - **`CircuitJobRequest`** - Defines circuit-based quantum tasks
  - **`HamiltonianJobRequest`** - Defines Hamiltonian-based quantum tasks

**Job Results:**

- **`JobResult`** - Returns the histogram of results along with metrics such as submission time and
  completion time

### Example: Circuit-Based Job Submission

This example demonstrates how to submit a Bell state circuit and retrieve the results:

```cpp
#include <mqss/client.h>

// Define a Bell state circuit in QASM format
static const std::string BELL_STATE_CIRCUIT = R"(
OPENQASM 2.0;
include "qelib1.inc";
qreg q[2];
creg c[2];
h q[0];
cx q[0], q[1];
measure q -> c;
)";

// Create a circuit job request
mqss::client::CircuitJobRequest job = mqss::client::CircuitJobRequest(
    BELL_STATE_CIRCUIT,  // Circuit definition
    "qasm",              // Circuit format
    "qr-1",              // Target resource name
    100,                 // Number of shots
    0,                   // Optimization level
    0                    // Additional flags
);

// Submit the job and get UUID
auto uuid = client.submitJob(job);

// Retrieve the job result
// Parameters: job, blocking (true), timeout in seconds (50)
std::unique_ptr<JobResult> result = client.getJobResult(job, true, 50);

// Access the results
if (result) {
    auto histogram = result->getHistogram();
    auto submissionTime = result->getSubmissionTime();
    auto completionTime = result->getCompletionTime();

    // Process the histogram
    for (const auto& [bitstring, count] : histogram) {
        std::cout << bitstring << ": " << count << std::endl;
    }
}
```

### Job Submission Workflow

1. **Define the quantum task** using `CircuitJobRequest` or `HamiltonianJobRequest`
2. **Submit the job** using `client.submitJob()` which returns a UUID
3. **Retrieve results** using `client.getJobResult()` with optional blocking and timeout
4. **Process the results** from the `JobResult` object

### Non-Blocking Job Retrieval

For asynchronous job handling:

```cpp
// Submit job
auto uuid = client.submitJob(job);

// Poll for results without blocking
std::unique_ptr<JobResult> result = client.getJobResult(job, false, 100);

if (result) {
    // Job completed, process results
} else {
    // Job still running, check again later
}
```

---

## Complete Example

Here's a complete example combining resource querying and job submission:

```cpp
#include <mqss/client.h>
#include <iostream>
#include <vector>

int main() {
    // Initialize client
    std::string token = "your-token";
    std::string url_or_queue = "https://api.mqss.example.com";
    bool isHPC = false;
    auto client = MQSSClient(token, url_or_queue, isHPC);

    // Find an online resource
    std::vector<mqss::client::Resource> resources = client.getAllResources();
    std::string targetResource;

    for (auto& resource : resources) {
        if (resource.isOnline()) {
            targetResource = resource.getName();
            std::cout << "Using resource: " << targetResource << std::endl;
            std::cout << "Qubits: " << resource.getQubitCount() << std::endl;
            break;
        }
    }

    if (targetResource.empty()) {
        std::cerr << "No online resources available" << std::endl;
        return 1;
    }

    // Define and submit a quantum circuit
    static const std::string CIRCUIT = R"(
OPENQASM 2.0;
include "qelib1.inc";
qreg q[2];
creg c[2];
h q[0];
cx q[0], q[1];
measure q -> c;
)";

    mqss::client::CircuitJobRequest job(CIRCUIT, "qasm", targetResource, 1000, 0, 0);
    auto uuid = client.submitJob(job);

    std::cout << "Job submitted with UUID: " << uuid << std::endl;

    // Wait for results
    std::unique_ptr<JobResult> result = client.getJobResult(job, true, 120);

    if (result) {
        std::cout << "Job completed!" << std::endl;
        auto histogram = result->getResults();
        for (const auto& [bitstring, count] : histogram) {
            std::cout << bitstring << ": " << count << std::endl;
        }
    } else {
        std::cerr << "Job failed or timed out" << std::endl;
    }

    return 0;
}
```
