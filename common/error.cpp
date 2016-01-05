#include <common/error.h>

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

  Error::Error()
    : Code(Success)
    , AppSpecific(true)
  {
  }

  Error::Error(unsigned code)
    : Code(code)
    , AppSpecific(true)
  {
  }

  Error::Error(const SourceLocation& location, unsigned code)
    : Loc(location)
    , Code(code)
    , AppSpecific(true)
  {
  }

  Error::Error(const Error::SourceLocation& location, unsigned code, const std::wstring& message, bool appSpecific)
    : Loc(location)
    , Code(code)
    , Message(message)
    , AppSpecific(appSpecific)
  {
  }

  Error::operator unsigned() const
  {
    return Code;
  }

  unsigned Error::GetCode() const
  {
    return Code;
  }

  std::wstring Error::GetMessage() const
  {
    return Message;
  }

  Error::SourceLocation Error::GetSourceLocation() const
  {
    return Loc;
  }

  bool Error::IsApplicationSpecific() const
  {
    return AppSpecific;
  }

  Error& Error::AddSubError(const Error& other)
  {
    Sub.reset(new Error(other));
    return *this;
  }

  Error::Ptr Error::GetSubError() const
  {
    return Sub;
  }

  std::wstring Error::Format(const Common::Error& error)
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
