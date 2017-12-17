#pragma once

namespace Common
{
  // New module identifiers must be added only to the end of the enumeration
  // Changing previously added elements or their order is strictly prohibited

  enum ModuleCode
  {
    MODULE_TEST = 1,
    MODULE_COMMON,
    MODULE_FILE_MANAGER,
    MODULE_OS,
  };

#define MAKE_MODULE_ERROR(module, code) (module << 16) | code
} // namespace Common
