#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    pathCoordUpdater = new RideUpdateWorker(this);

//    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
//    db.setDatabaseName("C:/Users/kruglyakovav/Documents/build-GraphicalDatabase-Desktop_Qt_5_14_2_MinGW_32_bit-Debug/ApBAZE_2021-03-10_0840.db");
//    db.open();

//    QFile file("C:/Users/kruglyakovav/Documents/build-GraphicalDatabase-Desktop_Qt_5_14_2_MinGW_32_bit-Debug/sql/NKM.sql");
//    file.open(QFile::ReadOnly);
//    // QString query = file.readAll();
//    QSqlQuery q;
//    q.prepare(file.readAll());
//    // q.bindValue(":ASSET_NUM", "String");
//    q.exec();
//    q.next();
//    qDebug() << "Last query: " << q.executedQuery();
//    qDebug() << "Value: " << q.value("KOD").toString();


    DatabaseAccess dba;
    if(dba.openDatabase("C:/Users/kruglyakovav/Documents/build-GraphicalDatabase-Desktop_Qt_5_14_2_MinGW_32_bit-Debug/ApBAZE_2021-03-10_0840.db") == false)
    {
        qDebug() << "Error: database error";
        qDebug() << dba.lastError();
        return;
    }

    QVector<QString> vecResult;
    QMap<QString, QVariant> bindValue;
    bindValue.insert(":ASSET_NUM", QVariant(110000123030));
    // bindValue.insert(":UP_NUM", QVariant(15101));
    // bindValue.insert(":TRACK_NUM", QVariant("2"));
    QTime time =  QTime::currentTime();
    QSqlQuery query = dba.execQueryFileBind("C:/Users/kruglyakovav/Documents/build-GraphicalDatabase-Desktop_Qt_5_14_2_MinGW_32_bit-Debug/sql/STR.sql", bindValue);
    query.next();
    while(query.next())
    {
        vecResult.push_back(query.value("KOD").toString() +
                            query.value("PUT").toString() +
                            query.value("K_EKASUI").toString() +
                            query.value("NOMER").toString() +
                            query.value("NPR").toString() +
                            query.value("KM").toString() +
                            query.value("M").toString());
        // qDebug() << "Query result: "
        //          << "Code: " << query.value("ID").toString();
    }

    qDebug() << "Query 1 size: " << vecResult.size();

    QVector<QString> vecResult2;
    QSqlQuery query2 = dba.execQueryFileBind("C:/Users/kruglyakovav/Documents/build-GraphicalDatabase-Desktop_Qt_5_14_2_MinGW_32_bit-Debug/sql/NKM.sql", bindValue);
    query2.next();
    while(query2.next())
    {
        vecResult2.push_back(query2.value("KOD").toString() +
                            query2.value("PUT").toString() +
                            query2.value("K_EKASUI").toString() +
                            query2.value("NOMER").toString() +
                            query2.value("NPR").toString() +
                            query2.value("KM").toString() +
                            query2.value("M").toString());
        // qDebug() << "Query result: "
        //          << "Code: " << query.value("ID").toString();
    }

    qDebug() << "Query 2 size: " << vecResult2.size();

    qDebug() << "Elapsed time: " << time.msecsTo(QTime::currentTime()) << " ms";

//    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
//        db.setDatabaseName("C:/Users/kruglyakovav/Documents/build-GraphicalDatabase-Desktop_Qt_5_14_2_MinGW_32_bit-Debug/ApBAZE_2021-03-10_0840.db");
//        db.open();

//        QFile file("C:/Users/kruglyakovav/Documents/build-GraphicalDatabase-Desktop_Qt_5_14_2_MinGW_32_bit-Debug/sql/test.sql");
//        file.open(QFile::ReadOnly);
//        QSqlQuery query;
//        query.prepare(file.readAll());
//        query.bindValue(":NUM", QVariant(10901));
//        query.exec();
//        query.next();

//        qDebug() << "test: " << query.lastError() << "\n"
//                 << query.value("ID").toString();


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
