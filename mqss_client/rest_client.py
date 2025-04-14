"""MQP REST API Client"""

from posixpath import join
from typing import Dict, Optional

import requests  # type: ignore
from decouple import config  # type: ignore

from .base_client import BaseClient

MQP_API_VERSION: str = "v1"
REQUEST_TIMEOUT: int = 10


def _fetch_hardcoded_url() -> str:
    """Default URL for MQP REST API"""
    return "https://portal.quantum.lrz.de:4000"


# pylint: disable=too-few-public-methods
class RESTClient(BaseClient):
    """REST API Client Base Class for basic REST functions"""

    def __init__(self, token: str, url: Optional[str] = None) -> None:
        self.token = token

        self.url = url or config("MQP_URL", default=None) or _fetch_hardcoded_url()
        if not self.url:
            raise RuntimeError("No URL provided for MQP.")

    def _headers(self) -> Dict[str, str]:
        return {"Authorization": f"Bearer {self.token}"}

    def get(self, path: str) -> dict:
        """GET request to the specified path"""
        assert isinstance(self.url, str)
        try:
            response = requests.get(
                join(self.url, MQP_API_VERSION, path),
                headers=self._headers(),
                timeout=REQUEST_TIMEOUT,
            )
            response.raise_for_status()
        except requests.exceptions.JSONDecodeError:
            print(f"Error decoding JSON response from {path}")
            return {}
        except requests.exceptions.Timeout:
            print(f"Request to {path} timed out")
            return {}
        except requests.exceptions.RequestException as e:
            print(f"Error fetching data from {path}: {e}")
            return {}
        return response.json()

    def post(self, path: str, data: dict) -> dict:
        """POST request to the specified path"""
        assert isinstance(self.url, str)
        try:
            response = requests.post(
                join(self.url, MQP_API_VERSION, path),
                json=data,
                headers=self._headers(),
                timeout=REQUEST_TIMEOUT,
            )
            response.raise_for_status()
        except requests.exceptions.Timeout:
            print(f"Request to {path} timed out")
            return {}
        except requests.exceptions.RequestException as e:
            print(f"Error posting data to {path}: {e}")
            return {}
        return response.json()

    def delete(self, path: str) -> None:
        """DELETE request to the specified path"""
        assert isinstance(self.url, str)
        try:
            response = requests.delete(
                join(self.url, MQP_API_VERSION, path),
                headers=self._headers(),
                timeout=REQUEST_TIMEOUT,
            )
            response.raise_for_status()
        except requests.exceptions.Timeout:
            print(f"Request to {path} timed out")
        except requests.exceptions.RequestException as e:
            print(f"Error deleting data from {path}: {e}")
