/*
 * main.cpp
 *
 *  Created on: Dec 19, 2015
 *      Author: Vladimir Semenov
 */

#include <QApplication>

#include "main_window.h"


int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  TF::MainWindow* main = new TF::MainWindow();
  main->show();
  return app.exec();
}
