#include "mainwindow.h"

#include <QApplication>
#include <QProcess>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    // w.show();


    //QProcess::startDetached("C:\\Windows\\explorer.exe", {});
    /*
    QString filePath = "C:/Users/svyat/Desktop/Syava_stroyova/Ф12-ГОЗА В А.docx"; // Use forward slashes
    //QDesktopServices::openUrl(QUrl(filePath));
    QUrl url = QUrl::fromLocalFile(QFileInfo(filePath).absoluteFilePath());
    QDesktopServices::openUrl(url);
    */

    return a.exec();
}
