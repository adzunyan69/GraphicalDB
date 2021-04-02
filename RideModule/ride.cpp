#include "ride.h"

RideUpdateWorker::RideUpdateWorker(QObject *parent) : RideUpdater(parent)
{

}

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

    qDebug() << "Updating";
    connect(timer, SIGNAL(timeout()), this, SLOT(updatePathCoord()));
    timer->start();
}

void RideUpdateWorker::updatePathCoord()
{
    if(registry != nullptr)
        currentPathCoord = registry->value("CurPathCoord").toString();

    // qDebug() << currentPathCoord;
    emit currentPathCoordChanged(currentPathCoord);
}

RideUpdateWorker::~RideUpdateWorker()
{

}

