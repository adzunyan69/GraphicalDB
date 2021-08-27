#include "ride.h"


void RideUpdateWorker::setRegistryPathAndRideInfo(QString _registryPath)
{
    qDebug() << "Registry path: " << _registryPath;
    registryPath = _registryPath;
    registry = new QSettings(registryPath, QSettings::Registry32Format, this);

    qDebug() << "Registry path: " << registryPath;

    ATapeRegistrationChecker atape;
    atape.setPathToPassport(registry->value("PassportPath").toString());
    atape.parsePassport();
    parseRegInfoToRideInfo(atape.getRegInfo());

}

void RideUpdateWorker::parseRegInfoToRideInfo(ATapeRegistrationInfo &regInfo)
{
    rideInfo.GUID = regInfo.GUID;
    rideInfo.directionCode = regInfo.DIR_CODE;
    rideInfo.trackNumber = regInfo.TRACK_NUM;
    rideInfo.trackCode = regInfo.TRACK_CODE;
    rideInfo.increase = regInfo.INCREASE == "1" ? true : false;
    qDebug() << "Parse reg info: " << rideInfo.trackCode << " / " << rideInfo.increase;
}

RideInfo RideUpdateWorker::getRideInfo()
{
    if(registry != nullptr)
    {
        qDebug() << "Registry exist";
    }
    else
    {
        qDebug() << "Registry ERROR";
    }
    return rideInfo;

}

void RideUpdateWorker::startUpdating()
{
    qDebug() << "Start updating";
    if(timer != nullptr)
        delete timer;

    timer = new QTimer(this);
    timer->setInterval(1000);

    if(guidTimer != nullptr)
        delete guidTimer;
    guidTimer = new QTimer(this);
    guidTimer->setInterval(100);

    qDebug() << "Updating";
    connect(guidTimer, SIGNAL(timeout()), this, SLOT(checkGUIDChange()));
    connect(timer, SIGNAL(timeout()), this, SLOT(updatePathCoord()));
    timer->start();
    guidTimer->start();
}

void RideUpdateWorker::updatePathCoord()
{
    if(registry != nullptr)
        currentPathCoord = registry->value("CurPathCoord").toString();

    // qDebug() << currentPathCoord;
    emit currentPathCoordChanged(currentPathCoord);
}

void RideUpdateWorker::checkGUIDChange()
{
    if(registry != nullptr)
    {
        QString str = registry->value("path_name").toString();
        if(registry->value("path_name").toString() != rideInfo.GUID)
        {
            qDebug() << "GUID changed.";
            setRegistryPathAndRideInfo(registryPath);
            emit trackChanged();
        }
    }
}

