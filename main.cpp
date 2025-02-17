#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();

    QCoreApplication::setOrganizationName("Hashika");
    QCoreApplication::setApplicationName("Billify");
    QCoreApplication::setApplicationVersion("1.0.0");
}
