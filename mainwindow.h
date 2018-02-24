#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <waveletspectrum.h>
#include <QMessageBox>
#include "plot/qcustomplot.h"
#include "showpicform.h"
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

    VideoCapture xVideo;
    VideoCapture yVideo;
    Mat ximage;
    Mat yimage;

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

    void setEnabledAll(bool value);



    void updateProfiles(const QVector<double> &xProfile, const QVector<double> &yProfile, double xScale, double xDelta, double yScale, double yDelta);

private slots:
    void on_startStopButton_clicked();

    void on_xCameraTestButton_clicked();

    void on_yCameraTestButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
