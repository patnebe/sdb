#include <libsdb/sdb_pipe.h>

#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unistd.h>
#include <utility>

namespace sdb
{

namespace
{
static const int EMPTY_PIPE_FD = -1;
} // namespace

Pipe::Pipe(bool closeOnExec)
{
  if (auto rc = pipe2(d_fds, 0 | (closeOnExec ? O_CLOEXEC : 0)); rc < 0)
  {
    std::stringstream ss;
    ss << "Failed to create pipe, rc=" << rc;
    throw std::runtime_error(ss.str());
  }
}

Pipe::~Pipe()
{
  closeRead();
  closeWrite();
}

int Pipe::releaseRead()
{
  return std::exchange(d_fds[d_readFdIdx], EMPTY_PIPE_FD);
}

int Pipe::releaseWrite()
{
  return std::exchange(d_fds[d_writeFdIdx], EMPTY_PIPE_FD);
}

void Pipe::closeRead()
{
  if (const auto readFd = getRead(); readFd != EMPTY_PIPE_FD)
  {
    close(readFd);
    releaseRead();
  }
}

void Pipe::closeWrite()
{
  if (const auto writeFd = getWrite(); writeFd != EMPTY_PIPE_FD)
  {
    close(writeFd);
    releaseWrite();
  }
}

std::vector<std::byte> Pipe::read()
{
  constexpr int numBytesToRead = 1024;
  std::byte buff[numBytesToRead];
  if (auto numBytesRead
      = ::read(getRead(), static_cast<void*>(buff), numBytesToRead);
      numBytesRead > -1)
  {
    return std::vector<std::byte>(buff, buff + numBytesRead);
  }

  return std::vector<std::byte>{};
}

void Pipe::write(std::byte* source, std::size_t numBytes)
{
  if (auto numBytesWritten
      = ::write(getWrite(), static_cast<const void*>(source), numBytes);
      numBytesWritten != static_cast<ssize_t>(numBytes))
  {
    std::cerr << "Expected to write numBytes=" << numBytes
              << ", but got numBytesWritten=" << numBytesWritten << std::endl;
  }
}
} // namespace sdb
