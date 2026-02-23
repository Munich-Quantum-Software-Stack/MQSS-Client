#include "mqss/client.h"
#include "clients/hpc_client.h"
#include "clients/rest_client.h"
#include <chrono>
#include <optional>
#include <thread>


using namespace mqss::client;

MQSSClient::MQSSClient(const std::string &token,
                       const std::string &url_or_queue, bool is_hpc) {
  if (is_hpc) {
    mClient = std::make_unique<MQSSHPCClient>(token, url_or_queue);
  } else {
    mClient = std::make_unique<MQSSRestClient>(token, url_or_queue);
  }

}

  std::vector<Device> MQSSClient::getAllResources() const {
    std::vector<Device> devices;
    std::string resp = mClient->get("resources");
    if (!nlohmann::json::accept(resp))
      return {};
    nlohmann::json parsed = nlohmann::json::parse(resp);
    for (auto &item : parsed) {
      devices.push_back(Device::fromJson(item));
    }
    return devices;
  }

  std::optional<Device> MQSSClient::getResourceInfo(const std::string &resource)
      const {
    std::string resp = mClient->get("resources/" + resource);
    if (resp.find("RESOURCE NOT FOUND") != std::string::npos)
      return std::nullopt;

    if (!nlohmann::json::accept(resp))
      return std::nullopt;

    nlohmann::json parsed = nlohmann::json::parse(resp);
    if (parsed.contains("ERROR"))
      return std::nullopt;
    return Device::fromJson(parsed);
  }
  std::optional<std::string> MQSSClient::submitJob(JobRequest & job) {
    std::string path = job.getPath();
    std::string result = mClient->post(path, job.toJson());
    
    if (result.empty() || !nlohmann::json::accept(result))
      return std::nullopt;

    nlohmann::json parsed = nlohmann::json::parse(result);
    if (!parsed.contains("uuid"))
      return std::nullopt;

    std::string uuid = parsed["uuid"].get<std::string>();
    job.setUuid(uuid);
    return uuid;
  }

  void MQSSClient::cancelJob(JobRequest & job) {
    std::string path = job.getPath() + "/" + job.getUuid();
    mClient->cancel(path);
  }

  std::string MQSSClient::getJobStatus(const JobRequest &job) {
    std::string path = job.getPath() + "/" + job.getUuid() + "/status";
    std::string resp = mClient->get(path);
    if (resp.empty())
      return "";

    if (!nlohmann::json::accept(resp))
      return "";

    nlohmann::json parsed = nlohmann::json::parse(resp);
    return parsed.value("status", "");
  }

  std::unique_ptr<JobResult> MQSSClient::getJobResult(const JobRequest &job) {
    std::string path = job.getPath() + "/" + job.getUuid() + "/result";
    std::string resp = mClient->get(path);
    if (resp.empty() || !nlohmann::json::accept(resp))
      return nullptr;

    nlohmann::json parsed = nlohmann::json::parse(resp);
    return std::make_unique<JobResult>(JobResult::fromJson(parsed));
  }

  std::unique_ptr<JobResult> MQSSClient::waitForJobResult(const JobRequest &job,
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

  int MQSSClient::getNumberPendingJobs(const std::string &resource) const {
    std::string resp =
        mClient->get("resources/" + resource + "/num_pending_jobs");
    if (resp.empty() || !nlohmann::json::accept(resp))
      return -1;

    nlohmann::json parsed = nlohmann::json::parse(resp);

    return parsed.value("num_pending_jobs", -1);
  }
