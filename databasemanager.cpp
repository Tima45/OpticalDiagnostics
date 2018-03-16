#include "databasemanager.h"

DataBaseManager::DataBaseManager(int flushSec, QObject *parent) : QObject(parent) , flushSec(flushSec)
{
    awaitingData.reserve(1800);

    timer = new QTimer(this);
    timer->setInterval(flushSec*1000);
    connect(timer,SIGNAL(timeout()),this,SLOT(flushData()));
    timer->start();

    dataBase = QSqlDatabase::addDatabase("QPSQL");
    dataBase.setDatabaseName("name");
    dataBase.setUserName("OpticalDiagnostics");
    dataBase.setPassword("optdiag");
    dataBase.setHostName("192.168.0.220");
    dataBase.setPort(5432);

    if(dataBase.open()){
        status = true;
        dataBase.close();
    }else{
        status = false;
        qDebug() << "Not database connection" << dataBase.lastError().text();
    }
}

DataBaseManager::~DataBaseManager()
{

}

void DataBaseManager::setFlushSec(int sec)
{
    flushSec = sec;
    timer->setInterval(sec);
}

void DataBaseManager::addData(DataBaseElement data)
{
    lock.lockForWrite();
    awaitingData.append(data);
    lock.unlock();
}

void DataBaseManager::flushData()
{
    lock.lockForWrite();
    if(!awaitingData.isEmpty()){
        if(dataBase.open()){
            status = true;
            QString sqlQuery = "INSERT INTO \"OpticalDiagnostics\" (time,xMax,yMax,width,height,smoothType) VALUES ";
            for(int i = 0; i < awaitingData.count(); i++){
                sqlQuery += "(";
                sqlQuery += ("'"+awaitingData.at(i).time.toString("yyyy-MM-dd hh:mm:ss")+" + 7.0',");
                sqlQuery += (QString::number(awaitingData.at(i).x)+",");
                sqlQuery += (QString::number(awaitingData.at(i).y)+",");

                if(awaitingData.at(i).width == -1){
                    sqlQuery += "NULL,";
                }else{
                    sqlQuery += (QString::number(awaitingData.at(i).width)+",");
                }

                if(awaitingData.at(i).height == -1){
                    sqlQuery += "NULL,";
                }else{
                    sqlQuery += (QString::number(awaitingData.at(i).height)+",");
                }
                sqlQuery += (QString::number(awaitingData.at(i).soothingType));
                sqlQuery += "),";
            }
            sqlQuery.chop(1);
            //qDebug() << (const char *)sqlQuery.toStdString().c_str();
            QSqlQuery q(sqlQuery);
            if(q.exec()){
                awaitingData.clear();
            }
            dataBase.close();
        }else{
            status = false;
            qDebug() << "Not database connection" << dataBase.lastError().text();
        }
    }
    emit flushStatus(status);
    lock.unlock();
}