#include "mainwindow.h"
#include "ui_mainwindow.h"

//#define ATAPE_MODE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(this, SIGNAL(error(QString)), this, SLOT(showErrorMessage(QString)));

    setupWindow();

    qDebug() << "Start pathCoordUpdater";
    pathCoordUpdater = new RideUpdateWorker(this);

    connect(&plot, SIGNAL(error(QString)), this, SLOT(showErrorMessage(QString)));
     connect(pathCoordUpdater, SIGNAL(error(QString)), this, SLOT(showErrorMessage(QString)));

    qDebug() << "Setup plot";
    plot.setupPlot(ui->graphicalDatabase);

    connectObjects();


}

void MainWindow::showErrorMessage(QString msg)
{
    if(QMessageBox::warning(this, "Ошибка", msg, QMessageBox::Ok | QMessageBox::Close) == QMessageBox::Close)
        exit(1);
}

void MainWindow::slotCustomMenuRequested(QPoint pos)
{
    qDebug() << "Context menu requested.";
    QMenu* menu = new QMenu(this);
    QAction* onTopToggle = new QAction("Поверх всех окон", menu);
    onTopToggle->setCheckable(true);
    onTopToggle->setChecked(settings.read(Settings::WINDOW_ON_TOP, "WindowSettings").toBool());
    connect(onTopToggle, SIGNAL(toggled(bool)), this, SLOT(slotOnTopToggle(bool)));

    QAction* bdConvectorStart = new QAction("Запуск обновления БД", menu);
    connect(bdConvectorStart, SIGNAL(triggered()), this, SLOT(slotBdConvectorStart()));

    menu->addAction(onTopToggle);
    menu->addAction(bdConvectorStart);

    menu->popup(this->mapToGlobal(pos));

}

void MainWindow::slotOnTopToggle(bool isOnTop)
{
    qDebug() << "slotOnTopToggle";
    settings.write(Settings::WINDOW_ON_TOP, isOnTop, "WindowSettings");

    if(isOnTop == true)
        this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    else
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowStaysOnTopHint);

    show();
}

void MainWindow::slotBdConvectorStart()
{
    QProcess *bdConvector = new QProcess(this);
    QString programmPath = QApplication::applicationDirPath() + "/BDConv/BDRadioavionika.exe";
    if(QFile::exists(programmPath) == false)
    {
        emit error("Ошибка при запуске конвертора: исполняемый файл отсуствует (" + programmPath + ").");
        return;
    }
    bdConvector->setProgram(programmPath);
    bdConvector->start();
    if(bdConvector->waitForFinished(-1) == false)
        emit error("Ошибка при конвертации: программа не завершила работу.");
    else
    {
        QMessageBox::information(this, "Завершение конвертации", "База обновлена. Перезапустите, пожалуйста, приложение.");
        qApp->quit();
    }
}

void MainWindow::focusChanged(QWidget* old, QWidget* now)
{
    qDebug() << "Focus changed from " << old << " to " << now;
    if(now == nullptr)
        emit windowFocusChanged(false);
    else
        emit windowFocusChanged(true);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    settings.write(Settings::WINDOW_WIDTH, this->width(), "WindowSettings");
    settings.write(Settings::WINDOW_HEIGHT, this->height(), "WindowSettings");
    settings.write(Settings::WINDOW_Y_POSITION, this->y(), "WindowSettings");
    settings.write(Settings::WINDOW_X_POSITION, this->x(), "WindowSettings");

    event->accept();
}

void MainWindow::setupWindow()
{
    if(settings.isFirstRun())
        createDefaultSettings();
    setWindowGeometry();
    setWindowsWidgets();

}

void MainWindow::createDefaultSettings()
{
    QRect screenGeometry = QGuiApplication::screens().first()->availableGeometry();
    settings.write(Settings::WINDOW_ON_TOP, "false", "WindowSettings");
    settings.write(Settings::WINDOW_WIDTH, screenGeometry.width(), "WindowSettings");
    settings.write(Settings::WINDOW_HEIGHT, 253, "WindowSettings");
    settings.write(Settings::WINDOW_Y_POSITION, screenGeometry.height() - this->height() - ui->statusbar->height(), "WindowSettings");
    settings.write(Settings::WINDOW_X_POSITION, 0, "WindowSettings");

    settings.write(Settings::DATABASE_PATH, QApplication::applicationDirPath() + "/ApBAZE.db", "Settings");


    QMessageBox::information(this, "Создание файла конфигурации",
                             "Файл конфигурации не был найден, был создан файл конфигурации по умолчанию.");
}

void MainWindow::setWindowGeometry()
{
    qDebug() << "MainWindow()";

    if(settings.read(Settings::WINDOW_ON_TOP, "WindowSettings").toString() == "true")
        this->setWindowFlag(Qt::WindowStaysOnTopHint);

    QVariant width = settings.read(Settings::WINDOW_WIDTH, "WindowSettings");
    QVariant height = settings.read(Settings::WINDOW_HEIGHT, "WindowSettings");
    QVariant yPosition = settings.read(Settings::WINDOW_Y_POSITION, "WindowSettings");
    QVariant xPosition = settings.read(Settings::WINDOW_X_POSITION, "WindowSettings");
    if(width.isValid() && height.isValid() && yPosition.isValid() && xPosition.isValid())
    {
        bool wOk = false, hOk = false, yOk = false, xOk = false;
        int w = width.toInt(&wOk), h = height.toInt(&hOk), y = yPosition.toInt(&yOk), x = xPosition.toInt(&xOk);
        if(wOk && hOk && yOk && xOk)
        {
            this->resize(w, h);
            this->setFixedHeight(h);
            this->move(x, y);
        }
        else
        {
            qDebug() << "Error: converting from QVariant to Int (geometry).";
            emit error("Ошибка при получении параметров окна из файла настроек.");
        }
    }
    else
    {
        qDebug() << "Error: reading geometry settings.";
        emit error("Ошибка чтения параметров окна из настроек.");
    }


    qDebug() << "Window geometry " << this->geometry();
}

void MainWindow::setWindowsWidgets()
{
    spdLabel = new QLabel(this);
    pchLabel = new QLabel(this);
    coordLabel = new QLabel(this);
    distanceLabel = new QLabel(this);
    spdLabel->setText("[Скорости]");
    spdLabel->setMinimumWidth(150);
    pchLabel->setText("[ПЧ]");
    pchLabel->setMinimumWidth(50);
    coordLabel->setText("[Координата]");
    coordLabel->setMinimumWidth(150);
    distanceLabel->setText("[Перегон]");
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


    ui->graphicalDatabase->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->graphicalDatabase, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuRequested(QPoint)));
    connect(QApplication::instance(), SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(focusChanged(QWidget*, QWidget*)));
    connect(this, SIGNAL(windowFocusChanged(bool)), &plot, SLOT(windowFocusChanged(bool)));
}

void MainWindow::drawPlot()
{

    getItemsMap();

    qDebug() << "Drawing plot";
    QTime time = QTime::currentTime();
    plot.drawObjects(itemsMap);



    qDebug() << "Full time for drawing graphical database: " << time.msecsTo(QTime::currentTime()) << " ms";
}

void MainWindow::getItemsMap()
{
    qDebug() << "Getting items from db";
    QTime time =  QTime::currentTime();

    TrackInfo trackInfo;

    connect(&trackInfo, SIGNAL(error(QString)), this, SLOT(showErrorMessage(QString)));

    QString database = settings.read(Settings::DATABASE_PATH, "Settings").toString();
    qDebug() << "DB / SQL: " << database << " / " << (QApplication::applicationDirPath() + "/sql");
    if(trackInfo.setAndOpenDatabase(database, QApplication::applicationDirPath() + "/sql") == false)
    {
        qDebug() << "Error while opening database";
        error("Ошибка при открытии базы: " + database);
        if(QMessageBox::question(this, "Обновить БД?", "Запустить конвертор для обновления БД?") == QMessageBox::Yes)
            slotBdConvectorStart();
        return;
    }

#ifdef ATAPE_MODE
    qDebug() << "RegInfo: " << pathCoordUpdater->getRideInfo().toString();
//    trackInfo.setDirInfo("14601", "2");
//    plot.setReversed(true);
    trackInfo.setAssetNum(pathCoordUpdater->getRideInfo().trackCode);
    plot.setReversed(!pathCoordUpdater->getRideInfo().increase);
#else
     // trackInfo.setAssetNum("110000123030");110000122929
    // trackInfo.setAssetNum("110000122929");VALUE="A5284454"/>
    // trackInfo.setAssetNum("A5284454");
    // trackInfo.setDirInfo("14601", "1");
    //trackInfo.setDirInfo("10901", "1");
    //trackInfo.setDirInfo("15301", "1");
    // trackInfo.setDirInfo("73001", "1");
    trackInfo.setAssetNum("14601");
    plot.setReversed(false);
#endif
    itemsMap = trackInfo.getItemsMap();
    if(itemsMap.isEmpty())
        emit error("Не было получено объектов из БД.");
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
        emit error("Ошибка инициализации объекта для проверки координаты.");
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
        emit error("Ошибка инициализации объекта для проверки координаты.");
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
        emit error("Получена неверная путейская координата в реестре: " + pathCoord);
        return;
    }

    m = pathCoord.split(";").at(1).toInt(&isCorrectPathCoord) / 1000;
    if(isCorrectPathCoord == false)
    {
        qDebug() << "Invalid path coord: m";
        emit error("Получена неверная путейская координата в реестре: " + pathCoord);
        return;
    }

    coordLabel->setText(QString::number(km) + " км " + QString::number(m) + " м");
}
