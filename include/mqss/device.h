#include <string>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>
namespace mqss::client {
class Device {
public:
  Device(std::string name, unsigned qubitCount, bool online,
         std::vector<std::pair<int, int>> couplingMap,
         std::vector<std::string> nativeGateset);

  const std::string &getName() const noexcept { return mName; }
  unsigned getQubitCount() const noexcept { return mQubitCount; }
  bool isOnline() const noexcept { return mOnline; }

  const std::vector<std::pair<int, int>> &getCouplingMap() const noexcept {
    return mCouplingMap;
  }

  const std::vector<std::string> &getNativeGateset() const noexcept {
    return mNativeGateset;
  }

  static Device fromJson(const nlohmann::json &response);

private:
  std::string mName;
  unsigned mQubitCount;
  bool mOnline;
  std::vector<std::pair<int, int>> mCouplingMap;
  std::vector<std::string> mNativeGateset;
};
} // namespace mqss::client
