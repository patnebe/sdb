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
class ProcessAttachError : std::runtime_error
{
public:
  using std::runtime_error::runtime_error;
};

/**
 * This is thrown when there's an launching a process.
 */
class ProcessLaunchError : std::runtime_error
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

  ProcessState d_state;
  const bool d_cleanup_on_exit;

public:
  ~Process();
  static ProcessUPtr attach(pid_t pid);
  static ProcessUPtr launch(std::filesystem::path path);

  Process(Process&&) = default;
  Process& operator=(Process&&) = default;

  /* Resume the debugee process */
  void resume();

  /**
   * Suspend execution of the calling process
   * until the process represented by this object
   * changes state. If the "inferior" process has
   * changed state, this returns immediately.
   */
  void waitOnSignal();

  pid_t pid() const;
  ProcessState state() const;
  bool isAlive() const;

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

inline bool Process::isAlive() const
{
  return d_state != ProcessState::e_EXITED
         && d_state != ProcessState::e_TERMINATED;
}
}

#endif
