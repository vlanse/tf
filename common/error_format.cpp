#include <common/error_format.h>

#include <sstream>

namespace Common
{
  namespace
  {
    std::string FormatStandaloneError(const Common::Error& err)
    {
      std::stringstream ss;
      if (err == Common::Success)
      {
        return "Success";
      }
      const Error::SourceLocation& loc = err.GetSourceLocation();
      if (loc.File != 0)
      {
        ss << loc.File << ": " << loc.Line << std::endl;
      }
      std::string kind = "[APP]";
      if (!err.IsApplicationSpecific())
      {
        kind = "[INT]";
      }
      ss << "code "<< err.GetCode() << " " << kind << " (0x" << std::hex << err.GetCode() << "): " << err.GetMessage();
      return ss.str();
    }
  } // namespace

  std::string FormatError(const Common::Error& error)
  {
    std::stringstream ss;
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
