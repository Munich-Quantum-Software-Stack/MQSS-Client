## Building and Requirements

The requirements of the MQSSClient are as follows:

```bash
1) cmake >= 3.19
2) CURL
3) nlohmann_json
4) rabbitmq-c
```

To build the MQSSClient, the commands need to be executable:

```bash
cmake -S . -B build
cmake --build build
```

## Usage

```cpp
#include <mqss/client.h>
URL_OR_QUEUE_NAME = "<mqp-api-url-queue-name>"
TOKEN = "<your-mqp-token>"
IS_HPC = false

client = mqss::client::MQSSClient(TOKEN, URL_OR_QUEUE_NAME, IS_HPC);

```
