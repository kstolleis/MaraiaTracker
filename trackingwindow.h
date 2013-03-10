#ifndef TRACKINGWINDOW_H
#define TRACKINGWINDOW_H

#include <QWidget>
#include <QTime>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QButtonGroup>
#include <QMessageBox>
#include <QIODevice>
#include <QRegExp>
#include <iostream>
#include "qextserialport.h"
#include "gpspoint.h"
#include "seriallist.h"


namespace Ui {
class TrackingWindow;
}

class TrackingWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit TrackingWindow(QWidget *parent = 0);
    ~TrackingWindow();

private slots:
    void initMap();
    void buildMapNow();
    void setSliderValue();
    void connectLocal();
    void connectOne();
    void setConnectButton();
    void portLocalChange(const QString &name);
    void portOneChange(const QString &name);
    void baudLocalChange(int index);
    void baudOneChange(int index);
    void readLocal();
    void readOne();
    void enableOneCommands();
    void armOne();
    void fireDrogue();
    void fireMain();
    void refreshComLocalList();
    void refreshComOneList();
    
private:    
    Ui::TrackingWindow *ui;
    QString mapCenter, startLat, startLon, startMarker, url, zoom, localMarker, localLat, localLon, oneMarker, pred_oneMarker, oneLat, oneLon, pred_oneLat, pred_oneLon;
    QString  roadMap, satImg, mapSetup, grayButtonStyle, redButtonStyle, greenButtonStyle, yellowButtonStyle, ackButtonStyle;
    QextSerialPort *portLocal, *portOne;
    GPSPoint lastLocal, lastOne;
    QTime timer;
    QFile *localFile, *oneFile;
    QButtonGroup mapButtons;
    QStringList usbPortList;
    int fileCounter;

    void buildMapDelay();
    void mapLocal();
    void mapOne();
    void mapAll();
    void setLocal();
    void setOne();
    void setGpsLocalWarning();
    void clearGpsLocalWarning();
    void setGpsOneWarning();
    void clearGpsOneWarning();
};

#endif // TRACKINGWINDOW_H
