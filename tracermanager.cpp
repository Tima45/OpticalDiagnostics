#include "tracermanager.h"


TracerManager::TracerManager(int tracerCount, QCustomPlot *plot, QObject *parent) : QObject(parent), tracerCount(tracerCount),plot(plot)
{
    for(int i = 0; i < tracerCount; i++){
        QPen p;
        p.setBrush((QColor(0,255,255,qRound((double)i/(double)tracerCount))));
        colors.append(p);
        tracers.append(new QCPItemTracer(plot));
        tracers.last()->setPen(p);
    }
}

void TracerManager::apdateTracers(double x, double y)
{
    for(int i = 0; i < tracerCount-1; i++){
        tracers.at(i+1)->setPen(colors.at(i));
    }
    tracers.at(tracerCount-1)->position->setCoords(x,y);

}
