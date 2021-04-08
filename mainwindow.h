#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "RideModule/ride.h"
#include "DatabaseAccessModule/databaseaccess.h"
#include "DatabaseAccessModule/trackinfo.h"
#include "PlotModule/plot.h"
#include "settings.h"

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
    QLabel *distanceLabel;

    RideUpdater *pathCoordUpdater = nullptr;
    Plot plot;
    Settings settings;

    int km;
    int m;

    QMap<TrackItem::TrackItemType, QVector<TrackItem>> itemsMap;

    void setupWindow();
    void createDefaultSettings();
    void setWindowGeometry();
    void setWindowsWidgets();
    void connectObjects();
    void getItemsMap();

    void closeEvent(QCloseEvent *event);
signals:
    void positionChanged(int newAbsPos);
    void windowFocusChanged(bool isWindowOnFocus);

    void error(QString msg);
private slots:
    void currentPathCoordUpdate(QString);
public slots:
    void slotCustomMenuRequested(QPoint pos);
    void slotOnTopToggle(bool isOnTop);
    void focusChanged(QWidget* old, QWidget* now);


    void positionChange();
    void spdChange(QString);
    void pchChange(QString);
    void distanceChange(QString);
    void coordChange(int);

    void showErrorMessage(QString msg);

};
#endif // MAINWINDOW_H
