"""RabbitMQ Client for HPCQC"""

import os
from dataclasses import dataclass
from json import load
from typing import Any, Dict, Optional
from warnings import warn

import pika.exceptions as pikaExceptions  # type: ignore
from pika import ConnectionParameters  # type: ignore
from pika import BlockingConnection, PlainCredentials
from pika.exceptions import AMQPError  # type: ignore


class RabbitMQClient:
    """RabbitMQ Client for HPCQC"""

    def __init__(self, token: str) -> None:
        self.token = token
        self.connection = None
        self.channel = None
        self.conn_config = (
            ConnectionConfiguration()
            if os.environ.get("MQSS_CLIENT_RMQ_CONN_CONFIG_FILE") is None
            else ConnectionConfiguration.from_json_file(
                os.environ["MQSS_CLIENT_RMQ_CONN_CONFIG_FILE"]
            )
        )

    def connect(self) -> None:
        """Connect to RabbitMQ server"""
        self.connection = create_rmq_connection(self.conn_config)
        assert self.connection is not None
        self.channel = self.connection.channel()

    def close(self) -> None:
        """Close the connection to RabbitMQ server"""
        if self.channel:
            self.channel.close()
        if self.connection:
            self.connection.close()

    def send(self, data: str, destination: str) -> bool:
        """Send data to RabbitMQ server"""
        if not self.channel:
            return False

        if self.channel.is_closed:
            self.channel = self.connection.channel()

        try:
            self.channel.queue_declare(queue=destination)
            self.channel.basic_publish(
                exchange="", routing_key=destination, body=data, mandatory=True
            )
        except AMQPError:
            return False
        return True

    def receive(
        self, source: str
    ) -> Optional[str]:  # Updated return type to Optional[str]
        """Receive data from RabbitMQ server"""
        if not self.channel:
            return None

        if self.channel.is_closed:
            self.channel = self.connection.channel()

        response = None
        self.channel.queue_declare(queue=source)
        for method_frame, _, body in self.channel.consume(
            queue=source,
            inactivity_timeout=1,
            auto_ack=True,
        ):
            if body is not None:
                response = body.decode()
                break

        return response

    def declare_queue(self, queue_name: str) -> None:
        """Declare a queue"""
        if not self.channel:
            return
        self.channel.queue_declare(queue=queue_name)

    def delete_queue(self, queue_name: str) -> None:
        """Delete a queue"""
        if not self.channel:
            return
        self.channel.queue_delete(queue=queue_name)

    def __enter__(self) -> "RabbitMQClient":
        self.connect()
        return self

    def __exit__(self, exc_type, exc_value, traceback) -> None:
        self.close()


@dataclass
class ConnectionConfiguration:
    """Configuration required for connection to RabbitMQ"""

    host: str = "localhost"
    port: int = 5672
    user: str = "guest"
    password: str = "guest"

    @classmethod
    def from_json_file(cls, config_file_path: str, conn_config_key: str = ""):
        """Read Connection Configuration from a file"""

        try:
            with open(file=config_file_path, encoding="utf-8") as json_config_file:
                config_data: Dict[str, Any] = load(json_config_file)
                config_dict: Dict[str, Any] = {}
                if conn_config_key != "" and conn_config_key in config_data:
                    config_dict = config_data[conn_config_key]
                else:
                    config_dict = config_data

                conn_config = cls(**config_dict)
        except (FileExistsError, FileNotFoundError):
            warn(f"Configuration file not found: {config_file_path}")
            return cls()

        return conn_config


def create_rmq_connection(
    conn_config: ConnectionConfiguration, raise_exceptions: bool = False
) -> Optional[
    BlockingConnection
]:  # Updated return type to Optional[BlockingConnection]
    """Connect to RabbitMQ and return a blocking connection handle"""

    _rmq_connection = None
    try:
        _rmq_connection = BlockingConnection(
            ConnectionParameters(
                host=conn_config.host,
                port=conn_config.port,
                heartbeat=0,
                credentials=PlainCredentials(
                    username=conn_config.user,
                    password=conn_config.password,
                ),
            )
        )
        assert _rmq_connection is not None

    except (
        pikaExceptions.ChannelWrongStateError,
        pikaExceptions.AMQPConnectionError,
    ) as _exception:
        if raise_exceptions:
            raise _exception

    return _rmq_connection
