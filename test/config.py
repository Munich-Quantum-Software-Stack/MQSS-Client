import os

from mqp_client.resource_info import ResourceInfo

# TODO: change this line to the API endpoint you want to test
URL = "https://portal.quantum.lrz.de:4000/"
# TODO: change this to your own token, obviously remove this line in the future

TOKEN = os.getenv("MQP_TOKEN", None)
# TODO: change this to all the currently available resources
CURRENT_RESOURCES = {
    "Q5": ResourceInfo(
        qubits=5,
        connectivity=None,
        instructions=None,
    ),
    "Q20": ResourceInfo(
        qubits=20,
        connectivity=None,
        instructions=None,
    ),
    "QExa20": ResourceInfo(qubits=20, connectivity=None, instructions=None),
    "AQT20": ResourceInfo(qubits=20, connectivity=None, instructions=None),
    "WMI3": ResourceInfo(qubits=3, connectivity=None, instructions=None),
    "QLM": ResourceInfo(qubits=38, connectivity=None, instructions=None),
}
# TODO: change this to the the qasm you want to test
QASM_FILE = "test/example.qasm"


def get_qasm() -> str:
    with open(QASM_FILE, "r") as f:
        return f.read()
