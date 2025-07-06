#ifndef SBB_PROCESS_H
#define SDB_PROCESS_H

#include <memory>
#include <filesystem>

namespace sdb
{

class Process;
using ProcessUPtr = std::unique_ptr<Process>;

class Process
{
private:
  /**
   * Private constructor.
   * Please use the launch/attach functions below
   */
  Process();

private:
  const pid_t d_pid;

public:
  static ProcessUPtr attach(pid_t pid);
  static ProcessUPtr launch(std::filesystem::path path);
};

}

#endif
