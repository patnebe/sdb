#include <libsdb/sdb_stringutil.h>

#include <stdexcept>

namespace sdb
{
    std::vector<std::string>
    StringUtil::split(std::string_view input, std::string &delimiter)
    {
        if (delimiter.empty())
        {
            throw std::invalid_argument("Delimiter cannot be empty");
        }

        std::vector<std::string> result;
        size_t nextTokenStart = 0;
        size_t delim_len = delimiter.size();

        const std::string inputStr(input);
        size_t end = inputStr.size();

        while (nextTokenStart < end)
        {
            const auto tokenEnd = inputStr.find(delimiter, nextTokenStart);
            result.push_back(inputStr.substr(nextTokenStart, tokenEnd - nextTokenStart));

            if (tokenEnd == inputStr.npos)
            {
                break;
            }

            nextTokenStart = tokenEnd + delim_len;
        }
        return result;
    }

    bool StringUtil::is_prefix(std::string_view first, std::string_view second)
    {
        return second.substr(0, first.size()) == first;
    }
}