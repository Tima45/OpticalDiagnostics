#ifndef SHOWPICFORM_H
#define SHOWPICFORM_H

#include <QWidget>
#include <QDebug>
#include <QPicture>
#include <QPixmap>
#include <cv.hpp>

using namespace cv;

namespace Ui {
class ShowPicForm;
}

class ShowPicForm : public QWidget
{
    Q_OBJECT

public:
    explicit ShowPicForm(Mat pic,QWidget *parent = 0);
    ~ShowPicForm();

private:
    Ui::ShowPicForm *ui;
};

#endif // SHOWPICFORM_H
