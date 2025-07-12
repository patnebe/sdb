#include <libsdb/sdb_process_state.h>

#include <ostream>
#include <type_traits>

namespace sdb
{

std::ostream& operator<<(std::ostream& os, ProcessState state)
{
  switch (state)
  {
  case ProcessState::e_STOPPED:
    os << "STOPPED";
    break;
  case ProcessState::e_EXITED:
    os << "EXITED";
    break;
  case ProcessState::e_RUNNING:
    os << "RUNNING";
    break;
  case ProcessState::e_TERMINATED:
    os << "TERMINATED";
    break;

  default:
    os << "UNEXPECTED_STATE[" << state << "]";
    break;
  }
  return os;
}
}
