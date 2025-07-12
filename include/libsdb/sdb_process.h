#ifndef SBB_PROCESS_H
#define SDB_PROCESS_H

#include <libsdb/sdb_process_state.h>

#include <filesystem>
#include <memory>

namespace sdb
{

class Process;
using ProcessUPtr = std::unique_ptr<Process>;

/**
 * This is thrown when there's an error attaching
 * to a process.
 */
class ProcessAttachErrror : std::runtime_error
{
public:
  using std::runtime_error::runtime_error;
};

class Process
{
private:
  /**
   * Private constructor.
   * Please use the launch/attach functions below
   */
  Process(pid_t pid, bool cleanup_on_exit);

private:
  const pid_t d_pid;

  // TODO: Set this properly
  const ProcessState d_state;
  const bool d_cleanup_on_exit;

public:
  ~Process();
  static ProcessUPtr attach(pid_t pid);
  static ProcessUPtr launch(std::filesystem::path path);

  /* Resume the debugee process */
  void resume();

  /**
   * Suspend execution of the calling process
   * until the process represented by this object
   * changes state. If the "inferior" process has
   * changed state, this returns immediately.
   */
  void wait_on_signal();

  pid_t pid() const;
  ProcessState state() const;

private:
  Process() = delete;
  Process(const Process&) = delete;
  Process& operator=(const Process&) = delete;
};

// INLINE AND TEMPLATE DECLARATIONS
inline pid_t Process::pid() const
{
  return d_pid;
}

inline ProcessState Process::state() const
{
  return d_state;
}
}

#endif
