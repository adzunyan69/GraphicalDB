#include "ride.h"

RideUpdateWorker::RideUpdateWorker(QObject *parent) : RideUpdater(parent)
{

}

void RideUpdateWorker::setRegistryPathAndRideInfo(QString _registryPath)
{
    registryPath = _registryPath;
    registry = new QSettings(registryPath, QSettings::Registry64Format, this);

    qDebug() << "Registry path: " << registryPath;

    rideInfo.directionCode = "1234";
    rideInfo.trackCode = "123";
    rideInfo.trackNumber = "12345";
    rideInfo.increase = false;
}

RideInfo RideUpdateWorker::getRideInfo()
{
    if(registry != nullptr)
    {
        qDebug() << "Registry exist";
    }

    return rideInfo;

}

void RideUpdateWorker::startUpdating()
{
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

    qDebug() << currentPathCoord;
    emit currentPathCoordChanged(currentPathCoord);
}

RideUpdateWorker::~RideUpdateWorker()
{

}

