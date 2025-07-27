#include <libsdb/sdb_process.h>

#include <iostream>
#include <signal.h>
#include <sstream>
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
  if (!isAlive())
  {
    return;
  }

  if (d_state == ProcessState::e_RUNNING)
  {
    kill(d_pid, SIGSTOP);
    waitOnSignal();
  }

  // Detach
  if (auto rc
      = ptrace(PTRACE_DETACH, d_pid, /*addr=*/nullptr, /*data=*/nullptr);
      rc < 0)
  {
    std::cout << "Unable to detach from process, rc=" << rc << std::endl;
    return;
  }

  kill(d_pid, SIGCONT);

  // Cleanup
  if (d_cleanup_on_exit)
  {
    kill(d_pid, SIGKILL);
    waitOnSignal();
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
  constexpr bool cleanupOnExit = false;
  auto proc = std::unique_ptr<Process>(new Process(pid, cleanupOnExit));
  proc->waitOnSignal();
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

  constexpr bool cleanupOnExit = true;
  auto proc = std::unique_ptr<Process>(new Process(pid, cleanupOnExit));
  proc->waitOnSignal();
  return proc;
}

void Process::resume()
{
  switch (d_state)
  {
    case ProcessState::e_RUNNING:
      std::cout << "Inferior process already running" << std::endl;
      break;
    case ProcessState::e_STOPPED:
      if (int rc = ptrace(PTRACE_CONT, d_pid, nullptr, nullptr); rc < 0)
      {
        std::cerr << "Unable to continue, rc=" << rc << std::endl;
        std::exit(-1);
      }
      d_state = ProcessState::e_RUNNING;
      waitOnSignal();
      break;
    default:
      std::cout << "Inferior process is not running" << std::endl;
  }
}

void Process::waitOnSignal()
{
  int waitStatus = 0;
  int options = 0;
  if (waitpid(d_pid, &waitStatus, options) < 0)
  {
    std::perror("waitpid failed");
    std::exit(-1);
  }

  if (WIFEXITED(waitStatus))
  {
    std::cout << "Inferior exited normally." << std::endl;
    d_state = ProcessState::e_EXITED;
    return;
  }

  if (WIFSIGNALED(waitStatus))
  {
    std::cout << "Inferior terminated by signal." << std::endl;
    d_state = ProcessState::e_TERMINATED;
    return;
  }
}
}
