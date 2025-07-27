#ifndef SBB_PROCESS_H
#define SDB_PROCESS_H

#include <stdexcept>
#include <string>
#include <vector>

namespace sdb
{

class CommandError : std::runtime_error
{
public:
  using std::runtime_error::runtime_error;
};

enum class CommandType
{
  e_UNKNOWN,
  e_QUIT,
  e_RUN,
  e_CONTINUE,
};

struct Command
{
  CommandType d_type;
  std::vector<std::string> d_commandArgs;

  Command(CommandType commandType, std::vector<std::string>&& commandArgs);
  static Command parse(std::string_view commandStr);
};

}

#endif
