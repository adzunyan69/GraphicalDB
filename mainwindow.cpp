#include "mainwindow.h"
#include "ui_mainwindow.h"

#define ATAPE_MODE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowGeometry();
    setWindowsWidgets();

    qDebug() << "Start pathCoordUpdater";
    pathCoordUpdater = new RideUpdateWorker(this);

    qDebug() << "Setup plot";
    plot.setupPlot(ui->graphicalDatabase);

    connectObjects();
}

void MainWindow::setWindowGeometry()
{
    qDebug() << "MainWindow()";
    QRect screenGeometry = QGuiApplication::screens().first()->availableGeometry();
    int yWindowPosition = screenGeometry.height() - this->height() - ui->statusbar->height();
    qDebug() << yWindowPosition;
    this->move(0, yWindowPosition);
    this->setFixedWidth(screenGeometry.width());
    this->setWindowFlag(Qt::WindowStaysOnTopHint);
    qDebug() << "Window geometry " << this->geometry();
}

void MainWindow::setWindowsWidgets()
{
    spdLabel = new QLabel(this);
    pchLabel = new QLabel(this);
    coordLabel = new QLabel(this);
    distanceLabel = new QLabel(this);
    spdLabel->setText("spdLabel");
    spdLabel->setMinimumWidth(150);
    pchLabel->setText("pchLabel");
    pchLabel->setMinimumWidth(50);
    coordLabel->setText("coordLabel");
    coordLabel->setMinimumWidth(150);
    distanceLabel->setText("distanceLabel");
    distanceLabel->setMinimumWidth(150);
    ui->statusbar->addWidget(spdLabel);
    ui->statusbar->addWidget(pchLabel);
    ui->statusbar->addWidget(coordLabel);
    ui->statusbar->addWidget(distanceLabel);
}

void MainWindow::connectObjects()
{
#ifdef ATAPE_MODE
    connect(pathCoordUpdater, SIGNAL(currentPathCoordChanged(QString)), &plot, SLOT(changePosition(QString)));
#else
    QTimer *timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(positionChange()));
    connect(this, SIGNAL(positionChanged(int)), &plot, SLOT(changePosition(int)));
    connect(this, SIGNAL(positionChanged(int)), this, SLOT(coordChange(int)));
    timer->start();
#endif
    connect(&plot, SIGNAL(spdChanged(QString)), this, SLOT(spdChange(QString)));
    connect(&plot, SIGNAL(pchChanged(QString)), this, SLOT(pchChange(QString)));
    connect(&plot, SIGNAL(distanceChanged(QString)), this, SLOT(distanceChange(QString)));
}

void MainWindow::drawPlot()
{
    getItemsMap();

    qDebug() << "Drawing plot";
    QTime time = QTime::currentTime();
    plot.drawObjects(itemsMap);



    qDebug() << "Full time for drawing grapgical database: " << time.msecsTo(QTime::currentTime()) << " ms";
}

void MainWindow::getItemsMap()
{
    qDebug() << "Getting items from db";
    QTime time =  QTime::currentTime();

    TrackInfo trackInfo;

    QSettings config(QApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
    QString database = config.value("DATABASE_PATH").toString();
    QString sql = config.value("SQL_PATH").toString();
    qDebug() << "DB / SQL: " << database << " / " << sql;
    if(trackInfo.setAndOpenDatabase(database, sql) == false)
    {
        qDebug() << "Error while opening database";
        return;
    }

#ifdef ATAPE_MODE
    qDebug() << "RegInfo: " << pathCoordUpdater->getRideInfo().toString();
    trackInfo.setAssetNum(pathCoordUpdater->getRideInfo().trackCode);
    plot.setReversed(!pathCoordUpdater->getRideInfo().increase);
#else
    // trackInfo.setAssetNum("110000123030");
    trackInfo.setDirInfo("14601", "2");
    // plot.setReversed(true);
#endif
    itemsMap = trackInfo.getItemsMap();
    this->setWindowTitle(trackInfo.getDirCode() + " - " + trackInfo.getTrackNum() + ", " + trackInfo.getDirName());

    qDebug() << "Full time for getting objects: " << time.msecsTo(QTime::currentTime()) << " ms";
}

void MainWindow::positionChange()
{
    static int pos = -100;
    pos += 50;
    emit positionChanged(pos);
}

void MainWindow::spdChange(QString spd)
{
    qDebug() << "spdChange: " << spd;
    spdLabel->setText("Скорости;Скорости ВСК: " + spd);
}

void MainWindow::pchChange(QString pch)
{
    qDebug() << "pchChange: " << pch;
    pchLabel->setText("Дистанция пути: " + pch);
}


void MainWindow::coordChange(int coord)
{
    coordLabel->setText("Абсолютная координата: " + QString::number(coord));
}

void MainWindow::distanceChange(QString distance)
{
    qDebug() << "distance change: " << distance;
    distanceLabel->setText("Перегон: " + distance);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setRegistryInfo(QString pathToReg)
{
#ifdef ATAPE_MODE
    qDebug() << "Set Registry Info";
    if(pathCoordUpdater == nullptr)
    {
        qDebug() << "Error: path coordinate updater is nullptr";
        return;
    }
    static_cast<RideUpdateWorker*>(pathCoordUpdater)->setRegistryPathAndRideInfo(pathToReg);
    qDebug() << "Registry info: " <<
                pathCoordUpdater->getRideInfo().toString();
    qDebug() << "Getting items from db";
#endif

}

void MainWindow::startPathCoordUpdater()
{
#ifdef ATAPE_MODE
    qDebug() << "Start PathCoordUpdater";
    if(pathCoordUpdater == nullptr)
    {
        qDebug() << "Error: path coordinate updater is nullptr";
        return;
    }
    pathCoordUpdater->startUpdating();

    connect(pathCoordUpdater, SIGNAL(currentPathCoordChanged(QString)), this, SLOT(currentPathCoordUpdate(QString)));
#endif
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

    // qDebug() << "currentPathCoordUpdate";
    coordLabel->setText(QString::number(km) + " км " + QString::number(m) + " м");
    // ui->statusbar->showMessage("Current path coord: " + pathCoord);
}
