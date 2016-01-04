#pragma once

#include <string>
#include <memory>

namespace Common
{
  static const unsigned Success = 0;

  class Error
  {
  public:
    struct SourceLocation
    {
      SourceLocation(const char* file, unsigned line)
        : File(file)
        , Line(line)
      {
      }

      SourceLocation()
        : File(0)
        , Line(0)
      {
      }

      const char* File;
      unsigned Line;
    };

    typedef std::shared_ptr<Error> Ptr;

    Error();
    Error(unsigned code);
    Error(const SourceLocation& location, unsigned code);
    Error(const SourceLocation& location, unsigned code, const std::string& message, bool appSpecific = true);
    Error& AddSubError(const Common::Error& other);
    SourceLocation GetSourceLocation() const;
    unsigned GetCode() const;
    std::string GetMessage() const;
    bool IsApplicationSpecific() const;
    Ptr GetSubError() const;
    operator unsigned() const;

  private:
    SourceLocation Loc;
    unsigned Code;
    std::string Message;
    Ptr Sub;
    bool AppSpecific;
  };
} // namespace Common

// TODO: get rid of such errors as soon as possible
#define UNSPECIFIED_ERROR 0xffffffff

#define MAKE_ERROR(code, message) Common::Error(Common::Error::SourceLocation(__FILE__, __LINE__), code, std::string(message))
#define LAST_WINDOWS_ERROR() Common::GetLastWindowsError(Common::Error::SourceLocation(__FILE__, __LINE__))
#define QT_MAKE_ERROR(code, message) MAKE_ERROR(code, message.toStdString())

#define RETURN_IF_FAILED(x) do { Common::Error error = (x); if (error) return error; } while(0)
