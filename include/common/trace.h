#ifndef COMMON_EVENT_H_
#define COMMON_EVENT_H_

#include <common/error.h>
#include <common/module.h>

#include <string>
#include <time.h>

namespace Common
{
  enum EventLevel
  {
    DEBUG,
    INFORMATION,
    WARNING,
    ERROR
  };

  void Event(const Error& error);
  void Event(ModuleCode module, EventLevel level, const std::wstring& message);

  class EventTrace
  {
  public:
    virtual ~EventTrace(){}
    virtual void ConsumeEvent(const struct tm, ModuleCode module, EventLevel level, const std::wstring& message) = 0;
  };

  void RegisterTrace(EventTrace* trace);
  void UnregisterTrace();

  EventTrace* CreateFileTrace(const std::wstring& filePath);
  EventTrace* CreateStdErrTrace();
}

#define DEBUG(module, msg) Common::Event(module, Common::DEBUG, std::wstring(msg))
#define INFO(module, msg) Common::Event(module, Common::INFORMATION, std::wstring(msg))
#define WARNING(module, msg) Common::Event(module, Common::WARNING, std::wstring(msg))
#define ERROR(module, msg) Common::Event(module, Common::ERROR, std::wstring(msg))

#endif // COMMON_EVENT_H_
