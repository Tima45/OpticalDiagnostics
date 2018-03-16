#ifndef DATABASEELEMENT_H
#define DATABASEELEMENT_H

#include <QDateTime>

class DataBaseElement
{
public:
    DataBaseElement();
    QDateTime time;
    double x;
    double y;
    double width;
    double height;
    char soothingType;
};

#endif // DATABASEELEMENT_H
