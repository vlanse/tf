#include <common/error.h>

namespace Common
{
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
} // namespace Common
