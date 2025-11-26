#include "mqss/client.h"

#include "mqss/clients/hpc_client.h"
#include "mqss/clients/rest_client.h"
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
    client = std::make_unique<MQSS_Rest_Client>(token_or_queue_name);
  }
}

MQSS_Client::MQSS_Client(std::string token, std::string url) {
  setToken(token);
  setUrlOrQueueName(url);
  setIsHPC(false);
  client = std::make_unique<MQSS_Rest_Client>(token, url);
}

std::string MQSS_Client::getAllResources() { return client->get("resources"); }

std::string MQSS_Client::getResourceInfo(std::string resource) {
  return client->get("resources/" + resource);
}

std::string MQSS_Client::submitJob(const Job_Request& jobRequest){
  return client->post("job", jobRequest.to_json_str());
}

auto cancelJob(std::string jobRequest);

auto getJobStatus(std::string jobRequest);

auto getJobResult(std::string jobRequest);

auto waitForJobResult(std::string jobRequest);

auto getNumberPendingJobs(std::string resource);
