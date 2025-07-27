#include <libsdb/sdb_command.h>
#include <libsdb/sdb_stringutil.h>

namespace sdb
{
namespace
{

bool isCommandType(const auto& src, const auto& expectedOption1,
                   const auto& expectedOption2)
{
  return (src == expectedOption1) || (src == expectedOption2);
}

CommandType parseCommandType(const std::string& commandTypeStr)
{
  if (isCommandType(commandTypeStr, std::string_view("continue"),
                    std::string_view("c")))
  {
    return CommandType::e_CONTINUE;
  }

  if (isCommandType(commandTypeStr, std::string_view("quit"),
                    std::string_view("q")))
  {
    return CommandType::e_QUIT;
  }

  if (isCommandType(commandTypeStr, std::string_view("run"),
                    std::string_view("r")))
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
