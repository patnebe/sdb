#include <libsdb/sdb_libsdb.h>
#include <libsdb/sdb_stringutil.h>

#include <iostream>
#include <stdexcept>
#include <unistd.h>

#include <sys/ptrace.h>
#include <sys/wait.h>

#include <editline/readline.h>
#include <sstream>
#include <string>

namespace
{
  using namespace sdb;

  /**
   * This is thrown when there's an error attaching
   * to a process
   */
  class ProcessAttachErrror : std::runtime_error
  {
  public:
    using std::runtime_error::runtime_error;
  };

  pid_t attach_to_running_proc(const char** argv)
  {
    pid_t pid = std::atoi(argv[2]);
    if (pid <= 0)
    {
      const std::string err("Invalid pid");
      std::perror(err.c_str());
      throw std::invalid_argument(err);
    }

    if (ptrace(PTRACE_ATTACH, pid, /*addr=*/nullptr, /*data=*/nullptr) < 0)
    {
      const std::string err("Failed to attach to process");
      std::perror(err.c_str());
      throw ProcessAttachErrror(err);
    }

    return pid;
  }

  pid_t launch_new_proc_and_attach(const char** argv)
  {
    pid_t pid = 0;
    const char* program_path = argv[1];
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
      if (execlp(program_path, program_path, nullptr) < 0)
      {
        std::perror("Exec failed");
        throw ProcessAttachErrror("Exec failed");
      }
    }
    return pid;
  }

  pid_t attach(int argc, const char** argv)
  {
    pid_t pid = 0;

    if (argc == 3 && argv[1] == std::string("-p"))
    {
      pid = attach_to_running_proc(argv);
    }
    else
    {
      pid = launch_new_proc_and_attach(argv);
    }
    return pid;
  }

  void handle_command(pid_t pid, std::string_view line)
  {
    (void)pid;
    std::cout << "pid = " << pid << ", command = " << line << std::endl;

    // What commands do we wanna support
    //
    static const std::string delimiter("");
    auto args = StringUtil::split(line, delimiter);
  }
} // namespace

int main(int argc, const char** argv)
{
  if (argc == 1)
  {
    std::cerr << "No arguments given\n";
    return -1;
  }

  pid_t pid = attach(argc, argv);

  int wait_status = 0;
  int options = 0;
  if (waitpid(pid, &wait_status, options) < 0)
  {
    std::perror("waitpid failed");
  }

  char* line = nullptr;
  while ((line = readline("sdb> ")) != nullptr)
  {
    std::string line_str;
    if (line == std::string_view(""))
    {
      free(line);
      if (history_length > 0)
      {
        line_str = history_list()[history_length - 1]->line;
      }
    }
    else
    {
      line_str = line;
      add_history(line);
      free(line);
    }

    if (!line_str.empty())
    {
      handle_command(pid, line_str);
    }
  }
}
