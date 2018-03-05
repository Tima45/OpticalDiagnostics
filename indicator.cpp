#include "indicator.h"

Indicator::Indicator(QWidget *parent, Qt::WindowFlags f) : QLabel(parent)
{
    Q_UNUSED(f);
    onPix = QPixmap(":/picures/indicatorOn.png");
    onPix2 = QPixmap(":/picures/indicatorOn2.png");
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

void Indicator::blink()
{
    if(value){
        if(rand()%2 == 0){
            this->setPixmap(onPix);
        }else{
            this->setPixmap(onPix2);
        }
    }
}
