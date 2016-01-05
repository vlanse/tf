#ifndef COMMON_ERROR_FORMAT_H_
#define COMMON_ERROR_FORMAT_H_

#include <common/error.h>

#include <string>

namespace Common
{
  std::wstring FormatError(const Common::Error& error);
} // namespace Common

#endif // COMMON_ERROR_FORMAT_H_
