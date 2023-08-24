#include "verify_parameter.hpp"

#include <regex>

namespace tools {

static constexpr int MIN_SIZE_OF_PSWD = 8;
static constexpr int MAX_AGE = 150;

#define MAIL_REGEX "(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+"

bool IsValidPasswd(const std::string& s) {
  return s.size() >= MIN_SIZE_OF_PSWD;
}

bool IsValidEmail(const std::string& s) {
  const std::regex pattern(MAIL_REGEX);
  return std::regex_match(s, pattern);
}

bool IsValidId(int id) { return id >= 1; }

bool IsValidAge(int age) { return age >= 1 && age <= MAX_AGE; }

bool IsValidGender(const std::string& s) {
  return s == "male" || s == "female";
}

bool IsValidStatus(const std::string& s) {
  return s == "offline" || s == "online";
}

}  // namespace tools