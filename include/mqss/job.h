#include <string>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Job_Request {
  std::string uuid;

public:
  virtual ~Job_Request() = default;
  virtual std::string to_json_str() const = 0;
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

  std::string to_json_str() const {
    json job_json = {
        {"circuit", circuit},
        {"circuit_format", circuit_format},
        {"resource_name", resource_name},
        {"shots", shots},
        {"no_modify", no_modify},
        {"queued", queued},
    };

    return job_json.dump();
  }

  std::string getPath() { return "job/"; }
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
  std::string to_json_str() const {
    json job_json = {{"resource_name", resource_name},
                     {"interaction_str", interaction_str},
                     {"coefficients_str", coefficients_str}};

    return job_json.dump();
  }

  std::string getPath() { return "hamiltonian_job/"; }
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

static Job_Result from_json(json& parsed) {
    const json& _parsed_job_result = parsed;

    std::string _circuit_result = _parsed_job_result.at("result");

    std::map<std::string, unsigned int> _circuit_result_dict;

    json _result_map = json::parse(_circuit_result);
    for (auto& [key, value] : _result_map.items()) {
        _circuit_result_dict[key] = value.get<unsigned int>();
    }

    return Job_Result(
        _circuit_result_dict,
        _parsed_job_result.at("timestamp_completed").get<std::string>(),
        _parsed_job_result.at("timestamp_submitted").get<std::string>(),
        _parsed_job_result.at("timestamp_scheduled").get<std::string>()
    );
}

};