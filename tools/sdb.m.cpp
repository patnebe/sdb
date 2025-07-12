#include <libsdb/sdb_libsdb.h>
#include <libsdb/sdb_process.h>
#include <libsdb/sdb_stringutil.h>

#include <iostream>
#include <stdexcept>
#include <unistd.h>

#include <sys/ptrace.h>
#include <sys/wait.h>

#include <editline/readline.h>
#include <sstream>
#include <string>
#include <utility>

namespace
{
using namespace sdb;

class CommandError : std::runtime_error
{
public:
  using std::runtime_error::runtime_error;
};

struct CommandOptions
{
  static std::string CONTINUE;
};
std::string CommandOptions::CONTINUE = "continue";

ProcessUPtr attach_to_running_proc(const char** argv)
{
  pid_t pid = std::atoi(argv[2]);

  if (pid <= 0)
  {
    const std::string err("Invalid pid");
    std::perror(err.c_str());
    throw std::invalid_argument(err);
  }

  return Process::attach(pid);
}

ProcessUPtr launch_new_proc_and_attach(const char** argv)
{
  const char* program_path = argv[1];
  const std::filesystem::path path(program_path);

  return Process::launch(path);
}

ProcessUPtr attach(int argc, const char** argv)
{
  ProcessUPtr process = nullptr;
  if (argc == 3 && argv[1] == std::string("-p"))
  {
    process = attach_to_running_proc(argv);
  }
  else
  {
    process = launch_new_proc_and_attach(argv);
  }
  return process;
}

/**
 * Parse the command to be executed and dispatch it
 * to the process.
 */
void handle_command(const ProcessUPtr& process_uptr, std::string_view line)
{
  std::cout << "pid = " << process_uptr->pid() << ", command = " << line
            << std::endl;

  // What commands do we wanna support
  static const std::string delimiter(" ");
  auto args = StringUtil::split(line, delimiter);
  const std::string& command = args[0];

  if (command == CommandOptions::CONTINUE)
  {
    process_uptr->resume();

    // Todo: This blocks the main thread
    // once the process has continued. This probably
    // should not be here
    // wait_on_signal(pid);
  }
  else
  {
    std::cout << "-> " << command << std::endl;
    // std::stringstream ss;
    // ss << "Invalid command=" << command << " provided";
    // throw CommandError(std::move(ss).str());
  }
}

} // namespace

int main(int argc, const char** argv)
{
  if (argc == 1)
  {
    std::cerr << "No arguments given\n";
    return -1;
  }

  const ProcessUPtr inferiorProc = attach(argc, argv);
  inferiorProc->wait_on_signal();

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
      // TODO: In the case where continue is called
      // and this process has been suspended
      handle_command(inferiorProc, line_str);
    }
  }
}
