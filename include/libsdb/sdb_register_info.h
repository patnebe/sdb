#ifndef SDB_REGISTER_INFO_H
#define SDB_REGISTER_INFO_H

#include <string>

#include <sys/user.h>

namespace sdb
{
enum class RegisterId
{
};

enum class RegisterType
{
  e_GPR,
  e_SUB_GPR,
  e_FPR,
  e_DR
};

enum class RegisterFormat
{
  e_UINT
};

struct RegisterInfo
{
  RegisterId     registerId;
  std::string    name; // Why did Sy use a string view here?
  std::int32_t   dwarfId;
  std::size_t    size;
  std::size_t    offset;
  RegisterType   type;
  RegisterFormat format;
};

// Global array for every register in the system
inline constexpr RegisterInfo GLOBAL_REGISTER_INFOS[] = {
  // ?
};
}

#endif // SDB_REGISTER_INFO_H
