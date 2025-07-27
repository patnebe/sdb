#include <libsdb/sdb_process.h>

#include <iostream>
#include <signal.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utility>

namespace sdb
{
Process::Process(pid_t pid, bool cleanup_on_exit)
    : d_pid(pid), d_state(ProcessState::e_STOPPED),
      d_cleanup_on_exit(cleanup_on_exit)
{
}

Process::~Process()
{
  if (d_pid == 0)
  {
    return;
  }

  // Stop the process
  if (d_state == ProcessState::e_RUNNING)
  {
    kill(d_pid, SIGSTOP);
    wait_on_signal();
  }

  // Detach
  if (ptrace(PTRACE_DETACH, d_pid, /*addr=*/nullptr, /*data=*/nullptr) < 0)
  {
    std::perror("Failed to attach to process");
  }
  kill(d_pid, SIGCONT);

  // Cleanup
  if (d_cleanup_on_exit)
  {
    kill(d_pid, SIGKILL);
    wait_on_signal();
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
  auto proc = std::unique_ptr<Process>(new Process(pid, cleanup_on_exit));
  proc->wait_on_signal();
  return proc;
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
  auto proc = std::unique_ptr<Process>(new Process(pid, cleanup_on_exit));
  proc->wait_on_signal();
  return proc;
}

void Process::resume()
{
  switch (d_state)
  {
  case ProcessState::e_RUNNING:
    std::cout << "Process already running" << std::endl;
    return;
  case ProcessState::e_STOPPED:
    if (int rc = ptrace(PTRACE_CONT, d_pid, nullptr, nullptr); rc < 0)
    {
      std::cerr << "Unable to continue, rc=" << rc << std::endl;
      std::exit(-1);
    }
    d_state = ProcessState::e_RUNNING;
    return;
  }
  std::unreachable();
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
