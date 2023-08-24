#include <string>

namespace tools {

bool IsValidPasswd(const std::string& s);
bool IsValidEmail(const std::string& s);
bool IsValidId(int id);
bool IsValidAge(int age);
bool IsValidGender(const std::string& s);
bool IsValidStatus(const std::string& s);

}  // namespace tools