"""Base Client for underlying communication clients of the MQSS Client"""

from abc import ABC, abstractmethod


class BaseClient(ABC):
    """Abstract base class for MQSS clients"""

    @abstractmethod
    def get(self, path: str) -> dict:
        """GET request to the specified path"""
        return {}

    @abstractmethod
    def post(self, path: str, data: dict) -> dict:
        """POST request to the specified path"""
        return {}

    @abstractmethod
    def delete(self, path: str) -> None:
        """DELETE request to the specified path"""
        return None
