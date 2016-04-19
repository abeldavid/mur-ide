#include <QApplication>
#include "widgets/mainwindow.h"

int main(int argc, char *argv[])
{
    qRegisterMetaType<StatusInfo>();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
