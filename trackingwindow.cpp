#include "trackingwindow.h"
#include "ui_trackingwindow.h"

////////// CONSTRUCTOR //////////

TrackingWindow::TrackingWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrackingWindow)
{
    ui->setupUi(this);

    ////////// INITIALIZATION ///////////

    refreshComLocalList();
    refreshComOneList();

    timer.start();

    ui->baudLocalBox->addItems(QStringList()<<"4800"<<"9600");
    ui->baudOneBox->addItems(QStringList()<<"9600"<<"115200");

    PortSettings setLocal = {BAUD4800, DATA_8, PAR_NONE, STOP_1, FLOW_OFF, 10};
    PortSettings setOne = {BAUD9600, DATA_8, PAR_NONE, STOP_1, FLOW_OFF, 10};
    portLocal = new QextSerialPort(this->ui->comLocalBox->currentText(), setLocal, QextSerialPort::EventDriven);
    portOne = new QextSerialPort(this->ui->comOneBox->currentText(), setOne, QextSerialPort::EventDriven);

    fileCounter = 1;
    while(QFile::exists("LocalData_"+QString::number(fileCounter, 'i', 0)+".txt") ||
          QFile::exists("TargetOneData_"+QString::number(fileCounter, 'i', 0)+".txt")){
        fileCounter++;
    }

    localFile = new QFile("LocalData_"+QString::number(fileCounter, 'i', 0)+".txt");
    localFile->open(QIODevice::WriteOnly | QIODevice::Text);
    oneFile = new QFile("TargetOneData_"+QString::number(fileCounter, 'i', 0)+".txt");
    oneFile->open(QIODevice::WriteOnly | QIODevice::Text);

    mapButtons.addButton(this->ui->mapRadioButton);
    mapButtons.addButton(this->ui->satRadioButton);
    mapButtons.setExclusive(true);

    clearGpsOneWarning();
    clearGpsLocalWarning();

    startLat = "28.58086";
    startLon = "-80.64980";

    localLat = "##.####";
    localLon = "###.####";

    oneLat = "##.####";
    oneLon = "###.####";

    pred_oneLat = "##.####";
    pred_oneLon = "###.####";

    roadMap = "&maptype=roadmap";
    satImg = "&maptype=hybrid";
    mapSetup = "http://maps.googleapis.com/maps/api/staticmap?size=640x640&sensor=true";
    initMap();

    greenButtonStyle = "QPushButton {border: 2px solid #169E2B;border-radius: 5px;}QPushButton:hover {border: 2px solid #729E54;background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #DBFCC5, stop: 1 #9AC77B);}QPushButton:pressed {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #9AC77B, stop: 1 #DBFCC5);}";
    yellowButtonStyle = "QPushButton {border: 2px solid #B37D00;border-radius: 5px;}QPushButton:hover {border: 2px solid #B37D00;background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #FCE55D, stop: 1 #FCD700);}QPushButton:pressed {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #FCD700, stop: 1 #FCE55D);}";
    redButtonStyle = "QPushButton {border: 2px solid #A30810;border-radius: 5px;}QPushButton:hover {border: 2px solid #A30810;color:white;background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #FA0C18, stop: 1 #B80912);}QPushButton:pressed {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #B80912, stop: 1 #FA0C18);}";
    grayButtonStyle = "QPushButton {border: 2px solid #8f8f91;border-radius: 5px;background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #F6F7FA, stop: 1 #DADBDE);}QPushButton:hover {border: 2px solid #729E54;background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #DBFCC5, stop: 1 #9AC77B);}QPushButton:pressed {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #9AC77B, stop: 1 #DBFCC5);}";
    ackButtonStyle = "QPushButton {border: 2px solid #147A00;border-radius: 5px;background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #44E324, stop: 1 #1FBD00);}QPushButton:hover {border: 2px solid #147A00;background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #44E324, stop: 1 #1FBD00);}QPushButton:pressed {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #1FBD00, stop: 1 #44E324);}";

    ////////// SIGNAL AND SLOTS //////////

    QObject::connect(ui->mapRadioButton, SIGNAL(clicked()), this, SLOT(buildMapNow()));
    QObject::connect(ui->satRadioButton, SIGNAL(clicked()), this, SLOT(buildMapNow()));
    QObject::connect(ui->zoomSlide, SIGNAL(valueChanged(int)), this, SLOT(setSliderValue()));
    QObject::connect(ui->zoomSlide, SIGNAL(sliderReleased()), this, SLOT(buildMapNow()));
    QObject::connect(ui->connectLocalButton, SIGNAL(clicked()), this, SLOT(connectLocal()));
    QObject::connect(ui->connectOneButton, SIGNAL(clicked()), this, SLOT(connectOne()));
    QObject::connect(ui->baudLocalBox, SIGNAL(currentIndexChanged(int)), this, SLOT(baudLocalChange(int)));
    QObject::connect(ui->baudOneBox, SIGNAL(currentIndexChanged(int)), this, SLOT(baudOneChange(int)));
    QObject::connect(ui->comLocalBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(portLocalChange(QString)));
    QObject::connect(ui->comOneBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(portOneChange(QString)));
    QObject::connect(portLocal, SIGNAL(readyRead()), this, SLOT(readLocal()));
    QObject::connect(portOne, SIGNAL(readyRead()), this, SLOT(readOne()));
    QObject::connect(ui->enableOneButton, SIGNAL(clicked()), this, SLOT(enableOneCommands()));
    QObject::connect(ui->armOneButton, SIGNAL(clicked()), this, SLOT(armOne()));
    QObject::connect(ui->drogueOneButton, SIGNAL(clicked()), this, SLOT(fireDrogue()));
    QObject::connect(ui->mainOneButton, SIGNAL(clicked()), this, SLOT(fireMain()));
    QObject::connect(ui->mapRefreshButton, SIGNAL(clicked()), this, SLOT(buildMapNow()));
    QObject::connect(ui->comLocalRefreshButton, SIGNAL(clicked()), this, SLOT(refreshComLocalList()));
    QObject::connect(ui->comOneRefreshButton, SIGNAL(clicked()), this, SLOT(refreshComOneList()));
}

//////////// MAP METHODS ////////////

void TrackingWindow::initMap(){
    mapCenter = "&center="+startLat+","+startLon;
    startMarker = "&markers=color:yellow|label:S|"+startLat+","+startLon;
    zoom = "&zoom=" + QString::number(ui->zoomSlide->value());
    setSliderValue();
    if(ui->mapRadioButton->isChecked()){
        url = mapSetup+mapCenter+zoom+roadMap+startMarker;
    } else {
        url = mapSetup+mapCenter+zoom+satImg+startMarker;
    }
    ui->mapWindow->load(QUrl(url));
}

void TrackingWindow::mapLocal(){
    mapCenter = "&center="+localLat+","+localLon;
    localMarker = "&markers=color:blue|label:L|"+localLat+","+localLon;
    zoom = "&zoom=" + QString::number(ui->zoomSlide->value());
    if(ui->mapRadioButton->isChecked()){
        url = mapSetup+mapCenter+zoom+roadMap+localMarker;
    } else {
        url = mapSetup+mapCenter+zoom+satImg+localMarker;
    }
    ui->mapWindow->load(QUrl(url));
}

void TrackingWindow::mapOne(){
    mapCenter = "&center"+oneLat+","+oneLon;
    oneMarker = "&markers=color:green|label:1|"+oneLat+","+oneLon;
    zoom = "&zoom=" + QString::number(ui->zoomSlide->value());
    if(!pred_oneLat.contains(QRegExp("#"))){
        pred_oneMarker = "&markers=color:red|label:1|"+pred_oneLat+","+pred_oneLon;
        if(ui->mapRadioButton->isChecked()){
            url = mapSetup+mapCenter+zoom+roadMap+oneMarker+pred_oneMarker;
        } else {
            url = mapSetup+mapCenter+zoom+satImg+oneMarker+pred_oneMarker;
        }
    } else {
        if(ui->mapRadioButton->isChecked()){
            url = mapSetup+mapCenter+zoom+roadMap+oneMarker;
        } else {
            url = mapSetup+mapCenter+zoom+satImg+oneMarker;
        }
    }
    ui->mapWindow->load(QUrl(url));
}

void TrackingWindow::mapAll(){
    mapCenter = "&center="+oneLat+","+oneLon;
    localMarker = "&markers=color:blue|label:L|"+localLat+","+localLon;
    oneMarker = "&markers=color:green|label:1|"+oneLat+","+oneLon;
    zoom = "&zoom=" + QString::number(ui->zoomSlide->value());
    if(!pred_oneLat.contains(QRegExp("#"))){
        pred_oneMarker = "&markers=color:red|label:1|"+pred_oneLat+","+pred_oneLon;
        if(ui->mapRadioButton->isChecked()){
            url = mapSetup+mapCenter+zoom+roadMap+oneMarker+pred_oneMarker+localMarker;
        } else {
            url = mapSetup+mapCenter+zoom+satImg+oneMarker+pred_oneMarker+localMarker;
        }
    } else {
        if(ui->mapRadioButton->isChecked()){
            url = mapSetup+mapCenter+zoom+roadMap+oneMarker+localMarker;
        } else {
            url = mapSetup+mapCenter+zoom+satImg+oneMarker+localMarker;
        }
    }
    ui->mapWindow->load(QUrl(url));
}

void TrackingWindow::buildMapNow(){
    timer.restart();

    if(!localLat.contains(QRegExp("#")) && !oneLat.contains(QRegExp("#"))){
        mapAll();
        return;
    }
    else if(localLat.contains(QRegExp("#")) && !oneLat.contains(QRegExp("#"))){
        mapOne();
        return;
    }
    else if(!localLat.contains(QRegExp("#")) && oneLat.contains(QRegExp("#"))){
        mapLocal();
        return;
    }
    else {
        initMap();
        return;
    }
}

void TrackingWindow::buildMapDelay(){
    if(timer.elapsed() < 30000){
        return;
    } else {
        buildMapNow();
    }
    timer.restart();
}

void TrackingWindow::setSliderValue(){
    ui->zoomSliderLabel->setText("Zoom = " + QString::number(this->ui->zoomSlide->value()));
}

/////////// CONNECTION METHODS ////////////

void TrackingWindow::connectLocal(){
    if(!portLocal->isOpen()){
        portLocal->setPortName(ui->comLocalBox->currentText());
        portLocal->setBaudRate((BaudRateType)ui->baudLocalBox->currentText().toInt());
        portLocal->open(QIODevice::ReadWrite);
    } else {
        portLocal->close();
    }
    setConnectButton();
}

void TrackingWindow::connectOne(){
    if(!portOne->isOpen()){
        portOne->setPortName(ui->comOneBox->currentText());
        portOne->setBaudRate((BaudRateType)ui->baudOneBox->currentText().toInt());
        portOne->open(QIODevice::ReadWrite);
    } else {
        portOne->close();
    }
    setConnectButton();
}

void TrackingWindow::setConnectButton(){
    if(portLocal->isOpen()){
        ui->connectLocalButton->setText("CLOSE");
        ui->connectLocalButton->setStyleSheet(greenButtonStyle);
    } else {
        ui->connectLocalButton->setText("CONNECT");
        ui->connectLocalButton->setStyleSheet(redButtonStyle);
    }
    if(portOne->isOpen()){
        ui->connectOneButton->setText("CLOSE");
        ui->connectOneButton->setStyleSheet(greenButtonStyle);
    } else {
        ui->connectOneButton->setText("CONNECT");
        ui->connectOneButton->setStyleSheet(redButtonStyle);
    }
}


void TrackingWindow::enableOneCommands(){
    if(portOne->isOpen()){
        QMessageBox exitMessage;
        exitMessage.setIcon(QMessageBox::Warning);
        exitMessage.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
        exitMessage.setDefaultButton(QMessageBox::Ok);

        if("ENABLE\nCommanding" == ui->enableOneButton->text()){
            exitMessage.setText("<p align='center'><b>ENABLE</b><br>" "Commanding System Now?</p>");
            int ret = exitMessage.exec();
            switch(ret){
            case QMessageBox::Ok:
                ui->armOneButton->setEnabled(true);
                ui->armOneButton->setStyleSheet(yellowButtonStyle);
                ui->enableOneButton->setText("DISABLE\nCommanding");
                ui->enableOneButton->setStyleSheet(greenButtonStyle);
                break;
            }

        } else {                          // This allows the same button to be used as both an Enable and Disable button
            exitMessage.setText("<p align='center'><b>DISABLE</b><br>" "Commanding System Now?</p>");
            int ret = exitMessage.exec();
            switch(ret){
            case QMessageBox::Ok:
                portOne->write("D");        // Disarm Command
                ui->armOneButton->setEnabled(false);
                ui->drogueOneButton->setEnabled(false);
                ui->mainOneButton->setEnabled(false);
                ui->armOneButton->setStyleSheet(grayButtonStyle);
                ui->drogueOneButton->setStyleSheet(grayButtonStyle);
                ui->mainOneButton->setStyleSheet(grayButtonStyle);
                ui->enableOneButton->setText("ENABLE\nCommanding");
                ui->enableOneButton->setStyleSheet(redButtonStyle);
                break;
            }
        }
    }
}

void TrackingWindow::armOne(){
    if(portOne->isOpen()){
        portOne->write("<");
        ui->armOneButton->setStyleSheet(redButtonStyle);
        ui->drogueOneButton->setEnabled(true);
        ui->drogueOneButton->setStyleSheet(yellowButtonStyle);
        ui->mainOneButton->setEnabled(true);
        ui->mainOneButton->setStyleSheet(yellowButtonStyle);
    }
}

void TrackingWindow::fireDrogue(){
    if(portOne->isOpen()){
        portOne->write(":");
        ui->drogueOneButton->setStyleSheet(redButtonStyle);
    }
}

void TrackingWindow::fireMain(){
    if(portOne->isOpen()){
        portOne->write("?");
        ui->mainOneButton->setStyleSheet(redButtonStyle);
    }
}

void TrackingWindow::portLocalChange(const QString &/*name*/){
    portLocal->close();
    setConnectButton();
}

void TrackingWindow::portOneChange(const QString & /*name*/){
    portOne->close();
    setConnectButton();
}

void TrackingWindow::baudLocalChange(int /*index*/){
    portLocal->close();
    setConnectButton();
}

void TrackingWindow::baudOneChange(int /*index*/){
    portOne->close();
    setConnectButton();
}

void TrackingWindow::refreshComLocalList(){
    usbPortList.clear();
    ui->comLocalBox->clear();
    usbPortList = SerialList::getSerialPorts();
    ui->comLocalBox->addItems(usbPortList);
}

void TrackingWindow::refreshComOneList(){
    usbPortList.clear();
    ui->comOneBox->clear();
    usbPortList = SerialList::getSerialPorts();
    ui->comOneBox->addItems(usbPortList);
}

///////// DATA STATUS METHODS //////////

void TrackingWindow::setGpsLocalWarning(){
    ui->latLocalStale->setVisible(true);
    ui->lonLocalStale->setVisible(true);
}

void TrackingWindow::clearGpsLocalWarning(){
    ui->latLocalStale->setVisible(false);
    ui->lonLocalStale->setVisible(false);
}

void TrackingWindow::setGpsOneWarning(){
    ui->statusOneCaution->setVisible(true);
    ui->latOneStale->setVisible(true);
    ui->lonOneStale->setVisible(true);
}

void TrackingWindow::clearGpsOneWarning(){
    ui->statusOneCaution->setVisible(false);
    ui->latOneStale->setVisible(false);
    ui->lonOneStale->setVisible(false);
}

///////// READ SERIAL DATA //////////

void TrackingWindow::readLocal(){
    QTextStream outStream(localFile);
    if(portLocal->canReadLine()){
        QString temp = QString(portLocal->readLine());
        lastLocal.parseData(temp);
        setLocal();
        ui->dataLocal->moveCursor(QTextCursor::End);
        ui->dataLocal->insertPlainText(temp);
        outStream<<temp;
    }
}

void TrackingWindow::readOne(){
    QTextStream outStream(oneFile);
    if(portOne->canReadLine()){
        QString temp = QString(portOne->readLine());
        lastOne.parseData(temp);
        setOne();
        ui->dataOne->moveCursor(QTextCursor::End);
        ui->dataOne->insertPlainText(temp);
        outStream << temp;
    }
}

///////// DISPLAY DATA ////////

void TrackingWindow::setLocal(){
    ui->latLocal->setText(lastLocal.getLat());
    ui->lonLocal->setText(lastLocal.getLon());
    ui->timeLocal->setText(lastLocal.getUtc());
    ui->altLocalGps->setText(lastLocal.getMslFT());
    if(!lastLocal.getLat().contains(QRegExp("#"))){
        localLat = lastLocal.getLat();
        localLon = lastLocal.getLon();
    }
    if(false == lastLocal.getValid()){
        setGpsLocalWarning();
    } else {
        clearGpsLocalWarning();
    }
    buildMapDelay();
}

void TrackingWindow::setOne(){
    ui->latOne->setText(lastOne.getLat());
    ui->lonOne->setText(lastOne.getLon());
    ui->timeOne->setText(lastOne.getUtc());
    ui->altOneGps->setText(lastOne.getMslFT());
    ui->altOneBaro->setText(lastOne.getBaroAltFT());
    ui->intTempOne->setText(lastOne.getIntTempF());
    ui->ascentOne->setText(lastOne.getAscentFM());
    ui->statusOne->setText(lastOne.getStatusLine());
    ui->headOne->setText(GPSPoint::gpsHeading(lastLocal.getLat(), lastLocal.getLon(), lastOne.getLat(), lastOne.getLon()));
    ui->distOne->setText(GPSPoint::gpsDistanceFT(lastLocal.getLat(), lastLocal.getLon(), lastOne.getLat(), lastOne.getLon()));
    ui->elevOne->setText(GPSPoint::gpsAzmiuth(lastLocal.getLat(), lastLocal.getLon(), lastLocal.getMsl(), lastOne.getLat(), lastOne.getLon(), lastOne.getMsl()));
    ui->packetOne->setText(lastOne.getPacket());
    ui->predLatOne->setText(lastOne.getPlat());
    ui->predLonOne->setText(lastOne.getPlon());
    if(!lastOne.getLat().contains(QRegExp("#"))){
        oneLat = lastOne.getLat();
        oneLon = lastOne.getLon();
        pred_oneLat = lastOne.getPlat();
        pred_oneLon = lastOne.getPlon();
    }
    if(true == lastOne.getArm()){
        ui->armOneButton->setStyleSheet(ackButtonStyle);
    }
    else if(!ui->armOneButton->isEnabled()){
        ui->armOneButton->setStyleSheet(grayButtonStyle);
    }
    if(true == lastOne.getDrogue()){
        ui->drogueOneButton->setStyleSheet(ackButtonStyle);
    }
    else if(!ui->drogueOneButton->isEnabled()){
        ui->drogueOneButton->setStyleSheet(grayButtonStyle);
    }
    if(true == lastOne.getFire()){
        ui->mainOneButton->setStyleSheet(ackButtonStyle);
    }
    else if(!ui->mainOneButton->isEnabled()){
        ui->mainOneButton->setStyleSheet(grayButtonStyle);
    }
    if(false == lastOne.getValid()){
        setGpsOneWarning();
    } else {
        clearGpsOneWarning();
    }
    buildMapDelay();
}

/////// DESTRUCTOR ///////

TrackingWindow::~TrackingWindow()
{
    delete ui;
    delete portLocal;
    delete portOne;
    delete localFile;
    delete oneFile;
}
