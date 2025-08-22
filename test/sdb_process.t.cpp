#include <libsdb/sdb_process.h>

#include <filesystem>
#include <fstream>
#include <signal.h>
#include <sstream>
#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace sdb
{
namespace
{

using namespace testing;

bool processExists(int pid)
{
  const auto rc = kill(pid, 0);
  return (rc == 0) or (rc == -1 and errno == EPERM);
}

char getProcessStatus(int pid)
{
  std::stringstream ss;
  ss << "/proc/" << pid << "/stat";

  std::ifstream ifs(ss.str().c_str());

  if (!ifs.is_open())
  {
    return false;
  }

  // File names can contains spaces and parenthesis
  // But we can assume the contents of the stat file will
  // be in the format: 1 (systemd) S 0 1 1 0 -1 4194560 1403003 88311080 ...
  std::string buf;
  std::getline(ifs, buf);
  const auto idxOfLastParen = buf.rfind(')');
  const auto idxOfProcStatus = idxOfLastParen + 2;
  return buf[idxOfProcStatus];
}
} // namespace

TEST(ProcessTest, processLaunchedSuccessfully)
{
  // Given
  auto proc = Process::launch("ls");

  // When / Then
  ASSERT_THAT(proc, NotNull());
  auto pid = proc->pid();
  ASSERT_TRUE(processExists(pid))
    << "Process with pid=" << pid << " not found";
}

TEST(ProcessTest, launchNonExistentProgram)
{
  // Given / When / Then
  const std::filesystem::path procname{ "yess" };
  EXPECT_THROW(Process::launch(procname), ProcessLaunchError);
}

TEST(ProcessTest, attachToProcess)
{
  // Given
  const std::filesystem::path procname{ "ls" };
  const auto proc = Process::launch(procname, /*traceProc*/ false);
  ASSERT_THAT(proc, NotNull());

  // When
  const auto attachedProc = Process::attach(proc->pid());

  // Then
  ASSERT_EQ(proc->pid(), attachedProc->pid());
  auto processInTracingStoppedStatus
    = [](int pid) { return getProcessStatus(pid) == 't'; };

  ASSERT_TRUE(processInTracingStoppedStatus(attachedProc->pid()));
}

} // namespace sdb
