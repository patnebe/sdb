#include <libsdb/sdb_process.h>

#include <filesystem>
#include <signal.h>
#include <sstream>

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

} // namespace

TEST(ProcessTest, processLaunchedSuccessfully)
{
  // Given
  auto proc = Process::launch("yes");

  // When / Then
  ASSERT_THAT(proc, NotNull());
  auto pid = proc->pid();
  ASSERT_TRUE(processExists(pid))
    << "Process with pid=" << pid << " not found";
}

TEST(ProcessTest, launchNonExistentProgram)
{
  // Given / When / Then
  const std::filesystem::path path{ "yess" };
  EXPECT_THROW(Process::launch(path), ProcessLaunchError);
}

} // namespace sdb
