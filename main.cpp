#include <QApplication>
#include <iostream>
#include "gui.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  gui   window;
  window.show();
  return app.exec();
}
