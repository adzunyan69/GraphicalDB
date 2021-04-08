#ifndef TRACKINFO_H
#define TRACKINFO_H

#include <QObject>
#include "DatabaseAccessModule/databaseaccess.h"


struct TrackItem
{
    enum TrackItemType{ KM, STR, STAN, PCH, MOST, MOV, CUR, SPD} type;

    QString name;
    QString numb;

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

    bool operator<(const TrackItem &rhs)
    {
        return (this->beginKM < rhs.beginKM || (this->beginKM == rhs.beginKM) && this->beginM < rhs.beginM);
    }

};

class TrackInfo : public QObject
{
    Q_OBJECT

    DatabaseAccess dba;

    QString sqlPath;

    QString assetNum;

    QString dirCode;
    QString trackNum;
    QString dirName;
public:
    explicit TrackInfo(QObject *parent = nullptr);

    bool setAndOpenDatabase(QString databaseName, QString _sqlPath);
    void setAssetNum(QString _assetNum);
    void setDirInfo(QString _dirCode, QString _trackNum);
    QString getDirCode() { return dirCode; }
    QString getTrackNum() { return trackNum; }
    QString getDirName() { return dirName; }
    QVector<TrackItem> getVec(QString sqlName);

    QMap<TrackItem::TrackItemType, QVector<TrackItem>> getItemsMap();

    void calculateAbsCoordForKm(QVector<TrackItem> &km);
    void calculateAbsCoord(QVector<TrackItem> &trackItems, QVector<TrackItem> &km);

    int getAbsCoord(QVector<TrackItem> &km, int pathKm, int pathM);

public slots:
    void errorFromDBA(QString msg);
signals:
    void error(QString msg);
};


#endif // TRACKINFO_H
