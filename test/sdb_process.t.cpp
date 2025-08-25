#include <libsdb/sdb_process.h>

#include <filesystem>
#include <fstream>
#include <signal.h>
#include <sstream>
#include <string>
#include <thread>

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
  // i.e the proc state is the third field
  // See https://man7.org/linux/man-pages/man5/proc_pid_stat.5.html
  std::string buf;
  std::getline(ifs, buf);
  const auto idxOfLastParen = buf.rfind(')');
  const auto idxOfProcStatus = idxOfLastParen + 2;
  return buf[idxOfProcStatus];
}

const auto processInTracingStoppedStatus
  = [](int pid) { return getProcessStatus(pid) == 't'; };

const auto processInRunningStatus
  = [](int pid) { return getProcessStatus(pid) == 'R'; };
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
  const std::filesystem::path procname{ "./testbin.tsk" };
  // Don't trace the process on launch
  // since we'll attach to it below
  const auto proc = Process::launch(procname, /*traceProc*/ false);
  ASSERT_THAT(proc, NotNull());

  // When
  const auto attachedProc = Process::attach(proc->pid());

  // Then
  ASSERT_EQ(proc->pid(), attachedProc->pid());
  ASSERT_TRUE(processInTracingStoppedStatus(attachedProc->pid()));
}

TEST(ProcessTest, attachToInvalidPid)
{
  // Given / When / Then
  EXPECT_THROW(Process::attach(0), ProcessAttachError);
}

TEST(ProcessTest, resumeAProcess)
{
  // Given
  auto proc = Process::launch("./testbin.tsk");
  ASSERT_THAT(proc, NotNull());
  const auto pid = proc->pid();
  ASSERT_TRUE(processInTracingStoppedStatus(pid));
  const auto procState = proc->state();
  ASSERT_EQ(procState, ProcessState::e_STOPPED)
    << "unexpected proc state=" << procState;

  // When
  // TODO: Understand why this fails with the following
  // error when I move it to a different thread::
  // Unable to continue proc with pid=137233, rc=-1
  // No such process
  proc->resume();

  // Then
  ASSERT_TRUE(processInRunningStatus(pid));
}

} // namespace sdb
