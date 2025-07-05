#include "Widgets/mainwindow.h"
#include "Widgets/Terminal/terminal.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    createLog();
    qInstallMessageHandler(customMessageHandler);

    MainWindow w;
    w.show();
    return a.exec();
}
