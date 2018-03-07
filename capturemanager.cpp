#include "capturemanager.h"

CaptureManager::CaptureManager(unsigned int grabInterval, QObject *parent) : QObject(parent)
{
    grabTimer = new QTimer(this);
    grabTimer->setInterval(grabInterval);
    grabTimer->setSingleShot(true);
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
        }
    }else{
        lock.unlock();
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
            emit newFrame(type,pic);
        }else{
            emit losedConnection(type);
            cap.release();
        }
    }else{
        lock.unlock();
        emit losedConnection(type);
    }
}

void CaptureManager::open()
{
    lock.lockForWrite();
    if(!cap.isOpened()){
        cap.open(connectionString.toStdString());
        if(cap.isOpened()){
            grabTimer->start();
            emit openResult(type,true);
        }else{
            emit openResult(type,false);
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
    if(cap.grab()){
        grabTimer->start();
    }else{
        emit losedConnection(type);
        cap.release();
    }
}
