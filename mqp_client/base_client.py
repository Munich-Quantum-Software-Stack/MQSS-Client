"""MQP REST API Client"""

from posixpath import join
from typing import Dict, Optional

import requests  # type: ignore
from decouple import config  # type: ignore

MQP_API_VERSION: str = "v1"
REQUEST_TIMEOUT: int = 10


def _fetch_hardcoded_url() -> Optional[str]:
    """Default URL for MQP REST API"""
    return "https://portal.quantum.lrz.de:4000"


# pylint: disable=too-few-public-methods
class BaseClient:
    """REST API Client Base Class for basic REST functions"""

    def __init__(self, token: str, url: Optional[str] = None) -> None:
        self.token = token

        self.url = url or config("MQP_URL", default=None) or _fetch_hardcoded_url()
        if not self.url:
            raise RuntimeError("No url provided for munich quantum portal.")
        assert self.url is not None

    def _headers(self) -> Dict[str, str]:
        return {"Authorization": f"Bearer {self.token}"}

    def _get(self, path: str) -> dict:
        assert isinstance(self.url, str)
        response = requests.get(
            join(self.url, MQP_API_VERSION, path),
            headers=self._headers(),
            timeout=REQUEST_TIMEOUT,
        )
        response.raise_for_status()
        return response.json()

    def _post(self, path: str, data: dict) -> dict:
        assert isinstance(self.url, str)
        response = requests.post(
            join(self.url, MQP_API_VERSION, path),
            json=data,
            headers=self._headers(),
            timeout=REQUEST_TIMEOUT,
        )
        response.raise_for_status()
        return response.json()

    def _delete(self, path: str) -> None:
        assert isinstance(self.url, str)
        response = requests.delete(
            join(self.url, MQP_API_VERSION, path),
            headers=self._headers(),
            timeout=REQUEST_TIMEOUT,
        )
        response.raise_for_status()
