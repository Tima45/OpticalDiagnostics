#ifndef TRACERMANAGER_H
#define TRACERMANAGER_H

#include <QObject>
#include <QDebug>
#include <QColor>
#include "plot/qcustomplot.h"

class TracerManager : public QObject
{
    Q_OBJECT
public:
    explicit TracerManager(int tracerCount,QCustomPlot *plot,QObject *parent = 0);
    void apdateTracers(double x, double y);
    int tracerCount;
    QCustomPlot *plot;
    QVector<QCPItemTracer*> tracers;
    QVector<QPen> colors;
signals:

public slots:
};

#endif // TRACERMANAGER_H
