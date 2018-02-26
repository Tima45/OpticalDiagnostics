#include "capturemanager.h"

CaptureManager::CaptureManager(unsigned int grabInterval, QObject *parent) : QObject(parent)
{
    grabTimer = new QTimer(this);
    grabTimer->setInterval(grabInterval);
    connect(grabTimer,SIGNAL(timeout()),this,SLOT(grabFrame()));
}

CaptureManager::~CaptureManager()
{
    grabTimer->stop();
    cap.release();
}

bool CaptureManager::isOpend()
{
    return cap.isOpened();
}

Mat CaptureManager::takeSingleFrame()
{
    Mat pic;
    lock.lockForRead();
    if(cap.isOpened()){
        lock.unlock();
        if(cap.retrieve(pic)){
            return pic;
        }else{
            qDebug() << name << QTime::currentTime().toString() << "not readed!";
        }
    }else{
        lock.unlock();
        qDebug() << name << QTime::currentTime().toString() << "not open!";
    }
    return pic;
}

void CaptureManager::getFrame()
{
    lock.lockForRead();
    if(cap.isOpened()){
        lock.unlock();
        Mat pic;
        if(cap.retrieve(pic)){
            emit newFrame(name,pic);
        }else{
            qDebug() << name << QTime::currentTime().toString() << "not retrieved!";
            emit losedConnection(name);
            grabTimer->stop();
            cap.release();
        }
    }else{
        lock.unlock();
        qDebug() << name << QTime::currentTime().toString() << "not open!";
        emit losedConnection(name);
        grabTimer->stop();
    }
}

void CaptureManager::open()
{
    qDebug() << name << QTime::currentTime().toString() << "trying to open";
    lock.lockForWrite();
    if(!cap.isOpened()){
        cap.open(connectionString.toStdString());
        if(cap.isOpened()){
            grabTimer->start();
            qDebug() << name << QTime::currentTime().toString() << "done";
            emit openResult(name,true);
        }else{
            qDebug() << name << QTime::currentTime().toString() << "could not open";
            emit openResult(name,false);
        }
    }
    lock.unlock();
}

void CaptureManager::release()
{
    lock.lockForWrite();
    grabTimer->stop();
    cap.release();
    lock.unlock();
}

void CaptureManager::setConnectionString(QString connectionString)
{
    this->connectionString = connectionString;
}


void CaptureManager::grabFrame()
{
    if(!cap.grab()){
        grabTimer->stop();
        emit losedConnection(name);
        cap.release();
        qDebug() << name << QTime::currentTime().toString() << "Could bot grab.";
    }
}
