#include "mqss/client.h"
#include <regex>

using namespace mqss::client;

namespace {

const std::regex COUPLING_PATTERN(R"(\(\s*(\d+)\s*,\s*(\d+)\s*\))");

const std::regex GATE_PATTERN(R"('([^']+)')");

template <typename T, typename Converter>
std::vector<T> extractFromStringField(const nlohmann::json &response, const std::string &key,
                       const std::regex &pattern, Converter &&convert) {
  std::vector<T> result;

  const auto it = response.find(key);
  if (it == response.end() || !it->is_string())
    return result;

  const std::string &text = it->get_ref<const std::string &>();
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

} // namespace

Device::Device(std::string name, unsigned qubitCount, bool online,
               std::vector<std::pair<int, int>> couplingMap,
               std::vector<std::string> nativeGateset)
    : mName(std::move(name)), mQubitCount(qubitCount), mOnline(online),
      mCouplingMap(std::move(couplingMap)),
      mNativeGateset(std::move(nativeGateset)) {}

Device Device::fromJson(const nlohmann::json &response) {

  std::string name = response.value("name", "");
  unsigned qubitCount = response.value("qubits", 0);
  bool online = response.value("online", false);

  auto couplingMap = extractFromStringField<std::pair<int, int>>(
      response, "connectivity", COUPLING_PATTERN, [](const std::smatch &m) {
        return std::make_pair(std::stoi(m[1]), std::stoi(m[2]));
      });

  auto nativeGateset =
      extractFromStringField<std::string>(response, "instructions", GATE_PATTERN,
                           [](const std::smatch &m) { return m[1].str(); });

  return Device{name, qubitCount, online, std::move(couplingMap),
                std::move(nativeGateset)};
}
