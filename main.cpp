#include <QtWidgets/QApplication>
#include <QDesktopWidget>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    // set main window stat position/size
    //QDesktopWidget dw;
    //w.setGeometry(100,100,dw.width()*0.5,dw.height()*0.5);
    w.startTimer(100);
    w.show();
    
    return a.exec();
}
