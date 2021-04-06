#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QMap>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>

class Settings : QObject
{
    Q_OBJECT
public:
    enum parametr{ WINDOW_ON_TOP,
                 WINDOW_WIDTH,
                 WINDOW_HEIGHT,
                 WINDOW_Y_POSITION,
                 WINDOW_X_POSITION,

                 DATABASE_PATH};

private:
    QMap<parametr, QString> settingsMap;
    bool firstRun;

    QSettings *settings;
public:
    Settings(QString filename = "settings.conf");
    ~Settings();


    void write(parametr param, const QVariant &value, const QString &section = "Settings");
    QVariant read(parametr param, const QString section = "Settings");

    bool isFirstRun() { return firstRun; }


};

#endif // SETTINGS_H
