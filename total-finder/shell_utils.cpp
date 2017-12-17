#include "shell_utils.h"

#include <QDebug>
#include <QProcess>
#include <QStringList>

namespace TotalFinder
{
  namespace Shell
  {
    void OpenEditorForFile(const QString& file)
    {
      QStringList args;
      args << "-a" << "Sublime Text" << file;
      qDebug() << "Open editor for file with args " << args;
      QProcess::startDetached("open", args);
    }

    void OpenTerminal(const QString& path)
    {
      QStringList args;
      args << "-a" << "iterm" << path;
      qDebug() << "Open terminal for path" << path;
      QProcess::startDetached("open", args);
    }

    void RevealInFinder(const QString& path)
    {
      QStringList args;
      args << "-R" << path;
      qDebug() << "Reveal path in finder" << path;
      QProcess::startDetached("open", args);
    }
  } // namespace Shell
} // namespace TotalFinder
