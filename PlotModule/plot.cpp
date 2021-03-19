#include "plot.h"

Plot::Plot(QObject *parent) : QObject(parent)
{

}

void Plot::setupPlot(QCustomPlot *_plot)
{
    if(_plot == nullptr)
    {
        qDebug() << "Error: qcustomplot is nullptr";
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

    plot->yAxis->setTicker(itemTypesTextTicker);
    plot->yAxis->setRange(-0.05, 1.3);

    // itemTypesTextTicker->addTick(yCUR, "Кривые");
//    itemTypesTextTicker->addTick(2, "Переезды");
//    itemTypesTextTicker->addTick(3, "Мосты");
//    itemTypesTextTicker->addTick(4, "ПЧ");
//    itemTypesTextTicker->addTick(5, "Станции");


    connect(plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));

    kmGraph = plot->addGraph();
    strLeftGraph = plot->addGraph();
    strRightGraph = plot->addGraph();
    stanGraph = plot->addGraph();
    posGraph = plot->addGraph();
    movGraph = plot->addGraph();
    mostGraph = plot->addGraph();

    posGraph->setLineStyle(QCPGraph::lsNone);
    posGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::blue, Qt::green, 10));



}

void Plot::xAxisChanged(const QCPRange range)
{
    // qDebug() << "Redraw Bounds " << range.lower << " / " << range.upper;



}

void Plot::drawObjects(QMap<TrackItem::TrackItemType, QVector<TrackItem>> &itemsMap)
{
    qDebug() << "Drawing objects at qcustomplot";



    const QVector<TrackItem> &km = itemsMap[TrackItem::KM];

    const TrackItem &kmMin = km.first();
    const TrackItem &kmMax = km.last();
    qDebug() << "Min km: " << kmMin.km << "/" << kmMin.absBegin << "; Max km: " << kmMax.km << "/" << kmMax.absBegin;

    plot->xAxis->setRange(kmMin.absBegin, kmMax.absBegin);

    QCPItemLine *posLine = new QCPItemLine(plot);
    posLine->start->setCoords(kmMin.absBegin, yPos);
    posLine->end->setCoords(kmMax.absBegin, yPos);

    drawKM(km);
    drawSTR(itemsMap[TrackItem::STR]);
    drawMOV(itemsMap[TrackItem::MOV]);
    drawMOST(itemsMap[TrackItem::MOST]);
    drawPCH(itemsMap[TrackItem::PCH]);
    drawSTAN(itemsMap[TrackItem::STAN]);
    drawCUR(itemsMap[TrackItem::CUR]);



}

void Plot::drawKM(const QVector<TrackItem> &km)
{
    QSharedPointer<QCPAxisTickerText> kmTextTicker(new QCPAxisTickerText);

    plot->xAxis->setTicker(kmTextTicker);

    kmVec = km;

    for(const TrackItem &kmItem: km)
    {
        QCPItemText *kmTextLabel = new QCPItemText(plot);
        kmTextLabel->position->setCoords(kmItem.absBegin + kmItem.len/2, 0.01);
        kmTextLabel->setText(QString::number(kmItem.len));
        kmTextLabel->setAntialiased(false);

//        QCPItemText *kmTextLabel2 = new QCPItemText(plot);
//        kmTextLabel2->position->setCoords(kmItem.absBegin + kmItem.len/2, 0.4);
//        kmTextLabel2->setText(QString::number(kmItem.km));
//        kmTextLabel2->setAntialiased(false);

        kmTextTicker->addTick(kmItem.absBegin, QString::number(kmItem.km));
    }

}


void Plot::drawSTR(const QVector<TrackItem> &str)
{
    QVector<double> xLeft, xRight, yLeft, yRight;

    for(const TrackItem &strItem: str)
    {
        if(strItem.name == "1")
            xLeft.push_back(strItem.absBegin);
        else if(strItem.name == "2")
            xRight.push_back(strItem.absBegin);

        QCPItemText *strLabel = new QCPItemText(plot);
        strLabel->position->setCoords(strItem.absBegin,ySTR + 0.3);
        strLabel->setText(QString::number(strItem.numb));

    }

    yLeft = QVector<double>(xLeft.size(), ySTR);
    yRight = QVector<double>(xRight.size(), ySTR);


    strLeftGraph->setLineStyle(QCPGraph::lsNone);
    strLeftGraph->setScatterStyle(QCPScatterStyle(QPixmap(QApplication::applicationDirPath() + "/images/str.bmp")));
    strLeftGraph->setData(xLeft, yLeft);

    strRightGraph->setLineStyle(QCPGraph::lsNone);
    strRightGraph->setScatterStyle(QCPScatterStyle(QPixmap(QApplication::applicationDirPath() + "/images/str.bmp").transformed(QTransform().scale(-1, 1))));
    strRightGraph->setData(xRight, yRight);

}

void Plot::drawMOV(const QVector<TrackItem> &mov)
{
    QVector<double> x, y;

    y = QVector<double>(mov.size(), yMOV);

    for(const TrackItem &movItem: mov)
    {
//        QCPItemLine *line = new QCPItemLine(plot);

//        line->start->setCoords(movItem.absBegin, yMOV);
//        line->end->setCoords(movItem.absEnd, yMOV);

//        line->setTail(QCPLineEnding::esSquare);
//        line->setHead(QCPLineEnding::esSquare);
        double movPos = movItem.absBegin + (movItem.absBegin - movItem.absEnd) / 2;

        QCPItemText *movLabel = new QCPItemText(plot);

        movLabel->position->setCoords(movPos + 1, yMOV + 0.2);
        movLabel->setText(QString::number(movItem.absEnd - movItem.absBegin));

        x.push_back(movItem.absBegin);
    }


    movGraph->setLineStyle(QCPGraph::lsNone);
    movGraph->setScatterStyle(QCPScatterStyle(QPixmap(QApplication::applicationDirPath() + "/images/mov.bmp")));
    movGraph->setData(x, y);
}

void Plot::drawMOST(const QVector<TrackItem> &most)
{
    QVector<double> x, y;

    y = QVector<double>(most.size(), yMOST);
    for(const TrackItem &mostItem: most)
    {
//        QCPItemLine *line = new QCPItemLine(plot);

//        line->start->setCoords(mostItem.absBegin, yMOST);
//        line->end->setCoords(mostItem.absEnd, yMOST);

//        line->setTail(QCPLineEnding::esFlatArrow);
//        line->setHead(QCPLineEnding::esFlatArrow);
        double mostPos = mostItem.absBegin + (mostItem.absBegin - mostItem.absEnd) / 2;

        QCPItemText *mostLabel = new QCPItemText(plot);

        mostLabel->position->setCoords(mostPos, yMOST + 0.15);
        mostLabel->setText(QString::number(mostItem.absEnd - mostItem.absBegin));
        x.push_back(mostPos);
    }
    mostGraph->setLineStyle(QCPGraph::lsNone);
    mostGraph->setScatterStyle(QCPScatterStyle(QPixmap(QApplication::applicationDirPath() + "/images/most.bmp")));
    mostGraph->setData(x, y);
}

void Plot::drawPCH(const QVector<TrackItem> &pch)
{
    for(const TrackItem &pchItem: pch)
    {
        QCPItemLine *line = new QCPItemLine(plot);

        line->start->setCoords(pchItem.absBegin, 4);
        line->end->setCoords(pchItem.absEnd, 4);

        line->setTail(QCPLineEnding::esBar);
        line->setHead(QCPLineEnding::esBar);
    }
}

void Plot::drawSTAN(const QVector<TrackItem> &stan)
{
    QVector<double> x, y;

    y = QVector<double>(stan.size(), ySTAN + 0.25);

    for(const TrackItem &stanItem: stan)
    {
        QCPItemLine *line = new QCPItemLine(plot);

        line->start->setCoords(stanItem.absBegin,ySTAN);
        line->end->setCoords(stanItem.absEnd, ySTAN);

        line->setTail(QCPLineEnding::esBar);
        line->setHead(QCPLineEnding::esBar);

        x.push_back(stanItem.absO);

        QCPItemText *stanName = new QCPItemText(plot);
        stanName->position->setCoords(stanItem.absBegin + (stanItem.absEnd - stanItem.absBegin) /2, ySTAN + 0.1);
        stanName->setText(stanItem.name);
    }

    stanGraph->setLineStyle(QCPGraph::lsNone);
    stanGraph->setScatterStyle(QCPScatterStyle(QPixmap(QApplication::applicationDirPath() + "/images/stan.bmp")));
//    stanGraph->setLineStyle(QCPGraph::lsNone);
//    stanGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, Qt::green, Qt::red, 5));
    stanGraph->setData(x, y);




}

void Plot::drawCUR(const QVector<TrackItem> &cur)
{
    for(auto it = cur.begin() + 1; it != cur.end(); ++it)
    {
        QCPItemLine *line = new QCPItemLine(plot);

        auto prev = it - 1;

        line->start->setCoords(
                    prev->absBegin,
                    prev->oKM == 0 ? yCUR :
                                     prev->oKM < 0 ? yCUR - 0.1 : yCUR + 0.1);

        line->end->setCoords(
                    it->absBegin,
                    it->oKM == 0 ? yCUR :
                                   it->oKM < 0 ? yCUR - 0.1 : yCUR + 0.1);

        if(prev->oKM != 0 || it->oKM != 0)
        {
            line->setTail(QCPLineEnding::esBar);
            line->setHead(QCPLineEnding::esBar);
        }

        if(prev->oKM != 0 && it->oKM != 0)
        {
            QCPItemText *curLabel = new QCPItemText(plot);

            curLabel->position->setCoords(prev->absBegin + (it->absBegin - prev->absBegin) / 2, yCUR - 0.2);
            curLabel->setText("R: " + QString::number(it->oKM));
        }

    }
}

void Plot::changePosition(int absPos)
{
    //qDebug() << "New Position: " << absPos;

    QVector<double> x, y;
    x.push_back(absPos);
    y.push_back(0);

    posGraph->setData(x, y);
    plot->replot();
}


void Plot::changePosition(QString pos)
{
    //qDebug() << "New Position: " << pos;


    int _km = pos.split(";").first().toInt();
    int _m = pos.split(";").at(1).toInt() / 1000;
    // qDebug() << "km: " << _km << " m: " << _m;

    // qDebug() << "First km abs: " << kmVec.first().absBegin;
    changePosition(getAbsCoord(_km, _m));
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
