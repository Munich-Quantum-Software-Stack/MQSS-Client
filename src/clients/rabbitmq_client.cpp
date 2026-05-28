/*
 * Copyright (c) 2024 - 2026 MQSS Project
 * All rights reserved.
 *
 * Licensed under the Apache License v2.0 with LLVM Exceptions (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://llvm.org/LICENSE.txt
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "rabbitmq_client.h"

int MQSSRabbitMQClient::connect() {

  mConnection = amqp_new_connection();
  mpSocket = amqp_tcp_socket_new(mConnection);
  if (!mpSocket)
    return 1;

  if (amqp_socket_open(mpSocket, mHostname.c_str(), mPort))
    return 1;

  amqp_rpc_reply_t reply =
      amqp_login(mConnection, "/", 0, AMQP_DEFAULT_FRAME_SIZE, 60,
                 AMQP_SASL_METHOD_PLAIN, mUser.c_str(), mPassword.c_str());

  if (reply.reply_type != AMQP_RESPONSE_NORMAL)
    return 1;

  amqp_channel_open(mConnection, 1);
  reply = amqp_get_rpc_reply(mConnection);

  if (reply.reply_type != AMQP_RESPONSE_NORMAL)
    return 1;

  return 0;
}

void MQSSRabbitMQClient::disconnect() {
  amqp_channel_close(mConnection, 1, AMQP_REPLY_SUCCESS);
  amqp_connection_close(mConnection, AMQP_REPLY_SUCCESS);
  amqp_destroy_connection(mConnection);
}

int MQSSRabbitMQClient::send(const std::string& queue,
                             const std::string& data) {
  amqp_basic_properties_t props;
  props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
  props.content_type = amqp_cstring_bytes("text/plain");
  props.delivery_mode = 2;
  return amqp_basic_publish(mConnection, 1, amqp_empty_bytes,
                            amqp_cstring_bytes(queue.c_str()), 0, 0, &props,
                            amqp_cstring_bytes(data.c_str()));
}

std::string MQSSRabbitMQClient::receive(const std::string& queue) {
  declareQueue(queue);

  amqp_basic_consume(mConnection, 1, amqp_cstring_bytes(queue.c_str()),
                     amqp_empty_bytes, 0, 0, 0, amqp_empty_table);

  amqp_envelope_t envelope;
  amqp_rpc_reply_t reply =
      amqp_consume_message(mConnection, &envelope, NULL, 0);

  if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
    return amqp_error_string2(reply.library_error);
  }

  std::string body(static_cast<char*>(envelope.message.body.bytes),
                   envelope.message.body.len);

  amqp_basic_ack(mConnection, 1, envelope.delivery_tag, false);
  amqp_destroy_envelope(&envelope);

  return body;
}

int MQSSRabbitMQClient::declareQueue(const std::string& queueName) {
  if (std::find(mQueues.begin(), mQueues.end(), queueName) != mQueues.end())
    return 0;

  amqp_queue_declare(mConnection, 1, amqp_cstring_bytes(queueName.c_str()), 0,
                     0, 0, 0, amqp_empty_table);
  mQueues.push_back(queueName);
  return 0;
}
