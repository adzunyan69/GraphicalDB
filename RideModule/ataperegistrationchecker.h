#ifndef ATAPEREGISTRATIONCHECKER_H
#define ATAPEREGISTRATIONCHECKER_H

#include <QObject>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QTextCodec>
#include <QDomDocument>

struct ATapeRegistrationInfo
{
    QString passport;
    QString passportFilename;
    QString ATapeDataFolder;
    QString datapath;
    QString fullPathToPassport;

    QString roadBaseID;
    QString DIR_CODE;
    QString TRACK_NUM;
    QString TRACK_CODE;
    QString START_KM;
    QString END_KM;
    QString END_M;
    QString PICKET_BEAT;
    QString INCREASE;
    QString ORIENTATION;
    QString GUID;
    QString SITEID;
    
    QString rideType;
    QString startType;
    QString Instr_num;
    QString Zero_line;
    QString printDiagram;
    QString Cor_Coord;

    QDateTime regStartDate;
    QDateTime regEndDate;

    bool regStarted;
};

struct PIKParams
{
    
};

class ATapeRegistrationChecker : public QObject
{
    Q_OBJECT


    ATapeRegistrationInfo registrationInfo;

    void savePassportLog();

public:
    explicit ATapeRegistrationChecker(QObject *parent = nullptr);
    ATapeRegistrationInfo& getRegInfo() { return registrationInfo; }
    void setPathToPassport(QString pathToPassport) { registrationInfo.fullPathToPassport = pathToPassport; }
    QString error;
    void parsePassport(bool debugMode = false);
};

#endif // ATAPEREGISTRATIONCHECKER_H
