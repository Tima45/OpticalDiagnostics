#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QSettings>
#include <QTimer>
#include <QTime>
#include <waveletspectrum.h>
#include <QMessageBox>
#include <QThread>
#include "plot/qcustomplot.h"
#include "showpicform.h"
#include "calibrationform.h"
#include "cvcameramanager.h"
#include "tracermanager.h"
#include "databasemanager.h"
#include <cv.hpp>

using namespace cv;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool isRunning = false;
    //QTimer *frameGrapTimer;
    //===========================================================
    QString settingsName = "settings.ini";
    QString xScaleSettingsName = "xScale";
    QString xDeltaSettingsName = "xDelta";
    QString yPositionForXAxisSettingsName = "yPositionForXAxis";
    QString xStartPositionSettingsName = "xStartPosition";
    QString xStopPositionSettingsName = "xStopPosition";

    QString yScaleSettingsName = "yScale";
    QString yDeltaSettingsName = "yDelta";
    QString xPositionForYAxisSettingsName = "xPositionForYAxis";
    QString yStartPositionSettingsName = "yStartPosition";
    QString yStopPositionSettingsName = "yStopPosition";



    QString xCameraIpSettingsName = "xCameraIp";
    QString yCameraIpSettingsName = "yCameraIp";
    QString xUserNameSettingsName = "xUserName";
    QString yUserNameSettingsName = "yUserName";

    QString dataBaseFlushSecSettingsName = "dataBaseFlushSec";

    QString frameRateMsSettingsName = "framerateMs";

    QString dataBaseIpSettingsName = "dataBaseIp";

    void loadSettings();
    void saveSettings();

    QString dataBaseIp = "192.168.0.220";
    int frameRateMs = 30;
    int dataBaseFlushSec = 60;
    double xScale = 1;
    double xDelta = 0;
    int yPositionForXAxis = 0;
    int xStartPosition = 0;
    int xStopPosition = 0;

    double yScale = 1;
    double yDelta = 0;
    int xPositionForYAxis = 0;
    int yStartPosition = 0;
    int yStopPosition = 0;

    QTime lastUpdate;

    //===========================================================
    void showPic(Mat &pic);
    void calibrate(Qt::Orientation type, Mat &pic);

    CVCameraManager *xCamera;
    CVCameraManager *yCamera;

    /*
    CaptureManager *xCapure;
    CaptureManager *yCapure;

    QThread *xThread;
    QThread *yThread;
    */


    Mat ximage;
    Mat yimage;

    //===========================================================
    void initPlot();
    QCPGraph *xProfileGraph;
    QCPGraph *yProfileGraph;

    QCPGraph *xSmoothProfileGraph;
    QCPGraph *ySmoothProfileGraph;

    QCPGraph *xSmoothSmoothProfileGraph;
    QCPGraph *ySmoothSmoothProfileGraph;
    QCPColorMap *xyProfileMap;
    QCPColorScale *colorScale;
    QCPColorGradient g;
    QCPItemLine *widthLine;
    QCPItemLine *heightLine;
    QCPItemTracer *centerTracer;

    TracerManager *manager;

    DataBaseManager *dataBaseManager;
    QThread *dataBaseThread;

    QPen selectedPen;
    QPen notSelectedPen;
    //===========================================================
    void setEnabledXCamer(bool value);
    void setEnabledYCamer(bool value);

    void updateProfiles(QVector<double> &xProfile, QVector<double> &yProfile);

    bool xReady = false;
    bool yReady = false;

private slots:
    void checkXConnection();
    void checkYConnection();

    void saveCalibration(Qt::Orientation type,double scale, double delta,int start, int stop, int otherPixel);
    void on_startStopButton_clicked();

    void on_xCameraCalibrationButton_clicked();

    void on_yCameraCalibrationButton_clicked();

    void handleXFrame(Mat newPic);
    void handleYFrame(Mat newPic);
    void handleBothFrames();

    void on_xCameraConnectButton_clicked();

    void on_yCameraConnectButton_clicked();


    void on_hideShowButton_clicked();


    void on_noSmoothRadioButton_clicked(bool checked);

    void on_smoothRadioButton_clicked(bool checked);

    void on_smoothSmoothRadioButton_clicked(bool checked);

    void on_hideExtraInformationButton_clicked();

private:
    Ui::MainWindow *ui;

    QVector<int> xWidthKeys;
    QVector<int> yHeightKeys;


    QVector<double> xProfile;
    QVector<double> yProfile;

    QVector<double> xSmoothProfile;
    QVector<double> ySmoothProfile;

    QVector<double> xSmoothSmoothProfile;
    QVector<double> ySmoothSmoothProfile;


    QVector<double> xLengthKeys;
    QVector<double> yLengthKeys;
};

#endif // MAINWINDOW_H

