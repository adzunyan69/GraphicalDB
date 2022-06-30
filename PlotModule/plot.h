#ifndef PLOT_H
#define PLOT_H

#include <QObject>
#include <QtConcurrent/QtConcurrent>
#include "PlotModule/qcustomplot/qcustomplot.h"
#include "DatabaseAccessModule/trackinfo.h"

class Plot : public QObject
{
    Q_OBJECT

    QCustomPlot *plot = nullptr;

    QCPGraph *kmGraph;
    QCPGraph *curGraph;
    QCPGraph *strLeftGraph;
    QCPGraph *strRightGraph;
    QCPGraph *stanGraph;
    QCPGraph *posGraph;
    QCPGraph *movGraph;
    QCPGraph *mostGraph;
    QCPGraph *overpassGraph;
    QCPGraph *tunnelGraph;
    QCPGraph *spdGraph;
    QCPGraph *isoGraph;

    int posAbsCoord = 0;
    QCPItemStraightLine  *posLine;
    QCPItemPixmap *traintPixmap;

    QVector<TrackItem> kmVec;
    QVector<TrackItem> pchVec;
    QVector<TrackItem> spdVec;
    QVector<TrackItem> stanVec;
    QVector<TrackItem> sleeperVec;
    QVector<TrackItem> bondingVec;
    QVector<TrackItem> railVec;

    QString currentSPD;
    QString currentPCH;
    QString currentDistance;
    QString currentSleeper;
    QString currentBonding;
    QString currentRail;

    // QMap<TrackItem::TrackItemType, QVector<TrackItem>> &itemsMap = QMap<TrackItem::TrackItemType, QVector<TrackItem>>();

    const double yKM = 0.1;
    const double yKMdiff = 0.1;

    const double ySTR = 1.0;
    const double ySTRdiff = 0.2;
    const double yMOV = 1.05;
    const double yMOVdiff = 0.2;
    const double yMOST = 0.9;
    const double yMOSTdiff = 0.15;
    const double yOVERPASS = 0.9;
    const double yOVERPASSdiff = 0.15;
    const double yTUNNEL = 0.9;
    const double yTUNNELdiff = 0.15;
    const double ySTAN = 0.8;
    const double ySTANosDiff = 0.25;
    const double ySTANlabelDiff = 0.64;
    const double yCUR = 0.30;
    const double yCURdiff = 0.1;
    const double yPCH = 0.6;
    const double yPCHdiff = 0.1;
    const double yPos = ySTAN + 0.05;
    const double ySleeper = 1.45; // Шпалы
    const double ySleeperDiff = 0.03;
    const double yISO = 0.85;

    const double ySPD = 0.7;
    const double ySPDdiff = 0.1;

    int prevKm = 0;
    int prevM = 0;

    bool reversed = false;

    bool focus = false;


    void drawKM(const QVector<TrackItem> &km);
    void drawItems(const QVector<TrackItem> &items, int yPos);
    void drawSTR(const QVector<TrackItem> &str);
    void drawMOV(const QVector<TrackItem> &mov);
    void drawMOST(const QVector<TrackItem> &most);
    void drawOVERPASS(const QVector<TrackItem> &overpass);
    void drawTUNNEL(const QVector<TrackItem> &tunnel);
    void drawPCH(const QVector<TrackItem> &pch);
    void drawSTAN(const QVector<TrackItem> &stan);
    void drawCUR(const QVector<TrackItem> &cur);
    void drawSPD(const QVector<TrackItem> &spd);
    void drawSLEEPER(const QVector<TrackItem> &sleeper);
    void drawISO(const QVector<TrackItem> &iso);


    void checkSPD(int absPos);
    void checkPCH(int absPos);
    void checkDistance(int absPos);
    void checkSleeper(int absPos);
    void checkBonding(int absPos);
    void checkRail(int absPos);
    int getAbsCoord(int pathKm, int pathM);
public:
    explicit Plot(QObject *parent = nullptr);

    void setupPlot(QCustomPlot *_plot);
    void setReversed(bool _reversed);
    void drawObjects(QMap<TrackItem::TrackItemType, QVector<TrackItem>> &itemsMap);

signals:
    void spdChanged(QString newSPD);
    void pchChanged(QString newPCH);
    void distanceChanged(QString newDistance);
    void sleeperChanged(QString newSleeper);
    void bondingChanged(QString newBonding);
    void railChanged(QString newRail);

    void error(QString msg);

public slots:
    void changePosition(int absPos);
    void changePosition(QString pos);
    void windowFocusChanged(bool isWindowOnFocus);
    void rangeChangedLimit(QCPRange newRange);

};

#endif // PLOT_H
