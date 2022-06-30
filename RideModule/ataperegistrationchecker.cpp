#include "ataperegistrationchecker.h"

ATapeRegistrationChecker::ATapeRegistrationChecker(QObject *parent) : QObject(parent)
{
}

void ATapeRegistrationChecker::parsePassport(bool debugMode)
{  
    QDomDocument doc;
    bool isDocCreated;

    QString filePath;
    if(debugMode)
        filePath = QDir::currentPath() + "/log/ATapeLog/debugPassport.xml";
    else
        filePath = registrationInfo.fullPathToPassport; //QDir::currentPath() + "/log/ATapeLog/" + registrationInfo.passportFilename + ".xml";

    qInfo() << "Passport path: " << filePath;
    QFile file(filePath);
    if(!file.exists())
    {
        error = "Файл паспорта не существует (либо не сохранен): " + filePath + "";
        qInfo() << error;
        return;
    }
    file.open(QFile::ReadOnly);
    QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
    isDocCreated = doc.setContent(codec->toUnicode(file.readAll()));
    file.close();

    if(!isDocCreated)
    {

        error = "Ошибка создания DOM-документа (XML-структуры).";
        qInfo() << error;
        return;
    }

    QDomElement root = doc.documentElement();
    QDomNode registrationDataNode = root.firstChild();
    while(registrationDataNode.toElement().tagName() != "REGISTRATION_DATA" && registrationDataNode.isNull() == false)
        registrationDataNode = registrationDataNode.nextSibling();

    registrationInfo.GUID = registrationDataNode.toElement().attribute("NAME");
    QDomNode dataNode = registrationDataNode.firstChild();
    while(dataNode.isNull() == false)
    {
        QDomElement dataElement = dataNode.toElement();
        QString inner = dataElement.attribute("INNER");
        QString value = dataElement.attribute("VALUE");

        if(inner == "DIR_CODE") registrationInfo.DIR_CODE = value; // Код направления
        else if(inner == "TRACK_NUM") registrationInfo.TRACK_NUM = value; // Номер пути
        else if(inner == "TRACK_CODE") registrationInfo.TRACK_CODE = value; // Код пути ЕКАСУИ
        else if(inner == "START_KM") registrationInfo.START_KM = value;// Стартовый КМ
        else if(inner == "PICKET_BEAT") registrationInfo.PICKET_BEAT = value; // Отбивать пикеты
        else if(inner == "INCREASE") registrationInfo.INCREASE = value;// Увеличение километров
        else if(inner == "ORIENTATION") registrationInfo.ORIENTATION = value; // Котлом вперед
        else if(inner == "DATE") registrationInfo.regStartDate = QDateTime::fromString(value, "yyyy:MM:dd:hh:mm");
        else if(inner == "STOP_DATE") registrationInfo.regEndDate = QDateTime::fromString(value, "yyyy:MM:dd:hh:mm");
        else if(inner == "END_CHOORD") { registrationInfo.END_KM = value.split(':').at(0); registrationInfo.END_M = value.split(':').at(1); }


        dataNode = dataNode.nextSibling();
    }

    qInfo() << "RegistrationInfo: " << "\n"
             << "DIR_CODE: " << registrationInfo.DIR_CODE << "\n"
             << "TRACK_NUM: LATIN1" << registrationInfo.TRACK_NUM << "\n"
             << "TRACK_CODE: " << registrationInfo.TRACK_CODE << "\n"
             << "START_KM: " << registrationInfo.START_KM << "\n"
             << "PICKET_BEAT: " << registrationInfo.PICKET_BEAT << "\n"
             << "INCREASE: " << registrationInfo.INCREASE << "\n"
             << "ORIENTATION: " << registrationInfo.ORIENTATION << "\n"
             << "DATE: " << registrationInfo.regStartDate.toString() << "\n"
             << "GUID: " << registrationInfo.GUID << "\n";



}

void ATapeRegistrationChecker::savePassportLog()
{
    // Временное сохранение бекап-файла паспорта
    QFile file(QDir::currentPath() + "/log/ATapeLog/" + registrationInfo.passportFilename + ".xml");
    qInfo() << "Паспорт сохранен: " << file.fileName();
    file.open(QFile::WriteOnly);
    QTextStream out(&file);
    out.flush();
    out << registrationInfo.passport;
    file.close();
}
