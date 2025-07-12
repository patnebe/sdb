#include <libsdb/sdb_process.h>

#include <iostream>
#include <signal.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>

namespace sdb
{
Process::Process(pid_t pid, bool cleanup_on_exit)
    : d_pid(pid), d_state(ProcessState::e_STOPPED),
      d_cleanup_on_exit(cleanup_on_exit)
{
}

Process::~Process()
{
  if (d_cleanup_on_exit)
  {
    kill(d_pid, SIGKILL);
  }
}

ProcessUPtr Process::attach(pid_t pid)
{
  if (ptrace(PTRACE_ATTACH, pid, /*addr=*/nullptr, /*data=*/nullptr) < 0)
  {
    const std::string err("Failed to attach to process");
    std::perror(err.c_str());
    throw ProcessAttachErrror(err);
  }
  constexpr bool cleanup_on_exit = false;
  return std::unique_ptr<Process>(new Process(pid, cleanup_on_exit));
}

ProcessUPtr Process::launch(std::filesystem::path path)
{
  pid_t pid = 0;
  if ((pid = fork()) < 0)
  {
    std::perror("fork failed");
    throw ProcessAttachErrror("fork failed");
  }

  if (pid == 0)
  {
    // In child process
    // Execute debugee
    if (ptrace(PTRACE_TRACEME, 0, nullptr, nullptr) < 0)
    {
      std::perror("Tracing failed");
      throw ProcessAttachErrror("Tracing failed");
    }
    if (execlp(path.c_str(), path.c_str(), nullptr) < 0)
    {
      std::perror("Exec failed");
      throw ProcessAttachErrror("Exec failed");
    }
  }

  constexpr bool cleanup_on_exit = true;
  return std::unique_ptr<Process>(new Process(pid, cleanup_on_exit));
}

void Process::resume()
{
  if (int rc = ptrace(PTRACE_CONT, d_pid, nullptr, nullptr); rc < 0)
  {
    // TODO: Track the state of the process so we don't
    // deliver the continue signal if the process is
    // already running.
    std::cerr << "Unable to continue, rc=" << rc << std::endl;
    std::exit(-1);
  }
}

void Process::wait_on_signal()
{
  int wait_status = 0;
  int options = 0;
  if (waitpid(d_pid, &wait_status, options) < 0)
  {
    std::perror("waitpid failed");
    std::exit(-1);
  }
}
}
