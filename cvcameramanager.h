#ifndef CVCAPTUREMANAGER_H
#define CVCAPTUREMANAGER_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <cv.hpp>

using namespace cv;

class Graber : public QObject
{
    Q_OBJECT
public:
    explicit Graber(VideoCapture *cap,QObject *parent = 0);
    ~Graber();
    VideoCapture *cap;

    bool isGrabbing = false;
    int interval = 30;
    QTime lastTime;
    QMutex *locker;
signals:
    void grabFinished();
public slots:
    void makeGrab();


};

class Retriever : public QObject
{
    Q_OBJECT
public:
    explicit Retriever(VideoCapture *cap,QObject *parent = 0);
    ~Retriever();
    VideoCapture *cap;
    QString connectionString;
    QMutex *locker;
private:
    int emptyFramesCounter = 0;
signals:
    void newFrame(Mat frame);
    void emptyFrame();
    void openStatus(bool);
public slots:
    void makeRetriev();
    void makeOpen();
    void makeClose();
};

class CVCameraManager : public QObject
{
    Q_OBJECT
public:
    explicit CVCameraManager(QObject *parent = 0);
    ~CVCameraManager();
    VideoCapture capture;

    int getFps();
    QMutex locker;
    bool isOpend();
    Mat takeSingleFrame();
signals:
    void newFrame(Mat frame);
    void emptyFrame();
    void error(QString text);
    void connected();
    void disconnected();
    void connectingToCamera();
    void disconnectingFromCamera();

//private signals:
    void makeGrab();
    void grabDuration(int);
    void retrieveDuration(int);
public slots:
    void setConnectionString(QString connectionString);
    void connectToCamera();
    void disconnectFromCamera();
    void startCapture();
    void stopCapture();

private slots:
    void handleConnectionStatus(bool status);
private:
    Graber *graber;
    Retriever *retriever;

    QThread *grabersThread;
    QThread *retrieverThread;

    QTimer *retrievTimer;
};

#endif // CVCAPTUREMANAGER_H
