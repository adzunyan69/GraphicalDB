#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    pathCoordUpdater = new RideUpdateWorker(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setRegistryInfo(QString pathToReg)
{
    qDebug() << "Set Registry Info";
    static_cast<RideUpdateWorker*>(pathCoordUpdater)->setRegistryPathAndRideInfo(pathToReg);
    qDebug() << "Registry info: " <<
                pathCoordUpdater->getRideInfo().toString();
}

void MainWindow::startPathCoordUpdater()
{
    qDebug() << "Start PathCoordUpdater";
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
