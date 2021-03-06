#include "databasemanager.h"

DataBaseManager::DataBaseManager(QString dataBaseIp,int flushSec, QObject *parent) : QObject(parent) , flushSec(flushSec)
{
    awaitingData.reserve(1800);

    timer = new QTimer(this);
    timer->setInterval(flushSec*1000);
    connect(timer,SIGNAL(timeout()),this,SLOT(flushData()));
    timer->start();

    dataBase = QSqlDatabase::addDatabase("QPSQL");
    dataBase.setDatabaseName("Experiments");
    dataBase.setUserName("tima45");
    dataBase.setPassword("optdiag");
    dataBase.setHostName(dataBaseIp);
    dataBase.setPort(5432);

    if(dataBase.open()){
        status = true;
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
    if(awaitingData.count() < 60*60){
        awaitingData.append(data);
    }else{
        awaitingData.removeFirst();
        awaitingData.append(data);
    }
    lock.unlock();
}

void DataBaseManager::flushData()
{
    emit flushStatus(true);
    lock.lockForWrite();
    if(!awaitingData.isEmpty()){
        if(dataBase.isOpen()){
            QString sqlQuery = "INSERT INTO public.\"OpticalDiagnostics\" (time,x_max,y_max,width,height,smooth_type) VALUES ";
            for(int i = 0; i < awaitingData.count(); i++){
                sqlQuery += "(";
                sqlQuery += ("'"+awaitingData.at(i).time.toString("yyyy-MM-dd hh:mm:ss")+"+07',");
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
            sqlQuery += ";";

            qDebug() << (const char *)sqlQuery.toStdString().c_str();

            QSqlQuery q;
            if(q.exec(sqlQuery)){
                awaitingData.clear();
                status = true;
            }else{
                qDebug() << q.lastError().text();
                dataBase.close();
                if(dataBase.open()){
                    qDebug() << "Connected.";
                    if(q.exec(sqlQuery)){
                        awaitingData.clear();
                        status = true;
                    }else{
                        qDebug() << "Could not connect";
                        status = false;
                    }
                }else{
                    qDebug() << "Could not connect";
                    status = false;
                }
            }
        }else{
            status = false;
            qDebug() << "Not database connection" << dataBase.lastError().text();
            if(dataBase.open()){
                qDebug() << "Connected.";
                lock.unlock();
                flushData();
                lock.lockForWrite();
            }else{
                qDebug() << "Could not connect";
            }
        }
    }
    emit flushStatus(status);
    lock.unlock();
}
