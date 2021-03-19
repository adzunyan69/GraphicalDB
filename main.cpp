#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QDateTime>
#include <QDir>
#include <QFile>

void myMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString &msg);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qInstallMessageHandler(myMessageHandler);


    if(argc != 2 || !argv[1])
    {
        qDebug() << "Usage; program <path to reg>";
        return 1;
    }

    if(strstr(argv[1], "Software") != argv[1])
    {
        qDebug() << "Invalid path to registry";
        return 1;
    }


    MainWindow w;
    //w.setRegistryInfo(QString("HKEY_CURRENT_USER\\") + QString(argv[1]));
    //w.startPathCoordUpdater();
    w.show();
    return a.exec();
}


void myMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString &msg)
{
    // static QTextCodec *cp1251 = QTextCodec::codecForName("CP1251");
    QString txt;
    static long long uid=0; //-- номеруем вывод
    //-- название функции с классом, берём только класс и саму функцию
    QRegExp rx("([\\w-]+::[\\w-]+)");
    if (rx.indexIn(context.function) ==-1) return;
    QString function = rx.cap(1);

    QString msgSep = (msg.length()>0)? ">> " : "";

    switch (type) {
        case QtInfoMsg:
            txt = QString("Info: %1%2%3").arg(function).arg(msgSep).arg(msg);
        break;
        case QtDebugMsg:
            txt = QString("Debug: %1%2%3").arg(function).arg(msgSep).arg(msg);
            break;
        case QtWarningMsg:
            txt = QString("Warning: %1%2%3").arg(function).arg(msgSep).arg(msg);
        break;
        case QtCriticalMsg:
            txt = QString("Critical: %1%2%3").arg(function).arg(msgSep).arg(msg);
        break;
        case QtFatalMsg:
            txt = QString("Fatal: %1%2%3").arg(function).arg(msgSep).arg(msg);
            abort();
        break;
    }

    QDateTime dateTime = QDateTime::currentDateTime();
    uid++;
    txt=QString("%1:%2 %3").arg(dateTime.toString(Qt::ISODate)).arg(uid).arg(txt);

    if(!QDir().exists(QApplication::applicationDirPath() + "\\log\\"))
    {
        QDir().mkpath(QApplication::applicationDirPath() + "\\log\\");
    }
    QFile outFile(QApplication::applicationDirPath() + "\\log\\" + QString("%1/log-%2.log").arg(".").arg(QDate::currentDate().toString("yy.MM.dd")));
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    // txt = cp1251->toUnicode(txt.toUtf8());
    ts << txt << endl;
    outFile.close();


}
