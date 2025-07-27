#include <libsdb/sdb_command.h>
#include <libsdb/sdb_process.h>

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

void handle_command(const ProcessUPtr& process_uptr, const Command& command)
{
  switch (command.d_type)
  {
    case CommandType::e_RUN:
      if (process_uptr->isAlive())
      {
        std::cout << "Inferior process is already running" << std::endl;
      }
      return;

    case CommandType::e_CONTINUE:
      process_uptr->resume();
      return;

    default:
      std::cout << "Unknown command: " << command.d_commandArgs[0]
                << std::endl;
  }
}

void debug(const ProcessUPtr& inferiorProc)
{
  char* line = nullptr;
  while (true)
  {
    if ((line = readline("sdb> ")) == nullptr)
    {
      continue;
    }

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
      const auto command = Command::parse(line_str);
      switch (command.d_type)
      {
        case CommandType::e_QUIT:
          return;
        default:
          handle_command(inferiorProc, command);
      }
    }
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

  try
  {
    const ProcessUPtr inferiorProc = attach(argc, argv);
    debug(inferiorProc);
  }
  catch (const std::runtime_error& err)
  {
    std::perror(err.what());
  }
}
