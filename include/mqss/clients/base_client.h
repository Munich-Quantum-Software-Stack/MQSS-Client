#pragma once
#include <string>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class MQSS_Base_Client {
public:
  virtual ~MQSS_Base_Client() = default;
  virtual std::string get(const std::string &path) = 0;
  virtual std::string post(const std::string &path,
                           const std::string &data) = 0;
  virtual void cancel(const std::string &path) = 0;
};