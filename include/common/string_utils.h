#pragma once

#include <string>
#include <sstream>
#include <vector>

namespace Common
{
  template<typename N, typename S> inline S ToString(N n)
  {
    std::basic_stringstream<typename S::value_type, std::char_traits<typename S::value_type>, std::allocator<typename S::value_type> > ss;
    ss << n;
    return ss.str();
  }

  std::vector<std::string> SplitString(const std::string &s, char delim);

  typedef std::vector<std::wstring> WStringList;

  typedef std::vector<std::string> StringList;

  std::string WideStringToString(const std::wstring& str);

  std::wstring StringToWideString(const std::string& str);
} // namespace Common
