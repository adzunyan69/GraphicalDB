#ifndef TRACKINFO_H
#define TRACKINFO_H

#include <QObject>
#include "DatabaseAccessModule/databaseaccess.h"


struct TrackItem
{
    enum TrackItemType{ KM, STR, STAN, PCH, MOST, MOV, CUR, SPD} type;

    QString name;
    int numb;

    int km = -1;
    int beginKM = -1;
    int beginM = -1;
    int oKM = -1;
    int oM = -1;
    int endKM = -1;
    int endM = -1;
    int len = -1;


    int absBegin;
    int absO;
    int absEnd;

};

class TrackInfo : public QObject
{
    Q_OBJECT

    DatabaseAccess dba;

    QString assetNum;

    QString dirCode;
    QString trackNum;
public:
    explicit TrackInfo(QObject *parent = nullptr);

    bool setAndOpenDatabase(QString databaseName);
    void setAssetNum(QString _assetNum);
    void setDirInfo(QString _dirCode, QString _trackNum) { dirCode = _dirCode, trackNum = _trackNum; }
    QVector<TrackItem> getVec(QString sqlName);

    QMap<TrackItem::TrackItemType, QVector<TrackItem>> getItemsMap();

    void calculateAbsCoordForKm(QVector<TrackItem> &km);
    void calculateAbsCoord(QVector<TrackItem> &trackItems, QVector<TrackItem> &km);

    int getAbsCoord(QVector<TrackItem> &km, int pathKm, int pathM);

signals:

};


#endif // TRACKINFO_H
