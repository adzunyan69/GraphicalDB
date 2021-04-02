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
    void drawPlot();
    void startPathCoordUpdater();


private:
    Ui::MainWindow *ui;

    QLabel *spdLabel;
    QLabel *pchLabel;
    QLabel *coordLabel;

    RideUpdater *pathCoordUpdater = nullptr;
    Plot plot;

    int km;
    int m;

    QMap<TrackItem::TrackItemType, QVector<TrackItem>> itemsMap;

    void setWindowGeometry();
    void setWindowsWidgets();
    void connectObjects();
    void getItemsMap();
signals:
    void positionChanged(int newAbsPos);
private slots:
    void currentPathCoordUpdate(QString);
public slots:
    void positionChange();
    void spdChange(QString);
    void pchChange(QString);
    void coordChange(QString);
    void coordChange(int);

};
#endif // MAINWINDOW_H
