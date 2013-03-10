#ifndef GPSPOINT_H
#define GPSPOINT_H

#include <QApplication>
#include <QRegExp>
#include <QStringList>
#include <QString>
#include <math.h>
#include "unitconversions.h"
#include <iostream>



#ifndef PI
#define PI 3.14159265358979323846264338327950288419716939937510
#endif
#ifndef EARTH_RAD
#define EARTH_RAD 6367500.0 // meters at sea level : source wolfram alpha
#endif
#ifndef EARTH_RAD_FEET
#define EARTH_RAD_FEET 20891000.0 // feet at sea level : source wolfram alpha
#endif
#ifndef EARTH_RAD_MILES
#define EARTH_RAD_MILES 3956.6 // miles at sea level : source wolfram alpha
#endif

class GPSPoint
{

public:

    GPSPoint();

    static QString gpsHeading(QString lat1, QString lon1, QString lat2, QString lon2);
    static QString gpsDistance(QString lat1, QString lon1, QString lat2, QString lon2);
    static QString gpsDistanceFT(QString lat1, QString lon1, QString lat2, QString lon2);
    static QString gpsDistanceMi(QString lat1, QString lon1, QString lat2, QString lon2);
    static QString gpsAzmiuth(QString lat1, QString lon1, QString alt1, QString lat2, QString lon2, QString alt2);
    QString dmmDD(QString ddmm);
    QString getLat();
    QString getLon();
    QString getEw();
    QString getNs();
    QString getMsl();
    QString getMslFT();
    QString getHdop();
    QString getUtc();
    QString getSpd();
    QString getHead();
    QString getDate();
    QString getBaroAlt();
    QString getBaroAltFT();
    QString getAscent();
    QString getAscentFM();
    QString getPlat();
    QString getPlon();
    QString getPacket();
    QString getIntTemp();
    QString getIntTempF();
    QString getExtTemp();
    QString getStatus();
    QString getCmdAck(int index);
    QString getRocketStatus(int index);
    QString getAeroStatus(int index);
    QString getStatusLine();
    bool getValid();
    bool getArm();
    bool getDrogue();
    bool getFire();
    void parseGPRMC(QString data);
    void parseGPGGA(QString data);
    void parseGPTXT(QString data);
    void parseAeroG0(QString data);
    void parseAeroG1(QString data);
    void parseAeroG6(QString data);
    void parseAeroG12(QString data);
    void parseAeroG13(QString data);
    void parseRocketG(QString data);
    void parseRocketR(QString data);
    void parseData(QString data);

private:

    QString latitude, ns, longitude, ew, msl, hdop, utc, spd, head, date, baro, intemp, intempF, extemp, ascent, plat, plon;
    QString packet, status, mslFT, ascentFM, baroFT, cmdAck, statusText;
    QStringList cmdAckList, rocketStatusList, aeroStatusList;
    bool valid, arm, drogue, fire;

    void setArm();
    void setDrogue();
    void setFire();
    void setDisarm();

};

#endif // GPSPOINT_H
