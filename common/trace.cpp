#include <common/trace.h>
#include <common/string_utils.h>

#include <algorithm>
#include <fstream>
#include <functional>
#include <cctype>
#include <locale>

namespace Common
{
  namespace
  {
    EventTrace* GlobalTrace = 0;

    struct tm CurrentTime()
    {
       time_t rawtime;
       rawtime = time(0);
#ifdef COMPILER_VS
       struct tm result;
       localtime_s(&result, &rawtime);
       return result;
#else
       return *localtime(&rawtime);
#endif
    }

    std::wstring TimeToStr(struct tm time)
    {
      std::string s;
      s = std::string(asctime(&time));
      s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
      return StringToWideString(s);
    }

    std::wstring DecodeLevel(EventLevel lvl)
    {
      switch (lvl)
      {
      case DEBUG:
        return L"debug";
      case INFORMATION:
        return L"information";
      case WARNING:
        return L"warning";
      case ERROR:
        return L"error";
      }
      return std::wstring();
    }
  }

  void Event(const Error& error)
  {
    if (!GlobalTrace)
    {
      return;
    }
    GlobalTrace->ConsumeEvent(CurrentTime(), static_cast<ModuleCode>(error.GetCode() >> 16), ERROR, Error::Format(error).c_str());
  }

  void Event(ModuleCode module, EventLevel level, const std::wstring& message)
  {
    if (!GlobalTrace)
    {
      return;
    }
    GlobalTrace->ConsumeEvent(CurrentTime(), module, level, message);
  }

  void RegisterTrace(EventTrace* trace)
  {
    UnregisterTrace();
    GlobalTrace = trace;
  }

  void UnregisterTrace()
  {
    delete GlobalTrace;
    GlobalTrace = 0;
  }

  class FileTrace : public EventTrace
  {
  public:
    FileTrace(const std::wstring& filePath);
    ~FileTrace();
    virtual void ConsumeEvent(const struct tm, ModuleCode module, EventLevel level, const std::wstring& message);
  private:
    std::wfstream fstr;
  };

  FileTrace::FileTrace(const std::wstring& filePath)
  {
    fstr.open(WideStringToString(filePath).c_str(), std::fstream::out | std::fstream::app);
  }

  FileTrace::~FileTrace()
  {
    fstr.close();
  }

  void FileTrace::ConsumeEvent(const struct tm time, ModuleCode module, EventLevel level, const std::wstring& message)
  {
    fstr << L"***** " << TimeToStr(time) << ": " << DecodeLevel(level) << L"; module " << module << std::endl;
    fstr << message << std::endl << std::endl;
  }

  EventTrace* CreateFileTrace(const std::wstring& filePath)
  {
    return new FileTrace(filePath);
  }
} // namespace Common
