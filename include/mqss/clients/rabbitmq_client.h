
#include <amqp.h>
#include <amqp_tcp_socket.h>

#include <algorithm>
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
                       std::string hostname = "host.docker.internal",
                       int port = 5672)
      : user(user), password(password), hostname(hostname), port(port) {};

  ~MQSS_RabbitMQ_Client() { disconnect(); }

  int connect() {
    connection = amqp_new_connection();
    socket = amqp_tcp_socket_new(connection);
    if (!socket)
      return -1;

    if (amqp_socket_open(socket, hostname.c_str(), port))
      return -2;

    amqp_rpc_reply_t reply =
        amqp_login(connection, "/", 0, AMQP_DEFAULT_FRAME_SIZE, 60,
                   AMQP_SASL_METHOD_PLAIN, user.c_str(), password.c_str());

    if (reply.reply_type != AMQP_RESPONSE_NORMAL)
      return -3;

    amqp_channel_open(connection, 1);
    reply = amqp_get_rpc_reply(connection);
    if (reply.reply_type != AMQP_RESPONSE_NORMAL)
      return -4;

    return 0;
  }

  void disconnect() {
    amqp_channel_close(connection, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(connection, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(connection);
  }

  int send(std::string queue, std::string data) {
    amqp_basic_properties_t props;
    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
    props.content_type = amqp_cstring_bytes("text/plain");
    props.delivery_mode = 2;
    return amqp_basic_publish(connection, 1, amqp_empty_bytes,
                              amqp_cstring_bytes(queue.c_str()), 0, 0, &props,
                              amqp_cstring_bytes(data.c_str()));
  }
  std::string receive(const std::string &queue) {
    declare_queue(queue);

    amqp_basic_consume(connection, 1, amqp_cstring_bytes(queue.c_str()),
                       amqp_empty_bytes, 0, 0, 0, amqp_empty_table);

    amqp_envelope_t envelope;
    amqp_rpc_reply_t reply =
        amqp_consume_message(connection, &envelope, NULL, 0);

    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
      return amqp_error_string2(reply.library_error);
    }

    std::string body(static_cast<char *>(envelope.message.body.bytes),
                     envelope.message.body.len);

    amqp_basic_ack(connection, 1, envelope.delivery_tag, false);
    amqp_destroy_envelope(&envelope);

    return body;
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
