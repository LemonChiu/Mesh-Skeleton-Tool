#include "mainwindow.h"
#include <QtGui/QApplication>
#include <QCleanlooksStyle>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle(new QCleanlooksStyle);
    MainWindow w;
    w.showMaximized();
    return app.exec();
}