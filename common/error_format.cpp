#include <common/error_format.h>

#include <sstream>

namespace Common
{
  namespace
  {
    std::wstring FormatStandaloneError(const Common::Error& err)
    {
      std::wstringstream ss;
      if (err == Common::Success)
      {
        return L"Success";
      }
      const Error::SourceLocation& loc = err.GetSourceLocation();
      if (loc.File != 0)
      {
        ss << loc.File << ": " << loc.Line << std::endl;
      }
      std::wstring kind = L"[APP]";
      if (!err.IsApplicationSpecific())
      {
        kind = L"[INT]";
      }
      ss << L"code "<< err.GetCode() << " " << kind << L" (0x" << std::hex << err.GetCode() << L"): " << err.GetMessage();
      return ss.str();
    }
  } // namespace

  std::wstring FormatError(const Common::Error& error)
  {
    std::wstringstream ss;
    ss << FormatStandaloneError(error);

    Error::Ptr sub = error.GetSubError();
    while(sub)
    {
      ss << std::endl << std::endl << FormatStandaloneError(*sub);
      sub = sub->GetSubError();
    }
    return ss.str();
  }
} // namespace Common
