#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "RideModule/ride.h"
#include "DatabaseAccessModule/databaseaccess.h"
#include "DatabaseAccessModule/trackinfo.h"
#include "PlotModule/plot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setRegistryInfo(QString pathToReg);
    void startPathCoordUpdater();


private:
    Ui::MainWindow *ui;

    RideUpdater *pathCoordUpdater = nullptr;
    Plot plot;

    int km;
    int m;
signals:
    void positionChanged(int newAbsPos);
private slots:
    void currentPathCoordUpdate(QString);
public slots:
    void positionChange();

};
#endif // MAINWINDOW_H
