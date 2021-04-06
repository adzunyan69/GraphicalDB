#include "settings.h"

Settings::Settings(QString filename)
{
    qDebug() << "Создание класса настроек";
    settings = new QSettings (QCoreApplication::applicationDirPath() + "/" + filename, QSettings::IniFormat);
    qDebug() << "Файл: " << QCoreApplication::applicationDirPath() + "/" + filename;

    settingsMap[WINDOW_ON_TOP] = "WindowOnTop";
    settingsMap[WINDOW_WIDTH] = "WindowWidth";
    settingsMap[WINDOW_HEIGHT] = "WindowHeight";
    settingsMap[WINDOW_Y_POSITION] = "WindowVerticalPosition";
    settingsMap[WINDOW_X_POSITION] = "WindowHorisontalPosition";

    settingsMap[DATABASE_PATH] = "DatabasePath";


    QVariant firstRunValue = settings->value("General/firstRun");
    if(!firstRunValue.isValid())
    {
        qDebug() << "Первый запуск приложения";
        // Первый запуск приложения
        settings->setValue("General/firstRun", false);

        firstRun = true;
    }
    else
        firstRun = false;
}

Settings::~Settings()
{
    qDebug() << "Удаление объекта настроек";
    delete settings;
}


void Settings::write(parametr param, const QVariant &value, const QString &section)
{
    qDebug() << "Установка параметра " << section << "/" << settingsMap[param] << ", значение " << value;
    settings->setValue(section + "/" + settingsMap[param], value);
    qDebug() << "Обновление настроек";
    settings->sync();
}

QVariant Settings::read(parametr param, const QString section)
{
    qDebug() << "Чтение параметра " << param << " / " << section;
    return settings->value(section + "/" + settingsMap[param]);
}
