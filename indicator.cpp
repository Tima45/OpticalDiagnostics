#include "indicator.h"

Indicator::Indicator(QWidget *parent, Qt::WindowFlags f) : QLabel(parent)
{
    Q_UNUSED(f);
    onPix = QPixmap(":/picures/indicatorOn.png");
    offPix = QPixmap(":/picures/indicatorOff.png");
    this->setPixmap(offPix);
    this->setToolTip("Отключено");
    this->setScaledContents(true);
}

bool Indicator::isTernedOn()
{
    return value;
}

void Indicator::setState(bool value)
{
    this->value = value;
    if(value){
        this->setPixmap(onPix);
        this->setToolTip("Подключено");
    }else{
        this->setPixmap(offPix);
        this->setToolTip("Отключено");
    }
}
