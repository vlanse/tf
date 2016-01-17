#include <common/string_utils.h>

#include <codecvt>

namespace Common
{
  StringList SplitString(const std::string &s, char delim)
  {
    StringList elems;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
      elems.push_back(item);
    }
    return elems;
  }

  std::string WideStringToString(const std::wstring& str)
  {
    typedef std::codecvt_utf8<wchar_t> convert_typeX;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(str);
  }

  std::wstring StringToWideString(const std::string& str)
  {
    typedef std::codecvt_utf8<wchar_t> convert_typeX;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
  }

  std::vector<char> StringToCStr(const std::string& s)
  {
    std::vector<char> buffer(s.size() + 1, 0);
    std::copy(s.begin(), s.end(), buffer.begin());
    return buffer;
  }

  std::vector<char> WideStringToCStr(const std::wstring& s)
  {
    return StringToCStr(WideStringToString(s));
  }
} // namespace Common
