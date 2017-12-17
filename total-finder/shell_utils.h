#pragma once

#include <QString>

namespace TotalFinder
{
  namespace Shell
  {
    void OpenEditorForFile(const QString& file);
    void OpenTerminal(const QString& path);
    void RevealInFinder(const QString& path);
  } // namespace Shell
} // namespace TotalFinder
