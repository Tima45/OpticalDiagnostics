#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initPlot();

    QVector<double> xProfile;
    QVector<double> yProfile;
    for(int i = 0; i < 256; i++){
        xProfile.append(0.4*exp(-powf(i-158,2)/100.0));
        yProfile.append(0.4*exp(-powf(i-150,2)/100.0));
    }


    updateProfiles(xProfile,yProfile,(60.0/256.0),-30,(60.0/256.0),-30);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initPlot()
{

    ui->plot->setInteraction(QCP::iRangeDrag, true);
    ui->plot->setInteraction(QCP::iRangeZoom, true);
    ui->plot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    ui->plot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);

    ui->plot->addLayer("low");
    ui->plot->addLayer("mid");
    ui->plot->addLayer("top");

    ui->plot->yAxis2->setVisible(true);
    ui->plot->xAxis2->setVisible(true);
    ui->plot->yAxis2->setLabel("Яркость");
    ui->plot->xAxis2->setLabel("Яркость");
    ui->plot->yAxis->setLabel("Y (мм)");
    ui->plot->xAxis->setLabel("X (мм)");

    xProfileGraph = ui->plot->addGraph(ui->plot->xAxis,ui->plot->yAxis2);

    yProfileGraph = ui->plot->addGraph(ui->plot->yAxis,ui->plot->xAxis2);

    xProfileGraph->setLayer("mid");
    yProfileGraph->setLayer("mid");
    xProfileGraph->setPen(QPen(QColor(0,0,255,128)));
    yProfileGraph->setPen(QPen(QColor(0,0,255,128)));

    xSmoothProfileGraph = ui->plot->addGraph(ui->plot->xAxis,ui->plot->yAxis2);
    ySmoothProfileGraph = ui->plot->addGraph(ui->plot->yAxis,ui->plot->xAxis2);
    xSmoothProfileGraph->setLayer("mid");
    ySmoothProfileGraph->setLayer("mid");
    xSmoothProfileGraph->pen().setWidth(2);
    ySmoothProfileGraph->pen().setWidth(2);

    xyProfileMap = new QCPColorMap(ui->plot->xAxis,ui->plot->yAxis);
    xyProfileMap->setLayer("low");
    xyProfileMap->data()->setRange(QCPRange(-30,-30),QCPRange(-30,-30));
    ui->plot->xAxis->setRange(-30,30);
    ui->plot->yAxis->setRange(-30,30);


    colorScale = new QCPColorScale(ui->plot);
    colorScale->setType(QCPAxis::atRight);
    g.clearColorStops();
    g.setColorInterpolation(QCPColorGradient::ColorInterpolation::ciRGB);
    g.setColorStopAt(0,QColor(255,255,255));
    g.setColorStopAt(1,QColor(0,0,255));
    g.setLevelCount(50);
    colorScale->setGradient(g);
    colorScale->setDataRange(QCPRange(0,1));
    ui->plot->plotLayout()->addElement(0,1,colorScale);
    xyProfileMap->setColorScale(colorScale);


    widthLine = new QCPItemLine(ui->plot);
    QPen p;
    p.setStyle(Qt::DashLine);
    p.setColor(QColor(0,64,128));
    widthLine->setPen(p);
    widthLine->setLayer("top");
    heightLine = new QCPItemLine(ui->plot);
    heightLine->setPen(p);
    heightLine->setLayer("top");



    ui->plot->moveLayer(ui->plot->layer("grid"),ui->plot->layer("top"));
}

void MainWindow::setEnabledAll(bool value)
{
    ui->xCameraIpEdit->setEnabled(value);
    ui->xCameraUsernameEdit->setEnabled(value);
    ui->xCameraPassword->setEnabled(value);
    ui->xCameraTestButton->setEnabled(value);
    ui->xCameraCalibrationButton->setEnabled(value);
    ui->yCameraIpEdit->setEnabled(value);
    ui->yCameraUsernameEdit->setEnabled(value);
    ui->yCameraPassword->setEnabled(value);
    ui->yCameraTestButton->setEnabled(value);
    ui->yCameraCalibrationButton->setEnabled(value);
}

void MainWindow::updateProfiles(const QVector<double> &xProfile, const QVector<double> &yProfile, double xScale,double xDelta, double yScale,double yDelta)
{
    QVector<double> xLengthKeys;
    QVector<double> yLengthKeys;

    for(int i = 0; i < xProfile.count(); i++){
        xLengthKeys.append(xDelta + xScale*i);
    }
    for(int i = 0; i < yProfile.count(); i++){
        yLengthKeys.append(yDelta + yScale*i);
    }
    //-------------------------------------------------
    WaveletSpectrum *dwt = new WaveletSpectrum(xProfile,WaveletSpectrum::BSPLINE_309);
    dwt->levelFilter(dwt->getLevels()-1,0);
    dwt->levelFilter(dwt->getLevels()-2,0);
    QVector<double> xSmoothProfile;
    xSmoothProfile = dwt->toData();
    dwt->deleteLater();
    dwt = new WaveletSpectrum(yProfile,WaveletSpectrum::BSPLINE_309);
    dwt->levelFilter(dwt->getLevels()-1,0);
    dwt->levelFilter(dwt->getLevels()-2,0);
    QVector<double> ySmoothProfile;
    ySmoothProfile = dwt->toData();
    dwt->deleteLater();
    //--------------------------------------------
    double xMax = 0;
    int xMaxId = 0;

    double yMax = 0;
    int yMaxId = 0;

    for(int i = 0; i < xSmoothProfile.count(); i++){
        if(xSmoothProfile.at(i) > xMax){
            xMax = xSmoothProfile.at(i);
            xMaxId = i;
        }
    }
    for(int i = 0; i < ySmoothProfile.count(); i++){
        if(ySmoothProfile.at(i) > yMax){
            yMax = ySmoothProfile.at(i);
            yMaxId = i;
        }
    }
    //--------------------------------------------
    QVector<int> xWidthKeys;
    QVector<int> yHeightKeys;
    for(int i = 1; i < xSmoothProfile.count()-1; i++){
        if(xSmoothProfile.at(i-1) < xMax/5.0 && xSmoothProfile.at(i+1) > xMax/5.0){
            xWidthKeys.append(i);
            break;
        }
    }
    for(int i = 1; i < xSmoothProfile.count()-1; i++){
        if(xSmoothProfile.at(i-1) > xMax/5.0 && xSmoothProfile.at(i+1) < xMax/5.0){
            xWidthKeys.append(i);
            break;
        }
    }

    for(int i = 1; i < ySmoothProfile.count()-1; i++){
        if(ySmoothProfile.at(i-1) < yMax/5.0 && ySmoothProfile.at(i+1) > yMax/5.0){
            yHeightKeys.append(i);
            break;
        }
    }
    for(int i = 1; i < ySmoothProfile.count()-1; i++){
        if(ySmoothProfile.at(i-1) > yMax/5.0 && ySmoothProfile.at(i+1) < yMax/5.0){
            yHeightKeys.append(i);
            break;
        }
    }
    if(xWidthKeys.count() == 2 && yHeightKeys.count() == 2){
        if(xWidthKeys.at(0) < xWidthKeys.at(1) && yHeightKeys.at(0) < yHeightKeys.at(1)){
            widthLine->start->setCoords(xLengthKeys.at(xWidthKeys[0]),yLengthKeys.at(yMaxId));
            widthLine->end->setCoords(xLengthKeys.at(xWidthKeys[1]),yLengthKeys.at(yMaxId));

            heightLine->start->setCoords(xLengthKeys.at(xMaxId),yLengthKeys.at(yHeightKeys[0]));
            heightLine->end->setCoords(xLengthKeys.at(xMaxId),yLengthKeys.at(yHeightKeys[1]));
        }
    }

    //--------------------------------------------
    xProfileGraph->clearData();
    xProfileGraph->setData(xLengthKeys,xProfile);
    yProfileGraph->clearData();
    yProfileGraph->setData(yLengthKeys,yProfile);

    xSmoothProfileGraph->clearData();
    xSmoothProfileGraph->setData(xLengthKeys,xSmoothProfile);
    ySmoothProfileGraph->clearData();
    ySmoothProfileGraph->setData(xLengthKeys,xSmoothProfile);
    //--------------------------------------------

    xyProfileMap->data()->clear();
    int minCount = xProfile.count() < yProfile.count() ? xProfile.count() : yProfile.count();
    xyProfileMap->data()->setSize(minCount,minCount);
    xyProfileMap->data()->setRange(QCPRange(xDelta,xProfile.count()*xScale+xDelta),QCPRange(yDelta,yProfile.count()*yScale+yDelta));
    for(int y = 0; y < minCount; y++){
        for(int x = 0; x < minCount; x++){
            xyProfileMap->data()->setCell(x,y,xSmoothProfile[x]*ySmoothProfile[y]);
        }
    }

    //xyProfileMap->rescaleDataRange();
    ui->plot->xAxis->setRange(QCPRange(xDelta,xProfile.count()*xScale+xDelta));
    ui->plot->yAxis->setRange(QCPRange(yDelta,yProfile.count()*yScale+yDelta));

    ui->plot->xAxis2->setRange(QCPRange(0,1));
    ui->plot->yAxis2->setRange(QCPRange(0,1));

    ui->plot->replot();
}

void MainWindow::on_startStopButton_clicked()
{
    if(!isRunning){
        isRunning = true;
        ui->startStopButton->setText("Стоп");
        setEnabledAll(false);


    }else{
        isRunning = false;
        ui->startStopButton->setText("Старт");
        setEnabledAll(true);
    }
}

void MainWindow::on_xCameraTestButton_clicked()
{
    /*
    QString s = QString("rtsp://%1:%2@%3:554").arg(ui->xCameraUsernameEdit->text()).arg(ui->xCameraPassword->text()).arg(ui->xCameraIpEdit->text());
    qDebug() << s;

    if(!xVideo.isOpened()){
        xVideo.open(s.toStdString());
        xVideo >> ximage;
        if(ximage.data){
            //imshow("testX",ximage);
            ShowPicForm *f = new ShowPicForm(ximage);
            f->setAttribute(Qt::WA_DeleteOnClose);
            f->show();
        }else{
            QMessageBox::critical(this,":(",":(");
        }
    }else{
        xVideo >> ximage;
        if(ximage.data){
            imshow("testX",ximage);
        }else{
            QMessageBox::critical(this,":(",":(");
        }
    }
*/

    Mat pic = imread("D:/1222.png",IMREAD_ANYCOLOR);
    ShowPicForm *f = new ShowPicForm(pic);
    f->setAttribute(Qt::WA_DeleteOnClose);
    f->show();

}

void MainWindow::on_yCameraTestButton_clicked()
{

    QString s = QString("rtsp://%1:%2@%3:554").arg(ui->yCameraUsernameEdit->text()).arg(ui->yCameraPassword->text()).arg(ui->yCameraIpEdit->text());
    qDebug() << s;
    if(!yVideo.isOpened()){
        yVideo.open(s.toStdString());
        yVideo >> yimage;
        if(yimage.data){
            imshow("testY",yimage);
        }else{
            QMessageBox::critical(this,":(",":(");
        }
    }else{
        yVideo >> yimage;
        if(yimage.data){
            imshow("testY",yimage);
        }else{
            QMessageBox::critical(this,":(",":(");
        }
    }
}
