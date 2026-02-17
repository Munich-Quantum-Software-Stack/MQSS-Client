#include "clients/hpc_client.h" 
#include "clients/rest_client.h" 
#include <iostream>
#include <thread>

using namespace mqss::client;


MQSSClient::MQSSClient() { client_ = std::make_unique<MQSS_HPC_Client>(); }

MQSSClient::MQSSClient(const std::string &token_or_queue, bool is_hpc) {
  if (is_hpc) {
    client_ = std::make_unique<MQSS_HPC_Client>(token_or_queue);
  } else {
    client_ = std::make_unique<MQSS_Rest_Client>(token_or_queue);
  }
}

MQSSClient::MQSSClient(const std::string &token, const std::string &url) {
  client_ = std::make_unique<MQSS_Rest_Client>(token, url);
}

std::vector<Device> MQSSClient::getAllResources() {
  std::vector<Device> devices;
  std::string resp = client_->get("resources");
  json parsed = json::parse(resp);
  for (auto &item : parsed) {
    devices.push_back(Device::from_json(item));
  }
  return devices;
}

std::optional<Device>
MQSSClient::getResourceInfo(const std::string &resource) {
  std::string resp = client_->get("resources/" + resource);
  if (resp.find("RESOURCE NOT FOUND") != std::string::npos)
    return std::nullopt;
  json parsed = json::parse(resp);
  if (parsed.contains("ERROR"))
    return std::nullopt;
  return Device::from_json(parsed);
}
std::optional<std::string> MQSSClient::submitJob(Job_Request &job) {
  std::string path = job.getPath();
  std::string result = client_->post(path, job.to_json());
  bool isValid = json::accept(result);
  if (result.empty() || !isValid)
    return std::nullopt;

  json parsed = json::parse(result);
  std::string uuid = parsed["uuid"];
  job.setUUID(uuid);
  return uuid;
}

void MQSSClient::cancelJob(Job_Request &job) {
  std::string path = job.getPath() + "/" + job.getUUID();
  client_->cancel(path);
}

std::string MQSSClient::getJobStatus(Job_Request &job) {
  std::string path = job.getPath() + "/" + job.getUUID() + "/status";
  std::string resp = client_->get(path);
  if (resp.empty())
    return "";

  json parsed = json::parse(resp);
  return parsed.value("status", "");
}

std::unique_ptr<Job_Result> MQSSClient::getJobResult(Job_Request &job) {
  std::string path = job.getPath() + "/" + job.getUUID() + "/result";
  std::string resp = client_->get(path);
  if (resp.empty())
    return nullptr;

  json parsed = json::parse(resp);
  return std::make_unique<Job_Result>(Job_Result::from_json(parsed));
}

std::unique_ptr<Job_Result> MQSSClient::waitForJobResult(Job_Request &job,
                                                          size_t poll_seconds) {

  while (true) {
    std::string status = getJobStatus(job);
    if (status == "COMPLETED")
      break;
    if (status == "FAILED" || status == "CANCELLED" || status.empty())
      return nullptr;
    std::this_thread::sleep_for(std::chrono::seconds(poll_seconds));
  }
  return getJobResult(job);
}

int MQSSClient::getNumberPendingJobs(const std::string &resource) {
  std::string resp =
      client_->get("resources/" + resource + "/num_pending_jobs");
  if (resp.empty())
    return -1;

  json parsed = json::parse(resp);
  return parsed.value("num_pending_jobs", -1);
}
