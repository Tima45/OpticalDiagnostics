#include "cvcameramanager.h"

CVCameraManager::CVCameraManager(QObject *parent) : QObject(parent)
{
   graber = new Graber(&capture);
   retriever = new Retriever(&capture);

   grabersThread = new QThread(0);
   grabersThread->start();
   graber->moveToThread(grabersThread);
   graber->locker = &locker;
   grabersThread->setPriority(QThread::HighestPriority);

   retrieverThread = new QThread(0);
   retrieverThread->start();
   retriever->moveToThread(retrieverThread);
   retriever->locker = &locker;


   retrievTimer = new QTimer(this);
   retrievTimer->setTimerType(Qt::PreciseTimer);

   connect(this,SIGNAL(connectingToCamera()),retriever,SLOT(makeOpen()),Qt::QueuedConnection);
   connect(this,SIGNAL(disconnectingFromCamera()),retriever,SLOT(makeClose()),Qt::QueuedConnection);
   connect(retriever,SIGNAL(openStatus(bool)),this,SLOT(handleConnectionStatus(bool)),Qt::QueuedConnection);

   qRegisterMetaType<Mat>("Mat");
   connect(this,SIGNAL(makeGrab()),graber,SLOT(makeGrab()),Qt::QueuedConnection);
   connect(retrievTimer,SIGNAL(timeout()),retriever,SLOT(makeRetriev()),Qt::QueuedConnection);
   connect(retriever,SIGNAL(newFrame(Mat)),this,SIGNAL(newFrame(Mat)),Qt::QueuedConnection);
   connect(retriever,SIGNAL(emptyFrame()),this,SIGNAL(emptyFrame()),Qt::QueuedConnection);

}

CVCameraManager::~CVCameraManager()
{
    grabersThread->quit();
    grabersThread->deleteLater();

    retrieverThread->quit();
    retrieverThread->deleteLater();

    graber->deleteLater();
    retriever->deleteLater();
    if(capture.isOpened()){
        capture.release();
    }
}

int CVCameraManager::getFps()
{
    if(capture.isOpened()){
        return capture.get(CAP_PROP_FPS);
    }
    return -1;
}

bool CVCameraManager::isOpend()
{
    return capture.isOpened();
}

Mat CVCameraManager::takeSingleFrame()
{
    if(capture.isOpened()){
        Mat frame;
        capture.read(frame);
        return frame;
    }
}

void CVCameraManager::setConnectionString(QString connectionString)
{
    retriever->connectionString = connectionString;
}

void CVCameraManager::connectToCamera()
{
    emit connectingToCamera();
}

void CVCameraManager::disconnectFromCamera()
{
    graber->isGrabbing = false;
    retrievTimer->stop();

    emit disconnectingFromCamera();
}

void CVCameraManager::startCapture()
{
    if(capture.isOpened()){
        graber->isGrabbing = true;
        emit makeGrab();
        retrievTimer->start();
    }
}

void CVCameraManager::stopCapture()
{
    graber->isGrabbing = false;
    retrievTimer->stop();
}

void CVCameraManager::handleConnectionStatus(bool status)
{
    if(status){
        int fps = getFps();
        if(fps > 100){
            emit error("Fps is too high:" + QString::number(fps));
            graber->interval = 30;
        }else if(fps <= 0){
            emit error("Wrong fps:" + QString::number(fps));
            graber->interval = 30;
        }else{
            graber->interval = qRound(1000.0/fps);
        }
        retrievTimer->setInterval(graber->interval);
        retrievTimer->stop();
        emit connected();
    }else{
        retrievTimer->stop();
        emit disconnected();
    }
}


Graber::Graber(VideoCapture *cap, QObject *parent) : QObject(parent),cap(cap)
{
    lastTime = QTime::currentTime();
    connect(this,SIGNAL(grabFinished()),this,SLOT(makeGrab()),Qt::QueuedConnection);
}

Graber::~Graber()
{

}

void Graber::makeGrab()
{
    if(isGrabbing){
        if(cap->isOpened()){

            QTime currentTime = QTime::currentTime();
            int duration = lastTime.msecsTo(currentTime);
            lastTime = currentTime;
            if(duration < interval){
                this->thread()->msleep(interval-duration);
            }

            locker->lock();
            cap->grab();
            locker->unlock();

            emit grabFinished();
        }
    }
}

Retriever::Retriever(VideoCapture *cap, QObject *parent) : QObject(parent),cap(cap)
{

}

Retriever::~Retriever()
{

}

void Retriever::makeRetriev()
{
    if(cap->isOpened()){
        Mat frame;
        cap->retrieve(frame);
        if(!frame.empty()){
            emptyFramesCounter = 0;
            emit newFrame(frame);
        }else{
            emit emptyFrame();
            emptyFramesCounter++;
            if(emptyFramesCounter >= 10){
                makeClose();
                emptyFramesCounter = 0;
            }
        }
    }
}


void Retriever::makeOpen()
{
    if(!cap->isOpened()){
        locker->lock();
        bool status = cap->open(connectionString.toStdString());
        locker->unlock();
        emit openStatus(status);
    }
}

void Retriever::makeClose()
{
    if(cap->isOpened()){
        locker->lock();
        cap->release();
        locker->unlock();
        emit openStatus(cap->isOpened());
    }
}
