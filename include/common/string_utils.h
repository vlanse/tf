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

  typedef std::vector<std::wstring> WStringList;

  typedef std::vector<std::string> StringList;

  StringList SplitString(const std::string &s, char delim);

  std::string WideStringToString(const std::wstring& str);

  std::wstring StringToWideString(const std::string& str);

  std::vector<char> StringToCStr(const std::string& s);

  std::vector<char> WideStringToCStr(const std::wstring& s);
} // namespace Common
