#include <common/string_utils.h>

#include <codecvt>

namespace Common
{
  std::vector<std::string> SplitString(const std::string &s, char delim)
  {
    std::vector<std::string> elems;
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
}
