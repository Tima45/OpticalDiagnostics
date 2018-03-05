#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadSettings();
    initPlot();

    xCapure = new CaptureManager(30);
    xCapure->type = Qt::Horizontal;
    yCapure = new CaptureManager(30);
    yCapure->type = Qt::Vertical;

    xThread = new QThread(this);
    yThread = new QThread(this);

    xCapure->moveToThread(xThread);
    xThread->start();

    yCapure->moveToThread(yThread);
    yThread->start();

    frameGrapTimer = new QTimer(this);
    frameGrapTimer->setInterval(30);
    connect(frameGrapTimer,SIGNAL(timeout()),xCapure,SLOT(getFrame()),Qt::QueuedConnection);
    connect(frameGrapTimer,SIGNAL(timeout()),yCapure,SLOT(getFrame()),Qt::QueuedConnection);

    qRegisterMetaType<Mat>("Mat");
    qRegisterMetaType<Qt::Orientation>("Qt::Orientation");


    connect(this,SIGNAL(openXCapture()),xCapure,SLOT(open()));
    connect(this,SIGNAL(openYCapture()),yCapure,SLOT(open()));
    connect(xCapure,SIGNAL(openResult(Qt::Orientation,bool)),this,SLOT(handleOpenResult(Qt::Orientation,bool)));
    connect(yCapure,SIGNAL(openResult(Qt::Orientation,bool)),this,SLOT(handleOpenResult(Qt::Orientation,bool)));

    connect(xCapure,SIGNAL(newFrame(Qt::Orientation,Mat)),this,SLOT(handleFrame(Qt::Orientation,Mat)),Qt::QueuedConnection);
    connect(xCapure,SIGNAL(losedConnection(Qt::Orientation)),this,SLOT(handleLostConnection(Qt::Orientation)),Qt::QueuedConnection);
    connect(yCapure,SIGNAL(newFrame(Qt::Orientation,Mat)),this,SLOT(handleFrame(Qt::Orientation,Mat)),Qt::QueuedConnection);
    connect(yCapure,SIGNAL(losedConnection(Qt::Orientation)),this,SLOT(handleLostConnection(Qt::Orientation)),Qt::QueuedConnection);


}

MainWindow::~MainWindow()
{
    saveSettings();

    xThread->terminate();
    yThread->terminate();

    xCapure->deleteLater();
    yCapure->deleteLater();


    delete ui;
}

void MainWindow::loadSettings()
{
    if(QFileInfo(settingsName).isFile()){
        QSettings settings(settingsName,QSettings::IniFormat);
        xScale = settings.value(xScaleSettingsName).toDouble();
        yScale = settings.value(yScaleSettingsName).toDouble();
        xPositionForYAxis = settings.value(xPositionForYAxisSettingsName).toInt();
        yPositionForXAxis = settings.value(yPositionForXAxisSettingsName).toInt();
        xStartPosition = settings.value(xStartPositionSettingsName).toInt();
        yStartPosition = settings.value(yStartPositionSettingsName).toInt();
        xStopPosition = settings.value(xStopPositionSettingsName).toInt();
        yStopPosition = settings.value(yStopPositionSettingsName).toInt();
        xDelta = settings.value(xDeltaSettingsName).toDouble();
        yDelta = settings.value(yDeltaSettingsName).toDouble();

        xProfile.resize(abs(xStopPosition-xStartPosition));
        yProfile.resize(abs(yStopPosition-yStartPosition));
        for(int i = 0; i < xProfile.count(); i++){
            xLengthKeys.append(xDelta + fabs(xScale)*i);
        }
        for(int i = 0; i < yProfile.count(); i++){
            yLengthKeys.append(yDelta + fabs(yScale)*i);
        }

        ui->xCameraIpEdit->setText(settings.value(xCameraIpSettingsName).toString());
        ui->yCameraIpEdit->setText(settings.value(yCameraIpSettingsName).toString());

        ui->xCameraUsernameEdit->setText(settings.value(xUserNameSettingsName).toString());
        ui->yCameraUsernameEdit->setText(settings.value(yUserNameSettingsName).toString());


    }else{
        //QMessageBox::warning(this,"Внимание","Файл настрок не найден. Значения дефолтные.");
        saveSettings();
    }
}

void MainWindow::saveSettings()
{
    QSettings settings(settingsName,QSettings::IniFormat);
    settings.setValue(xScaleSettingsName,xScale);
    settings.setValue(yScaleSettingsName,yScale);
    settings.setValue(xPositionForYAxisSettingsName,xPositionForYAxis);
    settings.setValue(yPositionForXAxisSettingsName,yPositionForXAxis);
    settings.setValue(xStartPositionSettingsName,xStartPosition);
    settings.setValue(yStartPositionSettingsName,yStartPosition);
    settings.setValue(xStopPositionSettingsName,xStopPosition);
    settings.setValue(yStopPositionSettingsName,yStopPosition);
    settings.setValue(xDeltaSettingsName,xDelta);
    settings.setValue(yDeltaSettingsName,yDelta);
    settings.setValue(xCameraIpSettingsName,ui->xCameraIpEdit->text());
    settings.setValue(yCameraIpSettingsName,ui->yCameraIpEdit->text());
    settings.setValue(xUserNameSettingsName,ui->xCameraUsernameEdit->text());
    settings.setValue(yUserNameSettingsName,ui->yCameraUsernameEdit->text());
}

void MainWindow::showPic(Mat &pic)
{
    ShowPicForm *f = new ShowPicForm(pic);
    f->setAttribute(Qt::WA_DeleteOnClose);
    f->show();
}

void MainWindow::calibrate(Qt::Orientation type, Mat &pic)
{
    CalibrationForm *f = new CalibrationForm(type,pic);
    connect(f,SIGNAL(saveCalibration(Qt::Orientation,double,double,int,int,int)),this,SLOT(saveCalibration(Qt::Orientation,double,double,int,int,int)));
    f->setAttribute(Qt::WA_DeleteOnClose);
    f->show();
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

    QPen pp(QColor(0,0,255,128));
    pp.setWidth(1);
    xSmoothProfileGraph->setPen(pp);
    ySmoothProfileGraph->setPen(pp);

    xSmoothSmoothProfileGraph = ui->plot->addGraph(ui->plot->xAxis,ui->plot->yAxis2);
    ySmoothSmoothProfileGraph = ui->plot->addGraph(ui->plot->yAxis,ui->plot->xAxis2);
    xSmoothSmoothProfileGraph->setLayer("mid");
    ySmoothSmoothProfileGraph->setLayer("mid");

    QPen ppp(QColor(0,0,255));
    ppp.setWidth(2);
    xSmoothSmoothProfileGraph->setPen(ppp);
    ySmoothSmoothProfileGraph->setPen(ppp);

    xyProfileMap = new QCPColorMap(ui->plot->xAxis,ui->plot->yAxis);
    xyProfileMap->setLayer("low");


    ui->plot->xAxis->setRange(QCPRange(xDelta,(xStopPosition-xStartPosition)*xScale+xDelta));
    ui->plot->yAxis->setRange(QCPRange(yDelta,(yStopPosition-yStartPosition)*yScale+yDelta));

    ui->plot->xAxis2->setRange(QCPRange(0,4));
    ui->plot->yAxis2->setRange(QCPRange(0,4));

    //xyProfileMap->data()->setRange(QCPRange(xDelta,abs(xStopPosition-xStartPosition)*xScale+xDelta),QCPRange(yDelta,abs(yStopPosition-yStartPosition)*yScale+yDelta));
    xyProfileMap->data()->clear();
    xyProfileMap->data()->setSize(xProfile.count(),yProfile.count());
    xyProfileMap->data()->setRange(QCPRange(xDelta,xProfile.count()*fabs(xScale)+xDelta),QCPRange(yDelta,yProfile.count()*fabs(yScale)+yDelta));


    colorScale = new QCPColorScale(ui->plot);
    colorScale->setType(QCPAxis::atRight);
    g.clearColorStops();
    g.setColorInterpolation(QCPColorGradient::ColorInterpolation::ciRGB);
    g.setColorStopAt(0,QColor(255,255,255));
    g.setColorStopAt(1,QColor(0,0,255));
    //g.setLevelCount(50);
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


    centerTracer = new QCPItemTracer(ui->plot);
    centerTracer->setLayer("mid");
    centerTracer->setStyle(QCPItemTracer::tsPlus);
    centerTracer->setSize(5);


    manager = new TracerManager(190,ui->plot);
    ui->plot->moveLayer(ui->plot->layer("grid"),ui->plot->layer("top"));
    ui->plot->replot();
}


void MainWindow::setEnabledXCamer(bool value)
{
    ui->xCameraIpEdit->setEnabled(value);
    ui->xCameraUsernameEdit->setEnabled(value);
    ui->xCameraPassword->setEnabled(value);
    ui->xCameraConnectButton->setEnabled(value);
    ui->xCameraCalibrationButton->setEnabled(!value);
    ui->xIndicator->setState(!value);
}

void MainWindow::setEnabledYCamer(bool value)
{
    ui->yCameraIpEdit->setEnabled(value);
    ui->yCameraUsernameEdit->setEnabled(value);
    ui->yCameraPassword->setEnabled(value);
    ui->yCameraConnectButton->setEnabled(value);
    ui->yCameraCalibrationButton->setEnabled(!value);
    ui->yIndicator->setState(!value);
}

void MainWindow::updateProfiles(QVector<double> &xProfile,QVector<double> &yProfile)
{
    ui->xIndicator->blink();
    ui->yIndicator->blink();

    if(xScale < 0){
        for(int i = 0; i < xProfile.count()/2; i++){
            double tmp = xProfile[i];
            xProfile[i] = xProfile[(xProfile.count()-1)-i];
            xProfile[(xProfile.count()-1)-i] = tmp;
        }
    }
    if(yScale < 0){
        for(int i = 0; i < yProfile.count()/2; i++){
            double tmp = yProfile[i];
            yProfile[i] = yProfile[(yProfile.count()-1)-i];
            yProfile[(yProfile.count()-1)-i] = tmp;
        }
    }

    //-------------------------------------------------

    WaveletSpectrum *dwt = new WaveletSpectrum(xProfile,WaveletSpectrum::BSPLINE_309);
    dwt->highFilter(0);
    xSmoothProfile = dwt->toData();
    dwt->levelFilter((dwt->getLevels()/2)-1,0);
    dwt->levelFilter((dwt->getLevels()/2)-2,0);
    xSmoothSmoothProfile = dwt->toData();
    delete dwt;


    dwt = new WaveletSpectrum(yProfile,WaveletSpectrum::BSPLINE_309);
    dwt->highFilter(0);
    ySmoothProfile = dwt->toData();
    dwt->levelFilter((dwt->getLevels()/2)-1,0);
    dwt->levelFilter((dwt->getLevels()/2)-2,0);
    ySmoothSmoothProfile = dwt->toData();
    delete dwt;
    //--------------------------------------------
    double xMax = 0;
    int xMaxId = 0;

    double yMax = 0;
    int yMaxId = 0;

    for(int i = 0; i < xSmoothSmoothProfile.count(); i++){
        if(xSmoothSmoothProfile.at(i) > xMax){
            xMax = xSmoothSmoothProfile.at(i);
            xMaxId = i;
        }
    }
    for(int i = 0; i < ySmoothSmoothProfile.count(); i++){
        if(ySmoothSmoothProfile.at(i) > yMax){
            yMax = ySmoothSmoothProfile.at(i);
            yMaxId = i;
        }
    }
    //--------------------------------------------

    xWidthKeys.clear();
    yHeightKeys.clear();
    for(int i = 1; i < xSmoothProfile.count()-1; i++){
        if(xSmoothProfile.at(i-1) < xMax/2.0 && xSmoothProfile.at(i+1) > xMax/2.0){
            xWidthKeys.append(i);
            break;
        }
    }
    for(int i = 1; i < xSmoothProfile.count()-1; i++){
        if(xSmoothProfile.at(i-1) > xMax/2.0 && xSmoothProfile.at(i+1) < xMax/2.0){
            xWidthKeys.append(i);
            break;
        }
    }

    for(int i = 1; i < ySmoothProfile.count()-1; i++){
        if(ySmoothProfile.at(i-1) < yMax/2.0 && ySmoothProfile.at(i+1) > yMax/2.0){
            yHeightKeys.append(i);
            break;
        }
    }
    for(int i = 1; i < ySmoothProfile.count()-1; i++){
        if(ySmoothProfile.at(i-1) > yMax/2.0 && ySmoothProfile.at(i+1) < yMax/2.0){
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

            ui->widthLabel->setText(QString::number(qRound(xLengthKeys.at(xWidthKeys[1])-xLengthKeys.at(xWidthKeys[0]))));
            ui->heightLabel->setText(QString::number(qRound(yLengthKeys.at(yHeightKeys[1])-yLengthKeys.at(yHeightKeys[0]))));
        }
    }else{
        widthLine->start->setCoords(xLengthKeys.at(0),yLengthKeys.at(0));
        widthLine->end->setCoords(xLengthKeys.at(0),yLengthKeys.at(0));

        heightLine->start->setCoords(xLengthKeys.at(0),yLengthKeys.at(0));
        heightLine->end->setCoords(xLengthKeys.at(0),yLengthKeys.at(0));

        ui->widthLabel->setText("--");
        ui->heightLabel->setText("--");
    }

    ui->xMaxLabel->setText(QString::number(qRound(xLengthKeys.at(xMaxId))));
    ui->yMaxLabel->setText(QString::number(qRound(yLengthKeys.at(yMaxId))));
    centerTracer->position->setCoords(xLengthKeys.at(xMaxId),yLengthKeys.at(yMaxId));
    manager->apdateLines(xLengthKeys.at(xMaxId),yLengthKeys.at(yMaxId));

    //--------------------------------------------
    xProfileGraph->clearData();
    xProfileGraph->setData(xLengthKeys,xProfile);
    yProfileGraph->clearData();
    yProfileGraph->setData(yLengthKeys,yProfile);

    xSmoothSmoothProfileGraph->clearData();
    xSmoothSmoothProfileGraph->setData(xLengthKeys,xSmoothSmoothProfile);
    ySmoothSmoothProfileGraph->clearData();
    ySmoothSmoothProfileGraph->setData(yLengthKeys,ySmoothSmoothProfile);

    xSmoothProfileGraph->clearData();
    xSmoothProfileGraph->setData(xLengthKeys,xSmoothProfile);
    ySmoothProfileGraph->clearData();
    ySmoothProfileGraph->setData(yLengthKeys,ySmoothProfile);


    //--------------------------------------------


    for(int y = 0; y < ySmoothProfile.count(); y++){
        for(int x = 0; x < xSmoothProfile.count(); x++){
            double value;
            if(ui->noSmoothRadioButton->isChecked()){
                value = xProfile[x]*yProfile[y];
            }
            if(ui->smoothRadioButton->isChecked()){
                value = xSmoothProfile[x]*ySmoothProfile[y];
            }
            if(ui->smoothSmoothRadioButton->isChecked()){
                value = xSmoothSmoothProfile[x]*ySmoothSmoothProfile[y];
            }
            xyProfileMap->data()->setCell(x,y,value);
        }
    }

    ui->plot->xAxis->setRange(QCPRange(xDelta,xProfile.count()*fabs(xScale)+xDelta));
    ui->plot->yAxis->setRange(QCPRange(yDelta,yProfile.count()*fabs(yScale)+yDelta));

    ui->plot->xAxis2->setRange(QCPRange(0,4));
    ui->plot->yAxis2->setRange(QCPRange(0,4));

    ui->plot->replot();
}

void MainWindow::handleOpenResult(Qt::Orientation type, bool status)
{
    if(type == Qt::Horizontal){
        if(status){
            setEnabledXCamer(false);
        }else{
            setEnabledXCamer(true);
            QMessageBox::critical(this,"Ошибка","Не удалось подключиться.");
        }
    }
    if(type == Qt::Vertical){
        if(status){
            setEnabledYCamer(false);
        }else{
            setEnabledYCamer(true);
            QMessageBox::critical(this,"Ошибка","Не удалось подключиться.");
        }
    }
}

void MainWindow::handleLostConnection(Qt::Orientation type)
{
    on_startStopButton_clicked();

    if(type == Qt::Horizontal){
        setEnabledXCamer(true);
        //QMessageBox::critical(this,"Ошибка","Связь с камерой X потеряна.");
    }
    if(type == Qt::Vertical){
        setEnabledYCamer(true);
        //QMessageBox::critical(this,"Ошибка","Связь с камерой Y потеряна.");
    }

}

void MainWindow::saveCalibration(Qt::Orientation type, double scale, double delta, int start, int stop, int otherPixel)
{
    if(type == Qt::Horizontal){
        xScale = scale;
        xDelta = delta;
        xStartPosition = start;
        xStopPosition = stop;
        yPositionForXAxis = otherPixel;
        xProfile.resize(abs(xStopPosition-xStartPosition));
        xLengthKeys.clear();
        for(int i = 0; i < xProfile.count(); i++){
            xLengthKeys.append(xDelta + fabs(xScale)*i);
        }
        ui->plot->xAxis->setRange(QCPRange(xDelta,(xStopPosition-xStartPosition)*xScale+xDelta));
        ui->plot->replot();
        saveSettings();
    }
    if(type == Qt::Vertical){
        yScale = scale;
        yDelta = delta;
        yStartPosition = start;
        yStopPosition = stop;
        xPositionForYAxis = otherPixel;
        yProfile.resize(abs(yStopPosition-yStartPosition));
        yLengthKeys.clear();
        for(int i = 0; i < yProfile.count(); i++){
            yLengthKeys.append(yDelta + fabs(yScale)*i);
        }
        ui->plot->yAxis->setRange(QCPRange(yDelta,(yStopPosition-yStartPosition)*yScale+yDelta));
        ui->plot->replot();
        saveSettings();
    }
}

void MainWindow::on_startStopButton_clicked()
{
    if(xStartPosition == 0 || xStopPosition == 0 || yStartPosition == 0 || yStartPosition == 0 || xPositionForYAxis == 0 || yPositionForXAxis == 0){
        QMessageBox::critical(this,"Ошибка","Не выполнена калибровка.");
        return ;
    }
    if(xCapure->isOpend() && yCapure->isOpend()){
        if(!isRunning){
            isRunning = true;
            ui->startStopButton->setText("Стоп");

            ui->xCameraCalibrationButton->setEnabled(false);
            ui->yCameraCalibrationButton->setEnabled(false);

            frameGrapTimer->start();
        }else{
            isRunning = false;
            ui->startStopButton->setText("Старт");

            ui->xCameraCalibrationButton->setEnabled(true);
            ui->yCameraCalibrationButton->setEnabled(true);

            frameGrapTimer->stop();
        }
    }else{
        QMessageBox::information(this,"Ошибка","Камеры не подключены");
    }
}


void MainWindow::on_xCameraCalibrationButton_clicked()
{
    if(xCapure->isOpend()){
        Mat pic = xCapure->takeSingleFrame();
        if(pic.data){
            calibrate(Qt::Horizontal,pic);
        }else{
            QMessageBox::critical(this,":(",":(");
        }
    }
}

void MainWindow::on_yCameraCalibrationButton_clicked()
{
    if(yCapure->isOpend()){
        Mat pic = yCapure->takeSingleFrame();
        if(pic.data){
            calibrate(Qt::Vertical,pic);
        }else{
            QMessageBox::critical(this,":(",":(");
        }
    }
}

void MainWindow::handleFrame(Qt::Orientation type, Mat newPic)
{
    if(type == Qt::Horizontal){
        xReady = true;
        ximage = newPic;
    }
    if(type == Qt::Vertical){
        yReady = true;
        yimage = newPic;
    }
    if(xReady && yReady){
        xReady = false;
        yReady = false;

        int start = 0;
        int stop = 0;
        if(xScale > 0){
            start = xStartPosition;
            stop = xStopPosition;
        }else{
            start = xStopPosition;
            stop = xStartPosition;
        }
        for(int x = start; x < stop; x++) {
            uchar b = ximage.at<Vec3b>(yPositionForXAxis,x)[0];
            xProfile[x-start] = (double)b/255.0;
        }

        if(yScale > 0){
            start = yStartPosition;
            stop = yStopPosition;
        }else{
            start = yStopPosition;
            stop = yStartPosition;
        }

        for(int y = start; y < stop; y++) {
            uchar b = yimage.at<Vec3b>(y,xPositionForYAxis)[0];
            yProfile[y-start] = (double)b/255.0;
        }
        updateProfiles(xProfile,yProfile);
        yimage.release();
        ximage.release();
    }
}

void MainWindow::on_xCameraConnectButton_clicked()
{
    xCapure->setConnectionString(QString("rtsp://%1:%2@%3:554/Streaming/Channels/103").arg(ui->xCameraUsernameEdit->text()).arg(ui->xCameraPassword->text()).arg(ui->xCameraIpEdit->text()));
    setEnabledXCamer(false);
    ui->xIndicator->setState(false);
    emit openXCapture();
}

void MainWindow::on_yCameraConnectButton_clicked()
{
    yCapure->setConnectionString(QString("rtsp://%1:%2@%3:554/Streaming/Channels/103").arg(ui->yCameraUsernameEdit->text()).arg(ui->yCameraPassword->text()).arg(ui->yCameraIpEdit->text()));
    setEnabledYCamer(false);
    ui->yIndicator->setState(false);
    emit openYCapture();
}
