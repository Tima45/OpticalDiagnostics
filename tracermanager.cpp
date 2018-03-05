#include "tracermanager.h"


TracerManager::TracerManager(int tracerCount, QCustomPlot *plot, QObject *parent) : QObject(parent), tracerCount(tracerCount),plot(plot)
{

    for(int i = 0; i < tracerCount; i++){
        QCPItemTracer *t = new QCPItemTracer(plot);
        t->setLayer("mid");
        t->position->setCoords(0,0);
        t->setStyle(QCPItemTracer::tsCircle);
        QBrush b = QBrush(QColor(0,220,200,20));
        t->setBrush(b);
        t->setPen(Qt::NoPen);
        tracers.append(t);
    }
}

void TracerManager::apdateLines(double x, double y)
{
    tracers.at(iterator)->position->setCoords(x,y);
    iterator++;
    if(iterator == tracerCount){
        iterator = 0;
    }

}
