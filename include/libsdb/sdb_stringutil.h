
#ifndef SBB_UTIL_H
#define SDB_UTIL_H

#include <vector>
#include <string>

namespace sdb
{
    struct StringUtil
    {
        static std::vector<std::string>
        split(std::string_view input, std::string& delimiter);

        /**
         * Returns true if the first string is a prefix of
         * the second
         */
        static bool is_prefix(
            std::string_view first, std::string_view second);
    };
}
 
#endif
