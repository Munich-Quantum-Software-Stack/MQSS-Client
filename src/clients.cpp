#include "hpc_client.h"
#include "mqss/client.h"
#include "rest_client.h"

#include <iostream>
#include <memory>
#include <string>

#include <curl/curl.h>

MQSS_Client::MQSS_Client() {
  setIsHPC(true);
  client = std::make_unique<MQSS_HPC_Client>();
}

MQSS_Client::MQSS_Client(std::string token_or_queue_name, bool is_hpc) {
  setIsHPC(is_hpc);
  if (is_hpc) {
    setUrlOrQueueName(token_or_queue_name);
    client = std::make_unique<MQSS_HPC_Client>(url_or_queue_name);
  } else {
    setToken(token_or_queue_name);
    client = std::make_unique<MQSS_Rest_Client>(token);
  }
}

MQSS_Client::MQSS_Client(std::string token, std::string url) {
  setToken(token);
  setUrlOrQueueName(url);
  setIsHPC(false);
  client = std::make_unique<MQSS_Rest_Client>(token, url);
}

std::string MQSS_Client::getAllResources() { return client->get("resources"); }