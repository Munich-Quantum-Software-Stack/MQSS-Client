#pragma once

#include <regex>
#include <string>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

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
