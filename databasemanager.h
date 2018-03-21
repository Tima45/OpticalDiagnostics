#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QDebug>
#include <QVector>
#include <QTimer>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QReadWriteLock>
#include "databaseelement.h"

class DataBaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DataBaseManager(int flushSec,QObject *parent = 0);
    ~DataBaseManager();
    void setFlushSec(int sec);
    bool status = false;
    QSqlDatabase dataBase;
    QList<DataBaseElement> awaitingData;
private:

    int flushSec = 10;

    QTimer *timer;

    QReadWriteLock lock;
signals:
    void flushStatus(bool status);
public slots:
    void addData(DataBaseElement data);
    void flushData();
};

#endif // DATABASEMANAGER_H
