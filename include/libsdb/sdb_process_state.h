#ifndef SBB_PROCESS_STATE_H
#define SDB_PROCESS_STATE_H

#include <ostream>
#include <type_traits>

namespace sdb
{

enum struct ProcessState
{
  e_STOPPED,
  e_RUNNING,
  e_EXITED,
  e_TERMINATED
};

} // sdb

#endif
