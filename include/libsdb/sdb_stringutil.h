#ifndef SBB_UTIL_H
#define SDB_UTIL_H

#include <string>
#include <vector>

namespace sdb
{
struct StringUtil
{
  static std::vector<std::string>
  split(const std::string_view input, const std::string& delimiter);

  /**
   * Returns true if the first string is a prefix of
   * the second
   */
  static bool
  is_prefix(const std::string_view first, const std::string_view second);
};
} // namespace sdb

#endif
