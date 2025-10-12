#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <QPalette>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPalette lightPalette;
    lightPalette.setColor(QPalette::Window, QColor(240, 240, 240));
    lightPalette.setColor(QPalette::WindowText, Qt::black);
    a.setStyle(QStyleFactory::create("Fusion"));
    a.setPalette(lightPalette);
    MainWindow w;
    w.show();
    return a.exec();
}
