#pragma once

#include <memory>
#include <optional>

#include <nlohmann/json.hpp>
#include <regex>
using json = nlohmann::json;

namespace mqss::client {

class MQSS_Base_Client {
public:
  virtual ~MQSS_Base_Client() = default;
  virtual std::string get(const std::string &path) = 0;
  virtual std::string post(const std::string &path, const json &data) = 0;
  virtual void cancel(const std::string &path) = 0;
};

template <typename T, typename Converter>
std::vector<T> extract(const json &response, const std::string &key,
                       const std::regex &pattern, Converter &&convert) {
  std::vector<T> result;

  const auto it = response.find(key);
  if (it == response.end() || !it->is_string())
    return result;

  const std::string text = it->get<std::string>();
  if (text == "None")
    return result;

  std::smatch match;
  auto begin = text.cbegin();
  auto end = text.cend();

  while (std::regex_search(begin, end, match, pattern)) {
    result.push_back(convert(match));
    begin = match.suffix().first;
  }

  return result;
}

class Device {
public:
  explicit Device(std::string name, unsigned qubit_count, bool online,
                  std::vector<std::pair<int, int>> coupling_map,
                  std::vector<std::string> native_gateset)
      : name_(std::move(name)), qubit_count_(qubit_count), online_(online),
        coupling_map_(std::move(coupling_map)),
        native_gateset_(std::move(native_gateset)) {}

  const std::string &name() const noexcept { return name_; }
  unsigned qubit_count() const noexcept { return qubit_count_; }
  bool online() const noexcept { return online_; }

  const std::vector<std::pair<int, int>> &coupling_map() const noexcept {
    return coupling_map_;
  }

  const std::vector<std::string> &native_gateset() const noexcept {
    return native_gateset_;
  }

  static Device from_json(const json &response) {

    std::string name = response.value("name", "");
    unsigned qubits = response.value("qubits", 0);
    bool online = response.value("online", false);

    auto coupling_map = extract<std::pair<int, int>>(
        response, "connectivity", std::regex(R"(\(\s*(\d+)\s*,\s*(\d+)\s*\))"),
        [](const std::smatch &m) {
          return std::make_pair(std::stoi(m[1]), std::stoi(m[2]));
        });

    auto gates =
        extract<std::string>(response, "instructions", std::regex("'([^']+)'"),
                             [](const std::smatch &m) { return m[1].str(); });
    return Device{name, qubits, online, coupling_map, gates};
  }

private:
  std::string name_;
  unsigned qubit_count_;
  bool online_;
  std::vector<std::pair<int, int>> coupling_map_;
  std::vector<std::string> native_gateset_;
};

class Job_Request {
  std::string uuid;

public:
  virtual ~Job_Request() = default;
  virtual json to_json() const = 0;
  virtual std::string getPath() const = 0;
  std::string getUUID() { return uuid; }
  void setUUID(std::string _uuid) { uuid = _uuid; }
};

class Circuit_Job_Request : public Job_Request {
private:
  std::string circuit;
  std::string circuit_format;
  std::string resource_name;
  unsigned int shots;
  bool no_modify;
  bool queued;

public:
  Circuit_Job_Request() {};

  Circuit_Job_Request(std::string circuit, std::string circuit_format,
                      std::string resource_name, unsigned int shots,
                      bool no_modify, bool queued)
      : circuit(circuit), circuit_format(circuit_format),
        resource_name(resource_name), shots(shots), no_modify(no_modify),
        queued(queued) {}

  void setCircuit(std::string _circuit) { circuit = _circuit; }
  std::string getCircuit() { return circuit; }
  void setCircuitFormat(std::string _circuit_format) {
    circuit_format = _circuit_format;
  }
  std::string getCircuitFormat() { return circuit_format; }
  void setResourceName(std::string _resource_name) {
    resource_name = _resource_name;
  }
  std::string getResourceName() { return resource_name; }
  void setShots(unsigned int _shots) { shots = _shots; }
  unsigned int getShots() { return shots; }
  void setNoModify(bool _no_modify) { no_modify = _no_modify; }
  bool isNoModify() { return no_modify; }
  void setQueued(bool _queued) { queued = _queued; }
  bool isQueued() { return queued; }

  json to_json() const {
    json job_json = {
        {"circuit", circuit},
        {"circuit_format", circuit_format},
        {"resource_name", resource_name},
        {"shots", shots},
        {"no_modify", no_modify},
        {"queued", queued},
    };

    return job_json;
  }

  std::string getPath() const { return "job"; }
};

class Hamiltonian_Job_Request : public Job_Request {
private:
  std::string resource_name;
  std::string interaction_str;
  std::string coefficients_str;

public:
  Hamiltonian_Job_Request(std::string resource_name,
                          std::string interaction_str,
                          std::string coefficients_str)
      : resource_name(resource_name), interaction_str(interaction_str),
        coefficients_str(coefficients_str) {}
  Hamiltonian_Job_Request() {};

  void setResourceName(std::string _resource_name) {
    resource_name = _resource_name;
  }
  std::string getResourceName() { return resource_name; }

  void setInteractionString(std::string _interaction_str) {
    interaction_str = _interaction_str;
  }
  std::string getInteractionString() { return interaction_str; }

  void setCoefficientsString(std::string _coefficients_str) {
    coefficients_str = _coefficients_str;
  }
  std::string getCoefficientsString() { return coefficients_str; }
  json to_json() const {
    json job_json = {{"resource_name", resource_name},
                     {"interaction_str", interaction_str},
                     {"coefficients_str", coefficients_str}};

    return job_json;
  }

  std::string getPath() const { return "hamiltonian_job"; }
};

class Job_Result {

public:
  std::map<std::string, unsigned int> results;
  std::string timestamp_completed;
  std::string timestamp_submitted;
  std::string timestamp_scheduled;

  Job_Result(std::map<std::string, unsigned int> results,
             std::string timestamp_completed, std::string timestamp_submitted,
             std::string timestamp_scheduled)
      : results(results), timestamp_completed(timestamp_completed),
        timestamp_submitted(timestamp_submitted),
        timestamp_scheduled(timestamp_scheduled) {}

  static Job_Result from_json(json &parsed) {
    const json &_parsed_job_result = parsed;

    std::string _circuit_result = _parsed_job_result.at("result");

    std::map<std::string, unsigned int> _circuit_result_dict;

    json _result_map = json::parse(_circuit_result);
    for (auto &[key, value] : _result_map.items()) {
      _circuit_result_dict[key] = value.get<unsigned int>();
    }

    return Job_Result(
        _circuit_result_dict,
        _parsed_job_result.at("timestamp_completed").get<std::string>(),
        _parsed_job_result.at("timestamp_submitted").get<std::string>(),
        _parsed_job_result.at("timestamp_scheduled").get<std::string>());
  }
};

class MQSSClient {
public:
  MQSSClient();
  MQSSClient(const std::string &token_or_queue, bool is_hpc);
  MQSSClient(const std::string &token, const std::string &url);

  // Devices
  std::vector<Device> getAllResources();
  std::optional<Device> getResourceInfo(const std::string &resource);

  // Jobs
  std::optional<std::string> submitJob(Job_Request &job);
  void cancelJob(Job_Request &job);
  std::string getJobStatus(Job_Request &job);
  std::unique_ptr<Job_Result> getJobResult(Job_Request &job);
  std::unique_ptr<Job_Result> waitForJobResult(Job_Request &job,
                                               size_t poll_seconds = 2);

  int getNumberPendingJobs(const std::string &resource);

private:
  std::unique_ptr<MQSS_Base_Client> client_;
};

} // namespace mqss::client
