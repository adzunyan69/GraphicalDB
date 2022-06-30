#include "settings.h"

Settings::Settings(QString filename)
{
    qInfo() << "Создание класса настроек";
    settings = new QSettings (QCoreApplication::applicationDirPath() + "/" + filename, QSettings::IniFormat);
    qInfo() << "Файл: " << QCoreApplication::applicationDirPath() + "/" + filename;

    settingsMap[WINDOW_ON_TOP] = "WindowOnTop";
    settingsMap[WINDOW_WIDTH] = "WindowWidth";
    settingsMap[WINDOW_HEIGHT] = "WindowHeight";
    settingsMap[WINDOW_Y_POSITION] = "WindowVerticalPosition";
    settingsMap[WINDOW_X_POSITION] = "WindowHorisontalPosition";

    settingsMap[DATABASE_PATH] = "DirBDRad";


    QVariant firstRunValue = settings->value("General/firstRun");
    if(!firstRunValue.isValid())
    {
        qInfo() << "Первый запуск приложения";
        // Первый запуск приложения
        settings->setValue("General/firstRun", false);

        firstRun = true;
    }
    else
        firstRun = false;
}

Settings::~Settings()
{
    qInfo() << "Удаление объекта настроек";
    delete settings;
}


void Settings::write(parametr param, const QVariant &value, const QString &section)
{
    qInfo() << "Установка параметра " << section << "/" << settingsMap[param] << ", значение " << value;
    settings->setValue(section + "/" + settingsMap[param], value);
    qInfo() << "Обновление настроек";
    settings->sync();
}

QVariant Settings::read(parametr param, const QString section)
{
    qInfo() << "Чтение параметра " << param << " / " << section;
    return settings->value(section + "/" + settingsMap[param]);
}
