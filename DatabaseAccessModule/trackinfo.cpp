#include "trackinfo.h"

TrackInfo::TrackInfo(QObject *parent) : QObject(parent)
{
}

void TrackInfo::setAssetNum(QString _assetNum)
{
    if(_assetNum.isEmpty() == true)
    {
        qDebug() << "Error: assetNum is empty";
        return;
    }

    QMap<QString, QVariant> bindValue;
    bindValue.insert(":ASSET_NUM", _assetNum);
    QSqlQuery query = dba.execQueryFileBind("C:/Users/kruglyakovav/Documents/build-GraphicalDatabase-Desktop_Qt_5_14_2_MinGW_32_bit-Debug/sql/ASSETNUM.sql",
                                            bindValue);

    query.first();
    dirCode = query.value("KOD").toString();
    trackNum = query.value("PUT").toString();

    qDebug() << dirCode << " " << trackNum;
}

bool TrackInfo::setAndOpenDatabase(QString databaseName)
{
    if(dba.openDatabase(databaseName) == false)
    {
        qDebug() << "Error: database error";
        qDebug() << dba.lastError();
        return false;
    }

    return true;
}

QVector<TrackItem> TrackInfo::getVec(QString sqlName)
{
    QTime time = QTime::currentTime();
    QVector<TrackItem> result;
    QMap<QString, QVariant> bindValue;
    if(dirCode.isEmpty() && trackNum.isEmpty())
    {
        qDebug() << "dir Code && track Num is empty";
        if(assetNum.isEmpty() == true)
        {
            qDebug() << "Asset num is empty";
            return result;
        }
        bindValue.insert(":ASSET_NUM", assetNum);
    }
    else
        bindValue.insert(":DIR_CODE", dirCode);
        bindValue.insert(":TRACK_NUM", trackNum);

    QSqlQuery query = dba.execQueryFileBind("C:/Users/kruglyakovav/Documents/build-GraphicalDatabase-Desktop_Qt_5_14_2_MinGW_32_bit-Debug/sql/" +
                                            sqlName, bindValue);

    query.first();
    do
    {
        TrackItem item;
        QString objType = query.value("OBJ").toString();
        if(objType == "NKM")
        {
            item.type = TrackItem::KM;
            item.km = query.value("NUMB").toInt();
            item.beginM = query.value("B_M").toInt();
            item.endM = query.value("E_M").toInt();
            item.len = query.value("LEN").toInt();
        }
        else if(objType == "STR")
        {
            item.type = TrackItem::STR;
            item.beginKM = query.value("B_KM").toInt();
            item.beginM = query.value("B_M").toInt();
            item.name = query.value("NAME").toString();
            item.numb = query.value("NUMB").toInt();
        }
        else if(objType == "STN")
        {
            item.type = TrackItem::STAN;
            item.name = query.value("NAME").toString();
            item.beginKM = query.value("B_KM").toInt();
            item.beginM = query.value("B_M").toInt();
            item.oKM = query.value("O_KM").toInt();
            item.oM = query.value("O_M").toInt();
            item.endKM = query.value("E_KM").toInt();
            item.endM = query.value("E_M").toInt();
        }
        else if(objType == "PCH")
        {
            item.type = TrackItem::PCH;
            item.beginKM = query.value("B_KM").toInt();
            item.beginM = query.value("B_M").toInt();
            item.endKM = query.value("E_KM").toInt();
            item.endM = query.value("E_M").toInt();
        }
        else if(objType == "MST")
        {
            item.type = TrackItem::MOST;
            item.beginKM = query.value("B_KM").toInt();
            item.beginM = query.value("B_M").toInt();
            item.endKM = query.value("E_KM").toInt();
            item.endM = query.value("E_M").toInt();

        }
        else if(objType == "MOV")
        {
            item.type = TrackItem::MOV;
            item.beginKM = query.value("B_KM").toInt();
            item.beginM = query.value("B_M").toInt();
            item.endKM = query.value("E_KM").toInt();
            item.endM = query.value("E_M").toInt();

        }
        else if(objType == "CUR")
        {
            item.type = TrackItem::CUR;
            item.beginKM = query.value("B_KM").toInt();
            item.beginM = query.value("B_M").toInt();
            item.oKM = query.value("O_KM").toInt();
            item.oM = query.value("O_M").toInt();
        }
        else if(objType == "SPD")
        {
            item.type = TrackItem::SPD;

        }

        result.push_back(item);
    } while(query.next());


    qDebug() << "Query time: " << time.msecsTo(QTime::currentTime()) << " ms";
    qDebug() << "Query  " << sqlName << " size: " << result.size();

    return result;


}

QMap<TrackItem::TrackItemType, QVector<TrackItem>> TrackInfo::getItemsMap()
{
    QMap<TrackItem::TrackItemType, QVector<TrackItem>> itemsMap;


    QVector<TrackItem> km = getVec("/NKM.sql");
    calculateAbsCoordForKm(km);

    QVector<TrackItem> str = getVec( "/STR.sql");
    calculateAbsCoord(str, km);
    QVector<TrackItem> stan =  getVec( "/STAN.sql");
    calculateAbsCoord(stan, km);
    QVector<TrackItem> pch =  getVec( "/PCH.sql");
    calculateAbsCoord(pch, km);
    QVector<TrackItem> most =  getVec( "/MOST.sql");
    calculateAbsCoord(most, km);
    QVector<TrackItem> mov =  getVec( "/MOV.sql");
    calculateAbsCoord(mov, km);
    QVector<TrackItem> cur =  getVec( "/CUR.sql");
    calculateAbsCoord(cur, km);
    QVector<TrackItem> spd =  getVec( "/SPD.sql");
    calculateAbsCoord(spd, km);

    QTime time = QTime::currentTime();
    itemsMap.insert(TrackItem::KM, km);
    itemsMap.insert(TrackItem::STR, str);
    itemsMap.insert(TrackItem::STAN, stan);
    itemsMap.insert(TrackItem::PCH, pch);
    itemsMap.insert(TrackItem::MOST, most);
    itemsMap.insert(TrackItem::MOV, mov);
    itemsMap.insert(TrackItem::CUR, cur);
    qDebug() << "Time for insert: " << time.msecsTo(QTime::currentTime());
    return itemsMap;
}

void TrackInfo::calculateAbsCoordForKm(QVector<TrackItem> &km)
{
    QTime time = QTime::currentTime();
    qDebug() << "Calculate Abs Coord For Km";

    int absCoord = -1;

    for(auto it = km.begin(); it != km.end(); ++it)
    {
        it->absBegin = ++absCoord;
        absCoord += it->len;
        it->absEnd = absCoord;
        // qDebug() << "Km: " << it->km << "; Begin: " << it->beginM << "/" << it->absBegin << "; End: " << it->endM << "/" << it->absEnd << "; Len: " << it->len;
    }
    qDebug() << "Time km: " << time.msecsTo(QTime::currentTime());
}

void TrackInfo::calculateAbsCoord(QVector<TrackItem> &trackItems, QVector<TrackItem> &km)
{
    qDebug() << "Calculate Abs Coord";
    QTime time = QTime::currentTime();
    if(trackItems.first().type == TrackItem::STR)
    {
        for(auto it = trackItems.begin(); it != trackItems.end(); ++it)
        {
            it->absBegin = getAbsCoord(km, it->beginKM, it->beginM);
            //qDebug() << "Str: Begin: " << it->beginKM << "km " << it->beginM
            //         << " Abs Begin: " << it->absBegin;
        }
    }
    else if(trackItems.first().type == TrackItem::STAN)
    {
        for(auto it = trackItems.begin(); it != trackItems.end(); ++it)
        {
            it->absBegin = getAbsCoord(km, it->beginKM, it->beginM);
            it->absO = getAbsCoord(km, it->oKM, it->oM);
            it->absEnd = getAbsCoord(km, it->endKM, it->endM);
             //qDebug() << "Stan: " << it->name << "; Begin" << it->beginKM << "km " << it->beginM << "m; " << "Os: " << it->oKM << " km " << it->oM << "m; End: " << it->endKM << " km " << it->endM << " m; "
             //       << " Abs Begin: " << it->absBegin << "; Abs Os: " << it->absO << "; Abs End: " << it->absEnd;
        }
    }
    else if(trackItems.first().type == TrackItem::PCH)
    {
        for(auto it = trackItems.begin(); it != trackItems.end(); ++it)
        {
            it->absBegin = getAbsCoord(km, it->beginKM, it->beginM);
            it->absEnd = getAbsCoord(km, it->endKM, it->endM);
            //qDebug() << "Pch: Begin" << it->beginKM << "km " << it->beginM << "m; End: " << it->endKM << " km " << it->endM << " m; "
            //         << " Abs Begin: " << it->absBegin  << "; Abs End: " << it->absEnd;
        }
    }
    else if(trackItems.first().type == TrackItem::MOST)
    {
        for(auto it = trackItems.begin(); it != trackItems.end(); ++it)
        {
            it->absBegin = getAbsCoord(km, it->beginKM, it->beginM);
            it->absEnd = getAbsCoord(km, it->endKM, it->endM);
            //qDebug() << "Most: Begin" << it->beginKM << "km " << it->beginM << "m; End: " << it->endKM << " km " << it->endM << " m; "
            //         << " Abs Begin: " << it->absBegin  << "; Abs End: " << it->absEnd;
        }
    }
    else if(trackItems.first().type == TrackItem::MOV)
    {
        for(auto it = trackItems.begin(); it != trackItems.end(); ++it)
        {
            it->absBegin = getAbsCoord(km, it->beginKM, it->beginM);
            it->absEnd = getAbsCoord(km, it->endKM, it->endM);
            //qDebug() << "Mov: Begin" << it->beginKM << "km " << it->beginM << "m; End: " << it->endKM << " km " << it->endM << " m; "
             //        << " Abs Begin: " << it->absBegin  << "; Abs End: " << it->absEnd;
        }
    }
    else if(trackItems.first().type == TrackItem::CUR)
    {
        for(auto it = trackItems.begin(); it != trackItems.end(); ++it)
        {
            it->absBegin = getAbsCoord(km, it->beginKM, it->beginM);
            // qDebug() << "Cur: Begin" << it->beginKM << "km " << it->beginM << "m;"
            //         << " Abs Begin: " << it->absBegin;
        }
    }
    qDebug() << "Time: " << time.msecsTo(QTime::currentTime());
}


int TrackInfo::getAbsCoord(QVector<TrackItem> &km, int pathKm, int pathM)
{
    int absCoord = -1;
    for(auto it = km.begin(); it != km.end(); ++it)
    {
        if(pathKm == it->km)
        {
            absCoord = it->absBegin + pathM;
            break;
        }
    }

    return absCoord;
}
