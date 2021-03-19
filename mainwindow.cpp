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

    // trackInfo.setAssetNum("110000123030");
    trackInfo.setDirInfo("14601", "2");

    auto itemsMap = trackInfo.getItemsMap();

    qDebug() << "Full time for getting objects: " << time.msecsTo(QTime::currentTime()) << " ms";


    time = QTime::currentTime();

    plot.setupPlot(ui->graphicalDatabase);
    plot.drawObjects(itemsMap);

    qDebug() << "Full time for drawing grapgical database: " << time.msecsTo(QTime::currentTime()) << " ms";

    connect(pathCoordUpdater, SIGNAL(currentPathCoordChanged(QString)), &plot, SLOT(changePosition(QString)));
    QTimer *timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(positionChange()));
    connect(this, SIGNAL(positionChanged(int)), &plot, SLOT(changePosition(int)));
    timer->start();
}

void MainWindow::positionChange()
{
    static int pos = 2000;
    pos += 25;
    emit positionChanged(pos);
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
