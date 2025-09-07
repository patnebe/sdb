#ifndef SDB_REGISTER_INFO_H
#define SDB_REGISTER_INFO_H

#include <algorithm>
#include <stdexcept>
#include <string>

#include <cstddef>
#include <sys/user.h>

namespace sdb
{

enum class RegisterId
{
#define DEFINE_REGISTER(name, dwarfId, size, offset, type, format) name
#include <libsdb/detail/registers.h>
#undef DEFINE_REGISTER
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
  e_UINT,
  e_LONG_DOUBLE,
  e_VECTOR
};

#include <string_view>

struct RegisterInfo
{
  RegisterId       registerId;
  std::string_view name; // Use string_view for constexpr compatibility
  std::int32_t     dwarfId;
  std::size_t      size;
  std::size_t      offset;
  RegisterType     type;
  RegisterFormat   format;
};

// Global array for every register in the system
inline constexpr RegisterInfo GLOBAL_REGISTER_INFOS[] = {
#define DEFINE_REGISTER(name, dwarfId, size, offset, type, format)            \
  { RegisterId::name, #name, dwarfId, size, offset, type, format }
#include <libsdb/detail/registers.h>
#undef DEFINE_REGISTER
};

// Helper methods to find register info
template <class Predicate> RegisterInfo registerInfoBy(Predicate pred)
{
  auto it = std::find_if(std::begin(GLOBAL_REGISTER_INFOS),
                         std::end(GLOBAL_REGISTER_INFOS), pred);
  if (it == std::end(GLOBAL_REGISTER_INFOS))
  {
    throw std::runtime_error("Failed to find registerInfo");
  }
  return *it;
}

// Inline definition in this header file
// prevents violation of the ODR
inline const RegisterInfo registerInfoById(const RegisterId registerId)
{
  const auto idPred = [registerId](const RegisterInfo& reg)
  { return reg.registerId == registerId; };
  return registerInfoBy(idPred);
}

inline const RegisterInfo registerInfoByName(std::string_view name)
{
  const auto namePred
    = [name](const RegisterInfo& reg) { return reg.name == name; };
  return registerInfoBy(namePred);
}

inline const RegisterInfo registerInfoByDwarf(std::int32_t dwarfId)
{
  const auto dwarfPred
    = [dwarfId](const RegisterInfo& reg) { return reg.dwarfId == dwarfId; };
  return registerInfoBy(dwarfPred);
}
}

#endif // SDB_REGISTER_INFO_H
