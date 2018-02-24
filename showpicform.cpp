#include "showpicform.h"
#include "ui_showpicform.h"

ShowPicForm::ShowPicForm(Mat pic, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowPicForm)
{
    ui->setupUi(this);
    QImage p(QSize(pic.cols,pic.rows),QImage::Format_RGB16);

    //qDebug() << pic.cols << pic.rows;


    for(int y = 0; y < pic.rows; y++){
        for(int x = 0; x < pic.cols; x++) {
            uchar b = pic.at<Vec3b>(y,x)[0];
            uchar g = pic.at<Vec3b>(y,x)[1];
            uchar r = pic.at<Vec3b>(y,x)[2];
            QColor color(r,g,b);
            p.setPixel(x,y,color.rgb());
        }
    }


    ui->label->setPixmap(QPixmap::fromImage(p));
    ui->label->setScaledContents(true);

}

ShowPicForm::~ShowPicForm()
{
    delete ui;
}
