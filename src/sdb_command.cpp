#include <libsdb/sdb_command.h>
#include <libsdb/sdb_stringutil.h>

namespace sdb
{
namespace
{

template <typename... Args>
bool isCommandType(std::string_view commandType, Args&&... options)
{
  return ((commandType == options) || ...);
}

CommandType parseCommandType(const std::string& commandTypeStr)
{
  if (isCommandType(commandTypeStr, "continue", "c"))
  {
    return CommandType::e_CONTINUE;
  }

  if (isCommandType(commandTypeStr, "quit", "q"))
  {
    return CommandType::e_QUIT;
  }

  if (isCommandType(commandTypeStr, "run", "r"))
  {
    return CommandType::e_RUN;
  }

  return CommandType::e_UNKNOWN;
}
}

Command::Command(CommandType commandType,
                 std::vector<std::string>&& commandArgs)
    : d_type(commandType), d_commandArgs(std::move(commandArgs))
{
}

Command Command::parse(std::string_view commandStr)
{
  if (commandStr.empty())
  {
    throw std::invalid_argument("commandStr cannot be empty");
  }

  static const std::string delimiter(" ");
  auto args = StringUtil::split(commandStr, delimiter);

  const auto commandType = parseCommandType(args[0]);
  return Command(commandType, std::move(args));
}
}
