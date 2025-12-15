
#include <algorithm>
#include <amqp.h>
#include <amqp_tcp_socket.h>
#include <bits/types/struct_timeval.h>
#include <cstddef>
#include <string>
#include <vector>

class MQSS_RabbitMQ_Client {
  amqp_connection_state_t connection;
  amqp_socket_t *socket;

  std::string hostname;
  std::string user;
  std::string password;
  int port;
  std::vector<std::string> queues;

public:
  MQSS_RabbitMQ_Client(std::string user = "guest",
                       std::string password = "guest",
                       std::string hostname = "localhost", int port = 5672)
      : user(user), password(password), hostname(hostname), port(port) {};

  int connect() {
    connection = amqp_new_connection();
    socket = amqp_tcp_socket_new(connection);
    if (socket == NULL)
      return -1;
    int status = amqp_socket_open(socket, hostname.c_str(), port);
    if (status)
      return -1;

    amqp_rpc_reply_t reply =
        amqp_login(connection, "/", 0, AMQP_DEFAULT_FRAME_SIZE, 0,
                   AMQP_SASL_METHOD_PLAIN, user.c_str(), password.c_str());
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
      return -1;
    }
    amqp_channel_open_ok_t *opened = amqp_channel_open(connection, 1);
    if (opened == NULL) {
      return -1;
    }

    return 0;
  }
  void disconnect() { free(connection); }

  int send(std::string queue, std::string data) {
    amqp_basic_properties_t props;
    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
    props.content_type = amqp_cstring_bytes("text/plain");
    props.delivery_mode = 2;
    return amqp_basic_publish(connection, 1, amqp_empty_bytes,
                              amqp_cstring_bytes(queue.c_str()), 0, 0, &props,
                              amqp_cstring_bytes(data.c_str()));
  }
  std::string receive(std::string queue) {

    amqp_basic_consume(connection, 1, amqp_cstring_bytes(queue.c_str()),
                       amqp_empty_bytes, 0, 1, 0, amqp_empty_table);
    amqp_rpc_reply_t rpc_reply = amqp_get_rpc_reply(connection);

    if (rpc_reply.reply_type != AMQP_RESPONSE_NORMAL) {
      fprintf(stderr, "Failed to start consuming messages.\n");
    }

    amqp_rpc_reply_t res;
    amqp_envelope_t envelope;

    amqp_maybe_release_buffers(connection);

    res = amqp_consume_message(connection, &envelope, NULL, 0);
    
    void* response = envelope.message.body.bytes;

    return std::string((char*)response);
  }

  int declare_queue(std::string queue_name) {
    if (std::find(queues.begin(), queues.end(), queue_name) != queues.end())
      return 0;

    amqp_queue_declare(connection, 1, amqp_cstring_bytes(queue_name.c_str()), 0,
                       0, 0, 0, amqp_empty_table);
    queues.push_back(queue_name);
    return 0;
  }
};