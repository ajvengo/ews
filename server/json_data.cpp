#include "json_data.hpp"
#include <rapidjson/document.h>

namespace ews {

json_data::status_type json_data::parse(const std::string& str) {
  attempts = 0;

  // parse JSON
  if (str.empty()) return json_parse_error;
  rapidjson::Document json;
  json.Parse(str.c_str());
  if (json.HasParseError()) return json_parse_error;

  const char key_data[] = "data";
  const char key_message[] = "message";
  const char key_attempts[] = "attempts";
  const char key_interval[] = "interval";

  // check that all required fields are present
  if (!json.IsObject() || !json.HasMember(key_data)) {
    return missing_data;
  }
  const rapidjson::Value& data = json[key_data];
  if (!data.IsObject() || !data.HasMember(key_message)) {
    return missing_message;
  }
  if (!data.HasMember(key_attempts)) {
    return missing_attempts;
  }
  if (!data.HasMember(key_interval)) {
    return missing_interval;
  }

  // check parameters types and values
  const rapidjson::Value& jmessage = data[key_message];
  if (!jmessage.IsString()) {
    return message_not_string;
  }
  const rapidjson::Value& jattempts = data[key_attempts];
  if (!jattempts.IsUint() || !jattempts.GetUint()) {
    return attempts_not_integer;
  }
  const rapidjson::Value& jinterval = data[key_interval];
  if (!(jinterval.IsUint() || jinterval.IsDouble()) || jinterval.GetDouble() < 0.001) {
    return interval_not_number;
  }

  // save results
  message = jmessage.GetString();
  attempts = jattempts.GetUint();
  interval = boost::posix_time::millisec(static_cast<unsigned>(jinterval.GetDouble() * 1e3));
  return ok;
}

/// Error message strings for JSON parser
static const std::string json_status_strings[] = {
  "",
  "JSON parse error",
  "data block is missing",
  "message parameter is missing",
  "attempts parameter is missing",
  "interval parameter is missing",
  "message is not a string",
  "attempts is not a positive integer",
  "interval is not a positive number"
};

const std::string& json_data::status_message(json_data::status_type status) {
  const int i(status), n(interval_not_number);
  return i <= n ? json_status_strings[i] : json_status_strings[0];
}

const std::string json_data::make_body(const std::string& tag, const std::string& value) {
  return "{\n \"" + tag + "\":{\n  \"message\":\"" + value + "\"\n }\n}";
}

} // namespace ews
