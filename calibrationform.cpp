#include "calibrationform.h"
#include "ui_calibrationform.h"

CalibrationForm::CalibrationForm(QString type, Mat pic, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CalibrationForm), type(type)
{
    ui->setupUi(this);

    sourcePic = QImage (QSize(pic.cols,pic.rows),QImage::Format_RGB16);

    for(int y = 0; y < pic.rows; y++){
        for(int x = 0; x < pic.cols; x++) {
            uchar b = pic.at<Vec3b>(y,x)[0];
            uchar g = pic.at<Vec3b>(y,x)[1];
            uchar r = pic.at<Vec3b>(y,x)[2];
            QColor color(r,g,b);
            sourcePic.setPixel(x,y,color.rgb());
        }
    }

    ui->setStartAxisButton->setText(ui->setStartAxisButton->text()+type);
    ui->setStopAxisButton->setText(ui->setStopAxisButton->text()+type);
    inUsePic = QImage(sourcePic);
    ui->label->setPixmap(QPixmap::fromImage(inUsePic));
    ui->label->setScaledContents(true);

    connect(ui->label,SIGNAL(clicked(QMouseEvent*)),this,SLOT(labelClicked(QMouseEvent*)));
}

CalibrationForm::~CalibrationForm()
{
    delete ui;
}

void CalibrationForm::setEnambledAll(bool value)
{
    ui->setStartAxisButton->setEnabled(value);
    ui->setStopAxisButton->setEnabled(value);
    ui->startInMmEdit->setEnabled(value);
    ui->stopInMmEdit->setEnabled(value);
    ui->saveButton->setEnabled(value);
    ui->clearButton->setEnabled(value);
}

void CalibrationForm::on_setStartAxisButton_clicked()
{
    setEnambledAll(false);
    settingStart = true;
}

void CalibrationForm::on_setStopAxisButton_clicked()
{
    setEnambledAll(false);
    settingStop = true;
}

void CalibrationForm::labelClicked(QMouseEvent* event)
{
    if(settingStart){
        if(type == "X"){
            startPixel = event->x();
            otherPixel = event->y();
            drawVericalLine(startPixel);
            ui->startInPixLabel->setText(QString::number(startPixel));
        }
        if(type == "Y"){
            startPixel = event->y();
            otherPixel = event->x();
            drawHorisontalLine(startPixel);
            ui->startInPixLabel->setText(QString::number(startPixel));
        }
        settingStart = false;
    }
    if(settingStop){
        if(type == "X"){
            stopPixel = event->x();
            drawVericalLine(stopPixel);
            ui->stopInPixLabel->setText(QString::number(stopPixel));
        }
        if(type == "Y"){
            stopPixel = event->y();
            drawHorisontalLine(stopPixel);
            ui->stopInPixLabel->setText(QString::number(stopPixel));
        }
        settingStop = false;
    }
    if(startPixel != 0 && otherPixel != 0 && stopPixel != 0){
        inUsePic = QImage(sourcePic);
        if(type == "X"){
            drawVericalLine(startPixel);
            drawVericalLine(stopPixel);
            QPainter painter;
            painter.begin(&inUsePic);
            painter.setPen(QPen(QColor(Qt::red)));
            painter.drawLine(startPixel,otherPixel,stopPixel,otherPixel);
            painter.end();
            ui->label->setPixmap(QPixmap::fromImage(inUsePic));
        }
        if(type == "Y"){
            drawHorisontalLine(startPixel);
            drawHorisontalLine(stopPixel);
            QPainter painter;
            painter.begin(&inUsePic);
            painter.setPen(QPen(QColor(Qt::red)));
            painter.drawLine(otherPixel,startPixel,otherPixel,stopPixel);
            painter.end();
            ui->label->setPixmap(QPixmap::fromImage(inUsePic));
        }
    }
    setEnambledAll(true);
    //qDebug() << event->x() << event->y();
}

void CalibrationForm::drawVericalLine(int pos)
{
    QPainter painter;
    painter.begin(&inUsePic);
    painter.setPen(QPen(QColor(Qt::red)));
    painter.drawLine(pos,0,pos,inUsePic.height());
    painter.end();
    ui->label->setPixmap(QPixmap::fromImage(inUsePic));
}

void CalibrationForm::drawHorisontalLine(int pos)
{
    QPainter painter;
    painter.begin(&inUsePic);
    painter.setPen(QPen(QColor(Qt::red)));
    painter.drawLine(0,pos,inUsePic.width(),pos);
    painter.end();
    ui->label->setPixmap(QPixmap::fromImage(inUsePic));
}

void CalibrationForm::on_clearButton_clicked()
{
    inUsePic = QImage(sourcePic);
    startPixel = 0;
    otherPixel = 0;
    stopPixel = 0;
    ui->startInPixLabel->setText("0");
    ui->stopInPixLabel->setText("0");
    ui->label->setPixmap(QPixmap::fromImage(inUsePic));
}

void CalibrationForm::on_saveButton_clicked()
{
    if(ui->stopInMmEdit->value() * ui->startInMmEdit->value() < 0){
        if(startPixel != 0 && otherPixel != 0 && stopPixel != 0){
            double scale = (ui->stopInMmEdit->value() - ui->startInMmEdit->value())/(stopPixel - startPixel);
            double delta = ui->startInMmEdit->value();
            emit saveCalibration(type,scale,delta,startPixel,stopPixel,otherPixel);
            this->close();
        }else{
            QMessageBox::critical(this,"Ошибка","Установите начало и конец оси.");
        }
    }else{
        QMessageBox::critical(this,"Ошибка","Начало и конец в мм должны проходить через 0.");
    }
}
