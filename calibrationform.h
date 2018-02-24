#ifndef CALIBRATIONFORM_H
#define CALIBRATIONFORM_H

#include <QWidget>
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>
#include "clickablelabel.h"
#include <cv.hpp>
using namespace cv;

namespace Ui {
class CalibrationForm;
}

class CalibrationForm : public QWidget
{
    Q_OBJECT

public:
    explicit CalibrationForm(QString type,Mat pic,QWidget *parent = 0);
    ~CalibrationForm();
    void setEnambledAll(bool value);
    bool settingStart = false;
    bool settingStop = false;

    QImage sourcePic;
    QImage inUsePic;
    QString type;


    int startPixel = 0;
    int otherPixel = 0;
    int stopPixel = 0;
signals:
    void saveCalibration(QString type,double scale, double delta,int start, int stop, int otherPixel);
private slots:
    void on_setStartAxisButton_clicked();

    void on_setStopAxisButton_clicked();

    void labelClicked(QMouseEvent *event);

    void on_clearButton_clicked();

    void on_saveButton_clicked();

private:
    Ui::CalibrationForm *ui;

    void drawVericalLine(int pos);
    void drawHorisontalLine(int pos);
};

#endif // CALIBRATIONFORM_H
