#ifndef RIDE_H
#define RIDE_H

#include <QObject>
#include <QSettings>
#include <QDebug>
#include <QTimer>
#include "ataperegistrationchecker.h"

struct RideInfo
{
    QString directionCode;
    QString trackNumber;
    QString trackCode;
    QString GUID;
    bool increase;

    QString toString()
    {
        return QString("Direction: %1 \n Track Number: %2 \n Track Code: %3 \n Increase: %4 \n")
                       .arg(directionCode)
                       .arg(trackNumber)
                       .arg(trackCode)
                       .arg(increase);
    }
};

class RideUpdater : public QObject
{
    Q_OBJECT
public:
    explicit RideUpdater(QObject *parent = nullptr) : QObject(parent) { }
    ~RideUpdater() { }

public:
    virtual void startUpdating() = 0;
    virtual RideInfo getRideInfo() = 0;
signals:
    void currentPathCoordChanged(QString pathCoord);
    void trackChanged();


};

// ATape
class RideUpdateWorker : public RideUpdater
{
    Q_OBJECT
private:
    QString registryPath;
    QSettings *registry = nullptr;


    RideInfo rideInfo;
    QTimer *timer = nullptr;
    QTimer *guidTimer = nullptr;

    QString currentPathCoord;

    void parseRegInfoToRideInfo(ATapeRegistrationInfo &regInfo);
public:
    explicit RideUpdateWorker(QObject *parent = nullptr) : RideUpdater(parent) { }
    ~RideUpdateWorker() { }
    void setRegistryPathAndRideInfo(QString _registryPath);
    void startUpdating() override;
    RideInfo getRideInfo() override;

private slots:
    void updatePathCoord();
    void checkGUIDChange();

};



#endif // RIDE_H
