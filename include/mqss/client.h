#pragma once

#include "mqss/device.h"
#include "mqss/job.h"
#include <mqss/clients/hpc_client.h>
#include <mqss/clients/rest_client.h>

#include <memory>
#include <optional>

class MQSS_Client {
public:

    MQSS_Client();
    MQSS_Client(const std::string& token_or_queue, bool is_hpc);
    MQSS_Client(const std::string& token, const std::string& url);

    // Devices
    std::vector<Device> getAllResources();
    std::optional<Device> getResourceInfo(const std::string& resource);

    // Jobs
    std::string submitJob(Job_Request& job);
    void cancelJob(Job_Request& job);
    std::string getJobStatus(Job_Request& job);
    std::unique_ptr<Job_Result> getJobResult(Job_Request& job);
    std::unique_ptr<Job_Result> waitForJobResult(Job_Request& job, size_t poll_seconds = 2);

    int getNumberPendingJobs(const std::string& resource);

private:
    std::unique_ptr<MQSS_Base_Client> client_;

};
