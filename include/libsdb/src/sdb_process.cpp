#include <libsdb/sdb_process.h>

namespace sdb
{
ProcessUPtr Process::attach(pid_t pid)
{
  return {};
}

ProcessUPtr Process::launch(std::filesystem::path path)
{
  return {};
}
}
