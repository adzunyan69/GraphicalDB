#ifndef PLOT_H
#define PLOT_H

#include <QObject>
#include "PlotModule/qcustomplot/qcustomplot.h"
#include "DatabaseAccessModule/trackinfo.h"

class Plot : public QObject
{
    Q_OBJECT

    QCustomPlot *plot = nullptr;

    QCPGraph *kmGraph;
    QCPGraph *strLeftGraph;
    QCPGraph *strRightGraph;
    QCPGraph *stanGraph;
    QCPGraph *posGraph;
    QCPGraph *movGraph;
    QCPGraph *mostGraph;

    QVector<TrackItem> kmVec;

    const double ySTR = 0.70;
    const double yMOV = 0.77;
    const double yMOST = 0.6;
    const double ySTAN = 0.5;
    const double yCUR = 0.3;

    const double yPos = 0.5;


    void drawKM(const QVector<TrackItem> &km);
    void drawItems(const QVector<TrackItem> &items, int yPos);
    void drawSTR(const QVector<TrackItem> &str);
    void drawMOV(const QVector<TrackItem> &mov);
    void drawMOST(const QVector<TrackItem> &most);
    void drawPCH(const QVector<TrackItem> &pch);
    void drawSTAN(const QVector<TrackItem> &stan);
    void drawCUR(const QVector<TrackItem> &cur);

    int getAbsCoord(int pathKm, int pathM);
public:
    explicit Plot(QObject *parent = nullptr);

    void setupPlot(QCustomPlot *_plot);
    void drawObjects(QMap<TrackItem::TrackItemType, QVector<TrackItem>> &itemsMap);

signals:


public slots:
    void changePosition(int absPos);
    void changePosition(QString pos);

    void xAxisChanged(const QCPRange  newRange);
};

#endif // PLOT_H
