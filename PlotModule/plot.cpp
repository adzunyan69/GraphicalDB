#include "plot.h"


Plot::Plot(QObject *parent) : QObject(parent)
{

}

void Plot::setupPlot(QCustomPlot *_plot)
{
    if(_plot == nullptr)
    {
        qDebug() << "Error: qcustomplot is nullptr";
        emit error("Ошибка: указатель на qcustomplot = nullptr.");
        return;
    }

    plot = _plot;

    plot->setNotAntialiasedElements(QCP::aeAll);
    plot->setNoAntialiasingOnDrag (true);
    plot->axisRect()->setRangeDrag(Qt::Horizontal);
    plot->axisRect()->setRangeZoom(Qt::Horizontal);
    plot->setInteraction(QCP::iRangeZoom, true);
    plot->setInteraction(QCP::iRangeDrag, true);


    QSharedPointer<QCPAxisTickerText> itemTypesTextTicker(new QCPAxisTickerText);
    plot->xAxis->setTickLengthIn(15);
    plot->xAxis->setRange(0, 10000);


    plot->yAxis->setRange(-0.05, 1.5);
    plot->yAxis->setTicker(itemTypesTextTicker);


    plot->setNotAntialiasedElements(QCP::aeAll);
    plot->setNoAntialiasingOnDrag(true);
    plot->yAxis->setTickLengthIn(0);
    plot->yAxis->grid()->setVisible(false);
    itemTypesTextTicker->addTick(yCUR, "План линий");
    itemTypesTextTicker->addTick(yCUR + yCURdiff, "Левая кривая");
    itemTypesTextTicker->addTick(yCUR - yCURdiff, "Правая кривая");
    itemTypesTextTicker->addTick(ySTAN + yPCHdiff / 2, "Объекты ЖД пути");
    itemTypesTextTicker->addTick(yPCH - yPCHdiff / 2, "Дистанции пути");
    itemTypesTextTicker->addTick(ySPD - ySPDdiff / 2, "Скорости");
    itemTypesTextTicker->addTick(ySPD + ySPDdiff / 2, "Скорости ВСК");
    itemTypesTextTicker->addTick(yKM - yKMdiff, "Длины КМ");



    kmGraph = plot->addGraph();
    strLeftGraph = plot->addGraph();
    strRightGraph = plot->addGraph();
    stanGraph = plot->addGraph();
    posGraph = plot->addGraph();
    movGraph = plot->addGraph();
    mostGraph = plot->addGraph();
    spdGraph = plot->addGraph();
    curGraph = plot->addGraph();

    posGraph->setLineStyle(QCPGraph::lsNone);
    posGraph->setScatterStyle(QCPScatterStyle(QPixmap(QApplication::applicationDirPath() + "/images/train.png")));
    curGraph->setPen(QPen(Qt::black));
    strLeftGraph->setVisible(true);
    strRightGraph->setVisible(true);

    plot->xAxis->rescale(true);
    connect(plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(rangeChangedLimit(QCPRange)));



}

void Plot::setReversed(bool _reversed)
{
    reversed = _reversed;
    plot->xAxis->setRangeReversed(reversed);
}
void Plot::drawObjects(QMap<TrackItem::TrackItemType, QVector<TrackItem>> &itemsMap)
{
    qDebug() << "Drawing objects at qcustomplot";

    plot->clearItems();

    // itemsMap = _itemsMap;

    kmVec = itemsMap[TrackItem::KM];
    pchVec = itemsMap[TrackItem::PCH];
    spdVec = itemsMap[TrackItem::SPD];
    stanVec = itemsMap[TrackItem::STAN];

    qDebug() << "Getting kmVector";
    const QVector<TrackItem> &km = itemsMap[TrackItem::KM];

    if(km.isEmpty())
    {
        qDebug() << "Отсутствуют километры.";
        emit error("Отсутствует список километров - возможно, ошибка при выполнении запрсоов к БД.");
        return;
    }

    const TrackItem &kmMin = km.first();
    const TrackItem &kmMax = km.last();
    qDebug() << "Min km: " << kmMin.km << "/" << kmMin.absBegin << "; Max km: " << kmMax.km << "/" << kmMax.absBegin;


    drawKM(km);
    drawSTR(itemsMap[TrackItem::STR]);
    drawMOV(itemsMap[TrackItem::MOV]);
    drawMOST(itemsMap[TrackItem::MOST]);
    drawPCH(itemsMap[TrackItem::PCH]);
    drawSTAN(itemsMap[TrackItem::STAN]);
    drawCUR(itemsMap[TrackItem::CUR]);
    drawSPD(itemsMap[TrackItem::SPD]);
    // drawSLEEPER(itemsMap[TrackItem::SLEEPER]);
    qDebug() << "end drawing";


}

void Plot::drawKM(const QVector<TrackItem> &km)
{
    qDebug() << "drawKM";
    QSharedPointer<QCPAxisTickerText> kmTextTicker(new QCPAxisTickerText);

    plot->xAxis->setTicker(kmTextTicker);


    for(const TrackItem &kmItem: km)
    {
        // qDebug() << "Current kmItem: " << kmItem.km;
        QCPItemText *kmTextLabel = new QCPItemText(plot);
        kmTextLabel->position->setCoords(kmItem.absBegin + kmItem.len/2, 0.01);
        kmTextLabel->setText(QString::number(kmItem.len));
        kmTextLabel->setAntialiased(false);

        kmTextTicker->addTick(kmItem.absBegin, QString::number(kmItem.km));
    }

    qDebug() << "Drawing kmLine";
    QCPItemStraightLine *kmLine = new QCPItemStraightLine(plot);
    kmLine->point1->setCoords(0, yKM);
    kmLine->point2->setCoords(1, yKM);

    qDebug() << "Drawing posLine";
    posLine = new QCPItemStraightLine (plot);
    posLine->setPen(QPen(Qt::DashDotDotLine));
}


void Plot::drawSTR(const QVector<TrackItem> &str)
{
    qDebug() << "Drawing STR";
    QVector<double> strAbsCoords, strHeights;
    QVector<double> xLeft, xRight, yLeft, yRight;

    for(const TrackItem &strItem: str)
    {
        // qDebug() << "Str abs: " << strItem.absBegin << "km m: " << strItem.beginKM << " / " << strItem.beginM << " numb: " << strItem.numb;
        if((strItem.name == "2" && reversed == false) || (strItem.name == "1" && reversed == true))
            xLeft.push_back(strItem.absBegin);
        else if((strItem.name == "1" && reversed == false)|| (strItem.name == "2" && reversed == true))
            xRight.push_back(strItem.absBegin);

        strAbsCoords.push_back(strItem.absBegin);
        strHeights.push_back(ySTR + ySTRdiff);
        if(strAbsCoords.size() > 1 && (strAbsCoords.last() - strAbsCoords.at(strAbsCoords.size() - 2)) < 25)
        {
            if(strHeights.at(strHeights.size() - 2) + 0.1 >= plot->yAxis->range().upper)
                strHeights.last() = strHeights.at(strHeights.size() - 2) - 0.1;
            else
                strHeights.last() = strHeights.at(strHeights.size() - 2) + 0.1;
        }


        if(strItem.name == "2" || strItem.name == "1")
        {
            // qDebug() << "Drawing str label";
            QCPItemText *strLabel = new QCPItemText(plot);
            strLabel->position->setCoords(strAbsCoords.last(), strHeights.last());
            strLabel->setPositionAlignment(Qt::AlignHCenter | Qt::AlignBottom);
            strLabel->setText(strItem.numb);
        }


    }

    yLeft = QVector<double>(xLeft.size(), ySTR);
    yRight = QVector<double>(xRight.size(), ySTR);

    // qDebug() << "Drawing str graphs";

    strLeftGraph->setLineStyle(QCPGraph::lsNone);
    strLeftGraph->setScatterStyle(QCPScatterStyle(QPixmap(QApplication::applicationDirPath() + "/images/str.png")));
    strLeftGraph->setData(xLeft, yLeft);

    strRightGraph->setLineStyle(QCPGraph::lsNone);
    strRightGraph->setScatterStyle(QCPScatterStyle(QPixmap(QApplication::applicationDirPath() + "/images/str.png").transformed(QTransform().scale(-1, 1))));
    strRightGraph->setData(xRight, yRight);



}

void Plot::drawMOV(const QVector<TrackItem> &mov)
{
    qDebug() << "Drawing mov";
    QVector<double> x, y;

    y = QVector<double>(mov.size(), yMOV);

    for(const TrackItem &movItem: mov)
    {
        double movPos = movItem.absBegin + (movItem.absEnd - movItem.absBegin) / 2;

        //  qDebug() << "Drawing mov: " << movPos;
        QCPItemText *movLabel = new QCPItemText(plot);

        movLabel->position->setCoords(movPos + 4, yMOV + yMOVdiff);
        movLabel->setText(QString::number(movItem.absEnd - movItem.absBegin));

        x.push_back(movItem.absBegin);
    }

    qDebug() << "Draing mov line";

    movGraph->setLineStyle(QCPGraph::lsNone);
    movGraph->setScatterStyle(QCPScatterStyle(QPixmap(QApplication::applicationDirPath() + "/images/mov.png")));
    movGraph->setData(x, y);
}

void Plot::drawMOST(const QVector<TrackItem> &most)
{
    qDebug() << "Drawing most";
    QVector<double> x, y;

    y = QVector<double>(most.size(), yMOST);
    for(const TrackItem &mostItem: most)
    {
        double mostPos = mostItem.absBegin + (mostItem.absEnd - mostItem.absBegin) / 2;

        QCPItemText *mostLabel = new QCPItemText(plot);

        mostLabel->position->setCoords(mostPos, yMOST + 0.15);
        mostLabel->setText(QString::number(mostItem.absEnd - mostItem.absBegin));
        x.push_back(mostPos);
    }
    qDebug() << "Drawing most graph";
    mostGraph->setLineStyle(QCPGraph::lsNone);
    mostGraph->setScatterStyle(QCPScatterStyle(QPixmap(QApplication::applicationDirPath() + "/images/most.png")));
    mostGraph->setData(x, y);
}

void Plot::drawPCH(const QVector<TrackItem> &pch)
{
    qDebug() << "Drawing PCH";
    QCPItemStraightLine *linePCHTop = new QCPItemStraightLine(plot);
    linePCHTop->point1->setCoords(0, yPCH + yPCHdiff);
    linePCHTop->point2->setCoords(1, yPCH + yPCHdiff);

    QCPItemStraightLine *linePCHBottom = new QCPItemStraightLine(plot);
    linePCHBottom->point1->setCoords(0, yPCH - yPCHdiff);
    linePCHBottom->point2->setCoords(1, yPCH - yPCHdiff);
    for(const TrackItem &pchItem: pch)
    {
        // qDebug() << "Drawing pch " << pchItem.name;
        QCPItemLine *lineLeft = new QCPItemLine(plot);

        lineLeft->start->setCoords(pchItem.absBegin, yPCH - yPCHdiff);
        lineLeft->end->setCoords(pchItem.absBegin, yPCH + yPCHdiff);

        QCPItemLine *lineRight = new QCPItemLine(plot);

        lineRight->start->setCoords(pchItem.absEnd, yPCH - yPCHdiff);
        lineRight->end->setCoords(pchItem.absEnd, yPCH + yPCHdiff);

        QCPItemText *pchLabel = new QCPItemText(plot);

        pchLabel->setText(" -> " + pchItem.name);
        pchLabel->setPositionAlignment(Qt::AlignLeft);
        if(reversed == false)
            pchLabel->position->setCoords(pchItem.absBegin + 5, yPCH);
        else
            pchLabel->position->setCoords(pchItem.absEnd + 5, yPCH);
    }
}

void Plot::drawSTAN(const QVector<TrackItem> &stan)
{
    qDebug() << "Drawing stan";
    QVector<double> x, y;

    y = QVector<double>(stan.size(), ySTAN + ySTANosDiff);

    for(const TrackItem &stanItem: stan)
    {
        // qDebug() << "Drawing stan " << stanItem.name;
        QCPItemLine *line = new QCPItemLine(plot);
        QPen pen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        line->setPen(pen);
        line->start->setCoords(stanItem.absBegin,ySTAN);
        line->end->setCoords(stanItem.absEnd, ySTAN);

        line->setTail(QCPLineEnding::esBar);
        line->setHead(QCPLineEnding::esBar);

        x.push_back(stanItem.absO);

        QCPItemText *stanName = new QCPItemText(plot);
        stanName->position->setCoords(stanItem.absBegin + (stanItem.absEnd - stanItem.absBegin) / 2, ySTAN + ySTANlabelDiff);
        stanName->setText(stanItem.name);
    }

    stanGraph->setLineStyle(QCPGraph::lsNone);
    stanGraph->setScatterStyle(QCPScatterStyle(QPixmap(QApplication::applicationDirPath() + "/images/stan.png")));
    stanGraph->setData(x, y);




}

void Plot::drawCUR(const QVector<TrackItem> &cur)
{
    qDebug() << "Drawing cur";
    if(cur.isEmpty())
    {
        qDebug() << "Отсутствуют данные для отрисовки, пропуск.";
        return;
    }
    QVector<double> x, y;
    QVector<double> curAbsCoords, curHeights;

    x.push_back(cur.begin()->absBegin);
    y.push_back(cur.begin()->oKM == 0 ? yCUR :
                                 cur.begin()->oKM < 0 ? yCUR - yCURdiff : yCUR + yCURdiff);

    bool switchLabelPosition = true;
    for(auto it = cur.begin() + 1; it != cur.end(); ++it)
    {

        x.push_back(it->absBegin);
        y.push_back(it->oKM == 0 ? yCUR :
                                   it->oKM < 0 ? yCUR - yCURdiff : yCUR + yCURdiff);

        auto prev = it - 1;
        if(prev->oKM != 0 && it->oKM != 0)
        {

//            if(prev->oKM == it->oKM && prev->oM == it->oM)
//                continue;

             QCPItemText *curLabel = new QCPItemText(plot);

             curLabel->setText("R:" + QString::number(it->oKM) + "\nИ:" + QString::number(it->oM));

             curAbsCoords.push_back(it->absBegin);
             curHeights.push_back(yCUR);


             if(curAbsCoords.size() > 1 && (curAbsCoords.last() - curAbsCoords.at(curAbsCoords.size() - 2)) < 50)
             {
                 if(switchLabelPosition)
                    curHeights.last() = curHeights.at(curHeights.size() - 2) + 0.1;
                 switchLabelPosition = !switchLabelPosition;
             }

             curLabel->position->setCoords(prev->absBegin + (it->absBegin - prev->absBegin) / 2,
                                           curHeights.last());
        }

    }

    curGraph->setData(x, y);

    QCPItemStraightLine *curLine = new QCPItemStraightLine(plot);
    curLine->point1->setCoords(0.0, yCUR);
    curLine->point2->setCoords(1.0, yCUR);
    curLine->setPen(QPen(Qt::DashLine));

    QCPItemStraightLine *curLineTop = new QCPItemStraightLine(plot);
    curLineTop->point1->setCoords(0.0, yCUR + yCURdiff);
    curLineTop->point2->setCoords(1.0, yCUR + yCURdiff);
    curLineTop->setPen(QPen(Qt::DotLine));

    QCPItemStraightLine *curLineBottom = new QCPItemStraightLine(plot);
    curLineBottom->point1->setCoords(0.0, yCUR - yCURdiff);
    curLineBottom->point2->setCoords(1.0, yCUR - yCURdiff);
    curLineBottom->setPen(QPen(Qt::DotLine));

}

void Plot::drawSPD(const QVector<TrackItem> &spd)
{
    qDebug() << "Drawing SPD";
    QVector<int> spdAbsCoords;

    for(auto it = spd.begin(); it != spd.end(); ++it)
    {
        QCPItemLine *lineHead = new QCPItemLine(plot);

        lineHead->start->setCoords(it->absBegin, ySPD - ySPDdiff);
        lineHead->end->setCoords(it->absBegin, ySPD + ySPDdiff);

        QCPItemLine *lineTail = new QCPItemLine(plot);

        lineTail->start->setCoords(it->absEnd, ySPD - ySPDdiff);
        lineTail->end->setCoords(it->absEnd, ySPD + ySPDdiff);

        QCPItemText *text1 = new QCPItemText(plot);

        if(reversed == false)
            text1->position->setCoords(it->absBegin + 7, ySPD + ySPDdiff);
        else
            text1->position->setCoords(it->absEnd - 7, ySPD + ySPDdiff);
        text1->setPositionAlignment(Qt::AlignLeft);
        text1->setText(it->name.split(";").first());

        QCPItemText *text2 = new QCPItemText(plot);

        if(reversed == false)
            text2->position->setCoords(it->absBegin + 7, ySPD);
        else
            text2->position->setCoords(it->absEnd - 7, ySPD);
        text2->setPositionAlignment(Qt::AlignLeft);
        text2->setText(it->name.split(";").at(1));

        spdAbsCoords.push_back(it->absBegin);

    }

    QCPItemStraightLine *spdLine = new QCPItemStraightLine(plot);
    spdLine->point1->setCoords(0, ySPD);
    spdLine->point2->setCoords(1, ySPD);

    QCPItemStraightLine *spdLineTop = new QCPItemStraightLine(plot);
    spdLineTop->point1->setCoords(0, ySPD + ySPDdiff);
    spdLineTop->point2->setCoords(1, ySPD + ySPDdiff);

    QCPItemStraightLine *spdLineBottom = new QCPItemStraightLine(plot);
    spdLineBottom->point1->setCoords(0, ySPD - ySPDdiff);
    spdLineBottom->point2->setCoords(1, ySPD - ySPDdiff);
}

void Plot::drawSLEEPER(const QVector<TrackItem> &sleeper)
{
    qDebug() << "Drawing SLEEPER";

    QCPItemStraightLine *ySleeperTopLine = new QCPItemStraightLine(plot);
    ySleeperTopLine->point1->setCoords(0, ySleeper + ySleeperDiff);
    ySleeperTopLine->point2->setCoords(1, ySleeper + ySleeperDiff);

    QCPItemStraightLine *ySleeperBottomLine = new QCPItemStraightLine(plot);
    ySleeperBottomLine->point1->setCoords(0, ySleeper - ySleeperDiff);
    ySleeperBottomLine->point2->setCoords(1, ySleeper - ySleeperDiff);

    for(int i = kmVec.first().absBegin; i < kmVec.last().absEnd; i+= 100)
    {
        QCPItemLine *line = new QCPItemLine(plot);
        line->start->setCoords(i, ySleeper - ySleeperDiff);
        line->end->setCoords(i, ySleeper);
    }

    for(auto it = sleeper.begin(); it != sleeper.end(); ++it)
    {
        if(it->absEnd <= it->absBegin)
        {
            qDebug() << "Неверные координаты шпалы: "
                     << it->absBegin << "/" << it->absEnd << "; "
                     << it->beginKM << "/" << it->beginM;
            continue;
        }
        QCPItemLine *lineTop = new QCPItemLine(plot);
        lineTop->setPen((QPen(Qt::red)));
        lineTop->start->setCoords(it->absBegin, ySleeper + ySleeperDiff);
        lineTop->end->setCoords(it->absEnd, ySleeper + ySleeperDiff);

        QCPItemLine *lineBottom = new QCPItemLine(plot);
        lineBottom->setPen((QPen(Qt::red)));
        lineBottom->start->setCoords(it->absBegin, ySleeper - ySleeperDiff);
        lineBottom->end->setCoords(it->absEnd, ySleeper - ySleeperDiff);


        QCPItemLine *lineLeft = new QCPItemLine(plot);
        lineLeft->setPen((QPen(Qt::red)));
        lineLeft->start->setCoords(it->absBegin, ySleeper - ySleeperDiff);
        lineLeft->end->setCoords(it->absBegin, ySleeper + ySleeperDiff);

        QCPItemLine *lineRight = new QCPItemLine(plot);
        lineRight->setPen((QPen(Qt::red)));
        lineRight->start->setCoords(it->absEnd, ySleeper - ySleeperDiff);
        lineRight->end->setCoords(it->absEnd, ySleeper + ySleeperDiff);
//        for(int i = it->absBegin; i <= it->absEnd; i+= 50)
//        {
//            QCPItemLine *line1 = new QCPItemLine(plot);
//            line1->start->setCoords(i - 2, ySleeper - ySleeperDiff);
//            line1->end->setCoords(i - 2, ySleeper + ySleeperDiff);
//            QCPItemLine *line2 = new QCPItemLine(plot);
//            line2->start->setCoords(i, ySleeper - ySleeperDiff);
//            line2->end->setCoords(i, ySleeper + ySleeperDiff);
//            QCPItemLine *line3 = new QCPItemLine(plot);
//            line3->start->setCoords(i + 2, ySleeper - ySleeperDiff);
//            line3->end->setCoords(i + 2, ySleeper + ySleeperDiff);
//        }
    }
}

void Plot::changePosition(int absPos)
{
    if(kmVec.isEmpty())
        return;
    QVector<double> x, y;
    x.push_back(absPos);
    y.push_back(yPos + 0.1);

    posGraph->setData(x, y);


    if(focus == false)
        plot->xAxis->setRange(absPos, plot->xAxis->range().size(), Qt::AlignCenter);

    posAbsCoord = absPos;
    posLine->point1->setCoords(posAbsCoord, 0);
    posLine->point2->setCoords(posAbsCoord, 1);

    plot->replot(QCustomPlot::rpQueuedReplot  );


    using namespace QtConcurrent;

    QtConcurrent::run(this, &Plot::checkSPD, absPos);
    QtConcurrent::run(this, &Plot::checkPCH, absPos);
    QtConcurrent::run(this, &Plot::checkDistance, absPos);
    //checkSPD(absPos);
   // checkPCH(absPos);
    //checkDistance(absPos);
}


void Plot::changePosition(QString pos)
{
    if(kmVec.isEmpty())
        return;


    bool isCorrectPathCoord;
    int _km = pos.split(";").first().toInt(&isCorrectPathCoord);
    if(isCorrectPathCoord == false)
    {
        qDebug() << "Invalid path coord: km";
        emit error("Получена неверная координата: " + pos);
        return;
    }

    int _m = pos.split(";").at(1).toInt(&isCorrectPathCoord) / 1000;
    if(isCorrectPathCoord == false)
    {
        qDebug() << "Invalid path coord: m";
        emit error("Получена неверная координата: " + pos);
        return;
    }

    if(_km != prevKm || _m != prevM)
        changePosition(getAbsCoord(_km, _m));
}

void Plot::windowFocusChanged(bool isWindowOnFocus)
{
    focus = isWindowOnFocus;
}

void Plot::checkSPD(int absPos)
{
    QString spd;
    for(auto it = spdVec.begin(); it != spdVec.end(); ++it)
        if(absPos >= it->absBegin && absPos <= it->absEnd)
            spd = it->name;

    if(spd != currentSPD)
    {
        currentSPD = spd;
        emit spdChanged(currentSPD);
    }
}

void Plot::checkPCH(int absPos)
{
    QString pch;
    for(auto it = pchVec.begin(); it != pchVec.end(); ++it)
    {
        if(absPos >= it->absBegin && absPos <= it->absEnd)
            pch = it->name;

    }

    if(pch != currentPCH)
    {
        currentPCH = pch;
        emit pchChanged(currentPCH);
    }
}

void Plot::checkDistance(int absPos)
{
    QString distance;
    for(auto it = stanVec.begin(); it != stanVec.end(); ++it)
    {
        if(absPos >= it->absBegin && absPos <= it->absEnd)
            distance = it->name;
        else if(reversed == false && it != stanVec.end())
        {
            if(absPos > it->absEnd && absPos < (it + 1)->absBegin)
                distance = it->name + " - " + (it + 1)->name;
        }
        else if(reversed == true && it != stanVec.begin())
        {
            if(absPos < it->absBegin && absPos > (it - 1)->absEnd)
                distance = it->name + " - " + (it - 1)->name;
        }
        else
            distance = " - ";
    }

    if(distance != currentDistance)
    {
        currentDistance = distance;
        emit distanceChanged(currentDistance);
    }
}

int Plot::getAbsCoord(int pathKm, int pathM)
{
    int absCoord = -1;
    for(auto it = kmVec.begin(); it != kmVec.end(); ++it)
    {
        if(pathKm == it->km)
        {
            // qDebug() << "Found coord: " << it->absBegin + " / " << it->km;
            absCoord = it->absBegin + pathM;
            break;
        }
    }

    return absCoord;
}

void Plot::rangeChangedLimit(QCPRange newRange)
{
        plot->xAxis->setRange(newRange.bounded(kmVec.first().absBegin - 5000, kmVec.last().absEnd + 5000));
}
