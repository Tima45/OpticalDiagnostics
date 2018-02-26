#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QSettings>
#include <QTimer>
#include <waveletspectrum.h>
#include <QMessageBox>
#include <QThread>
#include "plot/qcustomplot.h"
#include "showpicform.h"
#include "calibrationform.h"
#include "capturemanager.h"
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
    QTimer *frameGrapTimer;
    //===========================================================
    QString settingsName = "calibration.ini";
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
    void loadSettings();
    void saveSettings();
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
    //===========================================================
    void showPic(Mat &pic);
    void calibrate(QString type,Mat &pic);

    CaptureManager *xCapure;
    CaptureManager *yCapure;

    QThread *xThread;
    QThread *yThread;

    Mat ximage;
    Mat yimage;

    //===========================================================
    void initPlot();
    QCPGraph *xProfileGraph;
    QCPGraph *yProfileGraph;
    QCPGraph *xSmoothProfileGraph;
    QCPGraph *ySmoothProfileGraph;
    QCPColorMap *xyProfileMap;
    QCPColorScale *colorScale;
    QCPColorGradient g;
    QCPItemLine *widthLine;
    QCPItemLine *heightLine;
    //===========================================================
    void setEnabledXCamer(bool value);
    void setEnabledYCamer(bool value);

    void updateProfiles(QVector<double> &xProfile, QVector<double> &yProfile);

    bool xReady = false;
    bool yReady = false;

signals:
    void openXCapture();
    void openYCapture();
private slots:
    void handleLostConnection(QString name);
    void handleOpenResult(QString name, bool status);
    void saveCalibration(QString type,double scale, double delta,int start, int stop, int otherPixel);
    void on_startStopButton_clicked();

    void on_xCameraCalibrationButton_clicked();

    void on_yCameraCalibrationButton_clicked();

    void handleFrame(QString name, Mat newPic);

    void on_xCameraConnectButton_clicked();

    void on_yCameraConnectButton_clicked();

private:
    Ui::MainWindow *ui;
    QVector<double> xProfile;
    QVector<double> yProfile;
    QVector<double> xLengthKeys;
    QVector<double> yLengthKeys;
};

#endif // MAINWINDOW_H

