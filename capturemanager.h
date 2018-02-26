#ifndef CAPTUREMANAGER_H
#define CAPTUREMANAGER_H

#include <QObject>
#include <QDebug>
#include <QReadWriteLock>
#include <QTimer>
#include <QTime>
#include <cv.hpp>

using namespace cv;

class CaptureManager : public QObject
{
    Q_OBJECT
public:
    explicit CaptureManager(unsigned int grabInterval,QObject *parent = 0);
    QString name;
    ~CaptureManager();
    QString connectionString;
    VideoCapture cap;
    QReadWriteLock lock;
    QTimer *grabTimer;

    bool isOpend();
    Mat takeSingleFrame();
signals:
    void newFrame(QString name,Mat frame);
    void losedConnection(QString name);
    void openResult(QString name,bool result);
public slots:
    void getFrame();
    void open();
    void release();
    void setConnectionString(QString connectionString);
private slots:
    void grabFrame();
};

#endif // CAPTUREMANAGER_H
