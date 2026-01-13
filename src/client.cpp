#include "mqss/client.h"
#include <iostream>
#include <thread>


MQSS_Client::MQSS_Client() { client_ = std::make_unique<MQSS_HPC_Client>(); }

MQSS_Client::MQSS_Client(const std::string &token_or_queue, bool is_hpc) {
  if (is_hpc) {
    client_ = std::make_unique<MQSS_HPC_Client>(token_or_queue);
  } else {
    client_ = std::make_unique<MQSS_Rest_Client>(token_or_queue);
  }
}

MQSS_Client::MQSS_Client(const std::string &token, const std::string &url) {
  client_ = std::make_unique<MQSS_Rest_Client>(token, url);
}

std::vector<Device> MQSS_Client::getAllResources() {
  std::vector<Device> devices;
  std::string resp = client_->get("resources");
  std::cout << resp << "\n";
  json parsed = json::parse(resp);
  for (auto &item : parsed) {
    devices.push_back(Device::from_json(item));
  }
  return devices;
}

std::optional<Device>
MQSS_Client::getResourceInfo(const std::string &resource) {
  std::string resp = client_->get("resources/" + resource);
  if (resp.find("RESOURCE NOT FOUND") != std::string::npos)
    return std::nullopt;
  json parsed = json::parse(resp);
  if (parsed.contains("ERROR"))
    return std::nullopt;
  return Device::from_json(parsed);
}

std::string MQSS_Client::submitJob(Job_Request &job) {
  std::string path = job.getPath();
  std::string result = client_->post(path, job.to_json_str());
  if (result.empty())
    return "";

  json parsed = json::parse(result);
  std::string uuid = parsed["uuid"];
  job.setUUID(uuid);
  return uuid;
}

void MQSS_Client::cancelJob(Job_Request &job) {
  std::string path = job.getPath() + "/" + job.getUUID();
  client_->cancel(path);
}

std::string MQSS_Client::getJobStatus(Job_Request &job) {
  std::string path = job.getPath() + "/" + job.getUUID() + "/status";
  std::string resp = client_->get(path);
  if (resp.empty())
    return "";

  json parsed = json::parse(resp);
  return parsed.value("status", "");
}

std::unique_ptr<Job_Result> MQSS_Client::getJobResult(Job_Request &job) {
  std::string path = job.getPath() + "/" + job.getUUID() + "/result";
  std::string resp = client_->get(path);
  if (resp.empty())
    return nullptr;

  json parsed = json::parse(resp);
  return std::make_unique<Job_Result>(Job_Result::from_json(parsed));
}

std::unique_ptr<Job_Result> MQSS_Client::waitForJobResult(Job_Request &job,
                                                          size_t poll_seconds) {
  while (true) {
    std::string status = getJobStatus(job);
    if (status == "COMPLETED")
      break;
    if (status == "FAILED" || status.empty())
      return nullptr;

    std::this_thread::sleep_for(std::chrono::seconds(poll_seconds));
  }
  return getJobResult(job);
}

int MQSS_Client::getNumberPendingJobs(const std::string &resource) {
  std::string resp =
      client_->get("resources/" + resource + "/num_pending_jobs");
  if (resp.empty())
    return -1;

  json parsed = json::parse(resp);
  return parsed.value("num_pending_jobs", -1);
}
