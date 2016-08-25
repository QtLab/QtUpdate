#include "ui_mainwindow.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    a.connect(w.getUi()->cancelPushButton, SIGNAL(clicked(bool)),
              &a, SLOT(quit()));

    return a.exec();
}
