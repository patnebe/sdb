#ifndef SBB_PROCESS_PIPE_H
#define SDB_PROCESS_PIPE_H

#include <vector>

namespace sdb
{

/* @brief Abstraction over an unbounded Pipe used for IPC */
class Pipe
{
public:
  explicit Pipe(bool closeOnExec);
  ~Pipe();

  Pipe(Pipe&&) = default;
  Pipe& operator=(const Pipe&) = default;

  int getRead() const;
  int getWrite() const;

  int releaseRead();
  int releaseWrite();

  void closeRead();
  void closeWrite();

  std::vector<std::byte> read();
  void write(std::byte* source, std::size_t numBytes);

private:
  static const unsigned d_readFdIdx = 0;
  static const unsigned d_writeFdIdx = 1;
  int d_fds[2];

private:
  Pipe(Pipe&) = delete;
  Pipe& operator=(Pipe&&) = delete;
};

// INLINE AND TEMPLATE DECLARATIONS
inline int Pipe::getRead() const
{
  return d_fds[Pipe::d_readFdIdx];
}

inline int Pipe::getWrite() const
{
  return d_fds[Pipe::d_writeFdIdx];
}

} // namespace sdb

#endif // SDB_PROCESS_PIPE_H
