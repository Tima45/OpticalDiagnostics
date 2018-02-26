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
    xCapure->name = "X";
    yCapure = new CaptureManager(30);
    yCapure->name = "Y";

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


    connect(this,SIGNAL(openXCapture()),xCapure,SLOT(open()));
    connect(this,SIGNAL(openYCapture()),yCapure,SLOT(open()));
    connect(xCapure,SIGNAL(openResult(QString,bool)),this,SLOT(handleOpenResult(QString,bool)));
    connect(yCapure,SIGNAL(openResult(QString,bool)),this,SLOT(handleOpenResult(QString,bool)));

    connect(xCapure,SIGNAL(newFrame(QString,Mat)),this,SLOT(handleFrame(QString,Mat)),Qt::QueuedConnection);
    connect(xCapure,SIGNAL(losedConnection(QString)),this,SLOT(handleLostConnection(QString)),Qt::QueuedConnection);
    connect(yCapure,SIGNAL(newFrame(QString,Mat)),this,SLOT(handleFrame(QString,Mat)),Qt::QueuedConnection);
    connect(yCapure,SIGNAL(losedConnection(QString)),this,SLOT(handleLostConnection(QString)),Qt::QueuedConnection);


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

void MainWindow::calibrate(QString type, Mat &pic)
{
    CalibrationForm *f = new CalibrationForm(type,pic);
    connect(f,SIGNAL(saveCalibration(QString,double,double,int,int,int)),this,SLOT(saveCalibration(QString,double,double,int,int,int)));
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
    QPen pp(QColor(Qt::blue));
    pp.setWidth(2);
    xSmoothProfileGraph->setPen(pp);
    ySmoothProfileGraph->setPen(pp);

    xyProfileMap = new QCPColorMap(ui->plot->xAxis,ui->plot->yAxis);
    xyProfileMap->setLayer("low");


    ui->plot->xAxis->setRange(QCPRange(xDelta,(xStopPosition-xStartPosition)*xScale+xDelta));
    ui->plot->yAxis->setRange(QCPRange(yDelta,(yStopPosition-yStartPosition)*yScale+yDelta));

    ui->plot->xAxis2->setRange(QCPRange(0,4));
    ui->plot->yAxis2->setRange(QCPRange(0,4));

    xyProfileMap->data()->setRange(QCPRange(xDelta,abs(xStopPosition-xStartPosition)*xScale+xDelta),QCPRange(yDelta,abs(yStopPosition-yStartPosition)*yScale+yDelta));


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
    //dwt->levelFilter(dwt->getLevels()-1,0);
    //dwt->levelFilter(dwt->getLevels()-2,0);
    QVector<double> xSmoothProfile;
    xSmoothProfile = dwt->toData();
    dwt->deleteLater();
    dwt = new WaveletSpectrum(yProfile,WaveletSpectrum::BSPLINE_309);
    dwt->highFilter(0);
    //dwt->levelFilter(dwt->getLevels()-1,0);
    //dwt->levelFilter(dwt->getLevels()-2,0);
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

            ui->widthLabel->setText(QString::number(xLengthKeys.at(xWidthKeys[1])-xLengthKeys.at(xWidthKeys[0])));
            ui->heightLabel->setText(QString::number(yLengthKeys.at(yHeightKeys[1])-yLengthKeys.at(yHeightKeys[0])));
        }
    }

    ui->xMaxLabel->setText(QString::number(xLengthKeys.at(xMaxId)));
    ui->yMaxLabel->setText(QString::number(yLengthKeys.at(yMaxId)));

    //--------------------------------------------
    xProfileGraph->clearData();
    xProfileGraph->setData(xLengthKeys,xProfile);
    yProfileGraph->clearData();
    yProfileGraph->setData(yLengthKeys,yProfile);

    xSmoothProfileGraph->clearData();
    xSmoothProfileGraph->setData(xLengthKeys,xSmoothProfile);
    ySmoothProfileGraph->clearData();
    ySmoothProfileGraph->setData(yLengthKeys,ySmoothProfile);
    //--------------------------------------------

    xyProfileMap->data()->clear();
    xyProfileMap->data()->setSize(xProfile.count(),yProfile.count());
    xyProfileMap->data()->setRange(QCPRange(xDelta,xProfile.count()*fabs(xScale)+xDelta),QCPRange(yDelta,yProfile.count()*fabs(yScale)+yDelta));
    for(int y = 0; y < ySmoothProfile.count(); y++){
        for(int x = 0; x < xSmoothProfile.count(); x++){
            xyProfileMap->data()->setCell(x,y,xSmoothProfile[x]*ySmoothProfile[y]);
        }
    }

    ui->plot->xAxis->setRange(QCPRange(xDelta,xProfile.count()*fabs(xScale)+xDelta));
    ui->plot->yAxis->setRange(QCPRange(yDelta,yProfile.count()*fabs(yScale)+yDelta));

    ui->plot->xAxis2->setRange(QCPRange(0,4));
    ui->plot->yAxis2->setRange(QCPRange(0,4));

    ui->plot->replot();
}

void MainWindow::handleOpenResult(QString name, bool status)
{
    if(name == "X"){
        if(status){
            setEnabledXCamer(false);
        }else{
            QMessageBox::critical(this,"Ошибка","Не удалось подключиться.");
        }
    }
    if(name == "Y"){
        if(status){
            setEnabledYCamer(false);
        }else{
            QMessageBox::critical(this,"Ошибка","Не удалось подключиться.");
        }
    }
}

void MainWindow::handleLostConnection(QString name)
{
    on_startStopButton_clicked();

    if(name == "X"){
        setEnabledXCamer(true);
        QMessageBox::critical(this,"Ошибка","Связь с камерой X потеряна.");
    }
    if(name == "Y"){
        setEnabledYCamer(true);
        QMessageBox::critical(this,"Ошибка","Связь с камерой Y потеряна.");
    }

}

void MainWindow::saveCalibration(QString type, double scale, double delta, int start, int stop, int otherPixel)
{
    if(type == "X"){
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
    if(type == "Y"){
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
            calibrate("X",pic);
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
            calibrate("Y",pic);
        }else{
            QMessageBox::critical(this,":(",":(");
        }
    }
}

void MainWindow::handleFrame(QString name, Mat newPic)
{
    if(name == "X"){
        xReady = true;
        ximage = newPic;
    }
    if(name == "Y"){
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
    }
}

void MainWindow::on_xCameraConnectButton_clicked()
{
    xCapure->setConnectionString(QString("rtsp://%1:%2@%3:554").arg(ui->xCameraUsernameEdit->text()).arg(ui->xCameraPassword->text()).arg(ui->xCameraIpEdit->text()));
    emit openXCapture();
}

void MainWindow::on_yCameraConnectButton_clicked()
{
    yCapure->setConnectionString(QString("rtsp://%1:%2@%3:554").arg(ui->yCameraUsernameEdit->text()).arg(ui->yCameraPassword->text()).arg(ui->yCameraIpEdit->text()));
    emit openYCapture();
}
