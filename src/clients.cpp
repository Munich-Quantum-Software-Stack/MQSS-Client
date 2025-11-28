#include "mqss/client.h"
#include "mqss/clients/hpc_client.h"
#include "mqss/clients/rest_client.h"

#include <cstddef>
#include <curl/curl.h>
#include <map>
#include <memory>
#include <string>
#include <unistd.h>

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

std::string MQSS_Client::submitJob(Job_Request &jobRequest) {
  std::string path;
  if (dynamic_cast<Circuit_Job_Request *>(&jobRequest))
    path = "job";
  else if (dynamic_cast<Hamiltonian_Job_Request *>(&jobRequest))
    path = "hamiltonian_job";

  std::string result = client->post(path, jobRequest.to_json_str());
  if (result.empty())
    return "";
  nlohmann::json parsed_result = nlohmann::json::parse(result);
  std::string uuid = parsed_result["uuid"];
  ;
  jobRequest.setUUID(uuid);
  return uuid;
}

void MQSS_Client::cancelJob(Job_Request &jobRequest) {
  std::string path;
  if (dynamic_cast<Circuit_Job_Request *>(&jobRequest))
    path = "job";
  else if (dynamic_cast<Hamiltonian_Job_Request *>(&jobRequest))
    path = "hamiltonian_job";
  client->cancel(path + jobRequest.getUUID());
}

std::string MQSS_Client::getJobStatus(Job_Request &jobRequest) {
  std::string path = jobRequest.getUUID() + "/status";
  if (dynamic_cast<Circuit_Job_Request *>(&jobRequest))
    path = "job/" + path;
  else if (dynamic_cast<Hamiltonian_Job_Request *>(&jobRequest))
    path = "hamiltonian_job/" + path;
  std::string result = client->get(path);
  if (result.empty())
    return "";
  nlohmann::json parsed_result = nlohmann::json::parse(result);
  std::string status = parsed_result["status"];
  return status;
}

std::unique_ptr<Job_Result> MQSS_Client::getJobResult(Job_Request &jobRequest) {
  std::string path = jobRequest.getUUID() + "/result";
  if (dynamic_cast<Circuit_Job_Request *>(&jobRequest))
    path = "job/" + path;
  else if (dynamic_cast<Hamiltonian_Job_Request *>(&jobRequest))
    path = "hamiltonian_job/" + path;
  std::string _job_result = client->get(path);
  if (_job_result.empty())
    return NULL;
  nlohmann::json _parsed_job_result = nlohmann::json::parse(_job_result);
  std::string _circuit_result = _parsed_job_result["result"];

  std::map<std::string, unsigned int> _circuit_result_dict;
  nlohmann::json _result_map = nlohmann::json::parse(_circuit_result);
  for (auto &[key, value] : _result_map.items()) {
    _circuit_result_dict[key] = value;
  }

  auto _timestamp_submitted = _parsed_job_result["timestamp_submitted"];

  Job_Result results(_circuit_result_dict,
                     std::string(_parsed_job_result["timestamp_completed"]),
                     std::string(_parsed_job_result["timestamp_submitted"]),
                     std::string(_parsed_job_result["timestamp_scheduled"]));

  return std::make_unique<Job_Result>(results);
}

std::unique_ptr<Job_Result>
MQSS_Client::waitForJobResult(Job_Request &jobRequest) {
  size_t wait_time = 2;
  std::string _status = getJobStatus(jobRequest);
  while (_status == "WAITING" || _status == "PENDING") {
    sleep(wait_time);
    _status = getJobStatus(jobRequest);
  }

  return _status == "COMPLETED" ? getJobResult(jobRequest) : NULL ;
}

int MQSS_Client::getNumberPendingJobs(std::string resource){
  std::string _job_result = client->get("resources/" + resource + "/num_pending_jobs");
  if (_job_result.empty())
    return -1;
  nlohmann::json _parsed_job_result = nlohmann::json::parse(_job_result);
  return _parsed_job_result["num_pending_jobs"];
}
