#ifndef INDICATOR_H
#define INDICATOR_H

#include <QWidget>
#include <QLabel>
#include <QPixmap>

class Indicator : public QLabel
{
    Q_OBJECT
public:
    explicit Indicator(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());

    bool isTernedOn();
private:
    QPixmap onPix;
    QPixmap offPix;
    bool value;
signals:

public slots:
    void setState(bool value);
};



#endif // INDICATOR_H
