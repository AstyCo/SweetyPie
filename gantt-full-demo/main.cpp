#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QtGlobal>

int main(int argc, char *argv[])
{

    QTextCodec *tc = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(tc);
#if QT_VERSION < 0x050000
    QTextCodec::setCodecForCStrings(tc);
    QTextCodec::setCodecForTr(tc);
#endif
    QApplication a(argc, argv);


    MainWindow w;

    w.show();

    return a.exec();
}
