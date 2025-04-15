"""MQP Client settings"""

from decouple import config  # type: ignore

MQP_URL = config("MQP_URL")
