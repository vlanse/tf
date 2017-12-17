#include <common/trace.h>

#include <QApplication>

#include "main_window.h"


int main(int argc, char** argv)
{
  Common::RegisterTrace(Common::CreateStdErrTrace());

  QApplication app(argc, argv);
  QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

  TotalFinder::MainWindow* main = new TotalFinder::MainWindow();
  main->show();
  return app.exec();
}
