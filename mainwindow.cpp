#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    pathCoordUpdater = new RideUpdateWorker(this);


    QTime time =  QTime::currentTime();

    TrackInfo trackInfo;
    if(trackInfo.setAndOpenDatabase("C:/Users/kruglyakovav/Documents/build-GraphicalDatabase-Desktop_Qt_5_14_2_MinGW_32_bit-Debug/ApBAZE_2021-03-10_0840.db") == false)
        return;

    trackInfo.setAssetNum("110000123030");
    // trackInfo.setDirInfo("15101", "2");


    auto map = trackInfo.getItemsMap();

    qDebug() << map.value(TrackItem::KM).size() << " "
             << map.value(TrackItem::STR).size() << " "
             << map.value(TrackItem::STAN).size() << " "
             << map.value(TrackItem::PCH).size() << " "
             << map.value(TrackItem::MOST).size() << " "
             << map.value(TrackItem::MOV).size();




    qDebug() << "Full time: " << time.msecsTo(QTime::currentTime()) << " ms";

}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setRegistryInfo(QString pathToReg)
{
    qDebug() << "Set Registry Info";
    if(pathCoordUpdater == nullptr)
    {
        qDebug() << "Error: path coordinate updater is nullptr";
        return;
    }
    static_cast<RideUpdateWorker*>(pathCoordUpdater)->setRegistryPathAndRideInfo(pathToReg);
    qDebug() << "Registry info: " <<
                pathCoordUpdater->getRideInfo().toString();
}

void MainWindow::startPathCoordUpdater()
{
    qDebug() << "Start PathCoordUpdater";
    if(pathCoordUpdater == nullptr)
    {
        qDebug() << "Error: path coordinate updater is nullptr";
        return;
    }
    pathCoordUpdater->startUpdating();

    connect(pathCoordUpdater, SIGNAL(currentPathCoordChanged(QString)), this, SLOT(currentPathCoordUpdate(QString)));
}

void MainWindow::currentPathCoordUpdate(QString pathCoord)
{
    bool isCorrectPathCoord;
    km = pathCoord.split(";").first().toInt(&isCorrectPathCoord);
    if(isCorrectPathCoord == false)
    {
        qDebug() << "Invalid path coord: km";
        return;
    }

    m = pathCoord.split(";").at(1).toInt(&isCorrectPathCoord) / 1000;
    if(isCorrectPathCoord == false)
    {
        qDebug() << "Invalid path coord: m";
        return;
    }

    ui->statusbar->showMessage("Current path coord: " + pathCoord);
}
