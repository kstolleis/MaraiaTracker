#include "gpspoint.h"

GPSPoint::GPSPoint(){
    latitude = "##.####";
    ns = "#";
    longitude = "###.####";
    ew = "#";
    msl = "####.#";
    hdop = "#.#";
    utc = "##:##:##";
    spd = "###.#";
    head = "###.#";
    date = "######";
    baro = "####.#";
    baroFT = "####.#";
    intemp = "##.#";
    intempF = "##.#";
    extemp = "##.#";
    ascent = "####.#";
    plat = "##.####";
    plon = "##.####";
    packet = "###";
    status = "-1";
    statusText = "###############";
    mslFT = "######.#";
    ascentFM = "####.#";
    valid = false;
    arm = false;
    drogue = false;
    fire = false;
    cmdAck = "X";
    cmdAckList << "Safe" << "Armed" << "Deploy";
    rocketStatusList << "Initializing" << "Awaiting Launch" << "Ascending" << "Apogee" << "Descending" << "Landed" << "Awaiting Recovery";
    aeroStatusList << "System Disarmed" << "Manual System Arm" << "Manual Drogue Chute Deploy" << "Manual Main Chute Deploy" << "Auto System Arm" << "Auto Drogue Chute Deploy" << "Auto Main Chute Deploy";
}

void GPSPoint::parseData(QString data){    
    if(data.startsWith("$GPTXT")){
        parseGPTXT(data);
        return;
    }
    if(data.startsWith("$GPRMC")){
        parseGPRMC(data);
        return;
    }
    if(data.startsWith("$GPGGA")){
        parseGPGGA(data);
        return;
    }
    if(data.startsWith("G0,")){
        parseAeroG0(data);
        return;
    }
    if(data.startsWith("G1,")){
        parseAeroG1(data);
        return;
    }
    if(data.startsWith("G6,")){
        parseAeroG6(data);
        return;
    }
    if(data.startsWith("G12,")){
        parseAeroG12(data);
        return;
    }
    if(data.startsWith("G13,")){
        parseAeroG13(data);
        return;
    }
    if(data.startsWith("GR,")){
        parseRocketG(data);
    }
    if(data.startsWith("RR,")){
        parseRocketR(data);
    }
}

void GPSPoint::parseGPRMC(QString data){
    QStringList tempList = data.split(QRegExp(","));
    if(tempList.length() > 9){
        if(!tempList.at(1).isEmpty()){
            utc = tempList.at(1);
            utc.truncate(6);
            utc.insert(2, ":");
            utc.insert(5, ":");
        }
        if(tempList.at(2) == "A"){
            valid = true;
        } else {
            valid = false;
        }
        if(valid){
            if(!tempList.at(3).isEmpty()){
                latitude = GPSPoint::dmmDD(tempList.at(3));
            }
            ns = tempList.at(4);
            if("S" == ns){
                latitude = "-" + latitude;
            }
            if(!tempList.at(5).isEmpty()){
                longitude = GPSPoint::dmmDD(tempList.at(5));
            }
            ew = tempList.at(6);
            if("W" == ew){
                longitude = "-" + longitude;
            }
            spd = tempList.at(7);
            head = tempList.at(8);
            date = tempList.at(9);
        }
    }
}

void GPSPoint::parseGPGGA(QString data){
    QStringList tempList = data.split(QRegExp(","));
    if(tempList.length() > 9){
        if(!tempList.at(1).isEmpty()){
            utc = tempList.at(1);
            utc.truncate(6);
            utc.insert(2, ":");
            utc.insert(5, ":");
        }
        if(!tempList.at(2).isEmpty()){
            latitude = GPSPoint::dmmDD(tempList.at(2));
        }
        ns = tempList.at(3);
        if(ns == "S"){
            latitude = "-" + latitude;
        }
        if(!tempList.at(4).isEmpty()){
            longitude = GPSPoint::dmmDD(tempList.at(4));
        }
        ew = tempList.at(5);
        if(ew == "W"){
            longitude = "-" + longitude;
        }
        hdop = tempList.at(8);
        if(!tempList.at(9).isEmpty()){
            msl = tempList.at(9);
            mslFT = UnitConversions::meterToFt(msl);
        }
    }
}

void GPSPoint::parseGPTXT(QString data){
    QStringList tempList = data.split(QRegExp(";"));
    if(data.contains(QRegExp("axis"))){
        return;
    }
    if(data.contains(QRegExp("#")) && tempList.length() > 2){
        packet = tempList.at(2);
        return;
    }
    if(data.contains(QRegExp("crit")) && tempList.length() > 5){
        intemp = tempList.at(2);
        intempF = UnitConversions::CtoF(intemp);
        extemp = tempList.at(3);
        baro = tempList.at(4);
        baroFT = UnitConversions::meterToFt(baro);
        if("0" == tempList.at(5)){
            setDisarm();
        }
        else if("1" == tempList.at(5)){
            setArm();
        }
        else if("2" == tempList.at(5)){
            setDrogue();
        }
        return;
    }
    if(data.contains(QRegExp("brst")) && tempList.length() > 6){
        ascent = tempList.at(6);  // 30 second average
        ascentFM = UnitConversions::mpsFPM(ascent);
        return;
    }
    if(data.contains(QRegExp("land")) && tempList.length() > 3){
        plat = tempList.at(2);
        plon = tempList.at(3);
        return;
    }
}

void GPSPoint::parseAeroG0(QString data){
    QStringList tempData = data.split(QRegExp(","));
    if(tempData.length() > 2){
        packet = tempData.at(1);
        status = tempData.at(2);
        if(status.toInt() >= 0 && status.toInt() <= 6){
            statusText = aeroStatusList.at(status.toInt());
        }
        if("0" == status){
            setDisarm();
        }
        if("1" == status || "4" == status){
            setArm();
        }
        if("2" == status || "5" == status){
            setDrogue();
        }
        if("3" == status || "6" == status){
            setFire();
        }
    }
}

void GPSPoint::parseAeroG1(QString data){
    QStringList tempData = data.split(QRegExp(","));
    if(tempData.length() > 4){
        if( "5" == tempData.at(4) ||  "7" == tempData.at(4) || "13" == tempData.at(4) ||
           "15" == tempData.at(4) || "21" == tempData.at(4) || "23" == tempData.at(4) ||
           "29" == tempData.at(4) || "31" == tempData.at(4)){
            valid = true;
        } else {
            valid = false;
        }
        if(valid){
            latitude = tempData.at(1);
            longitude = tempData.at(2);
            latitude.truncate(7);
            longitude.truncate(8);
            msl = tempData.at(3);
            mslFT = UnitConversions::meterToFt(msl);
        }
    }
}

void GPSPoint::parseAeroG6(QString data){
    QStringList tempData = data.split(QRegExp(","));
    if(tempData.length() > 1){
        utc = tempData.at(1);
    }
}

void GPSPoint::parseAeroG12(QString data){
    QStringList tempData = data.split(QRegExp(","));
    if(tempData.length() > 3){
        intempF = tempData.at(1);
        intemp  = tempData.at(2);
     // intempF = UnitConversions::CtoF(intemp);
        baro = tempData.at(3);
        baroFT = UnitConversions::meterToFt(baro);
    }
}

void GPSPoint::parseAeroG13(QString data){
    QStringList tempData = data.split(QRegExp(","));
    if(tempData.length() > 2){
        plat = tempData.at(1);
        plon = tempData.at(2);
        plat.truncate(7);
        plon.truncate(8);
    }
}

void GPSPoint::parseRocketG(QString data){
    QStringList tempData = data.split(QRegExp(","));
    if(tempData.length() > 7){
        utc = tempData.at(1);
        if(tempData.at(2) == "A"){
            valid = true;
        } else {
            valid = false;
        }
        if(valid){
            if(!tempData.at(3).isEmpty()){
                latitude = GPSPoint::dmmDD(tempData.at(3));
            }
            ns = tempData.at(4);
            if("S" == ns){
                latitude = "-" + latitude;
            }
            if(!tempData.at(5).isEmpty()){
                longitude = GPSPoint::dmmDD(tempData.at(5));
            }
            ew = tempData.at(6);
            if("W" == ew){
                longitude = "-" + longitude;
            }
            msl = tempData.at(7);
            mslFT = UnitConversions::meterToFt(msl);
        }
    }
}

void GPSPoint::parseRocketR(QString data){
    QStringList tempData = data.split(QRegExp(","));
    if(tempData.length() > 5){
        cmdAck = tempData.at(1);
        if("0" == cmdAck){
            setDisarm();
        }
        if("1" == cmdAck){
            setArm();
        }
        if("2" == cmdAck){
            setDrogue();
        }
        status = tempData.at(2);
        if(status.toInt() >= 0 && status.toInt() <= 6){
            statusText = rocketStatusList.at(status.toInt());
        }
        packet = tempData.at(3);
        intemp = tempData.at(4);
        intempF = UnitConversions::CtoF(intemp);
        baro = tempData.at(5);
        baroFT = UnitConversions::meterToFt(baro);
    }
}

QString GPSPoint::getLat(){
    return latitude;
}

QString GPSPoint::getLon(){
    return longitude;
}

QString GPSPoint::getEw(){
    return ew;
}

QString GPSPoint::getNs(){
    return ns;
}

QString GPSPoint::getMsl(){
    return msl;
}

QString GPSPoint::getMslFT(){
    return mslFT;
}

QString GPSPoint::getHdop(){
    return hdop;
}

QString GPSPoint::getUtc(){
    return utc;
}

QString GPSPoint::getSpd(){
    return spd;
}

QString GPSPoint::getHead(){
    return head;
}

QString GPSPoint::getDate(){
    return date;
}

bool GPSPoint::getValid(){
    return valid;
}

QString GPSPoint::getBaroAlt(){
    return baro;
}

QString GPSPoint::getBaroAltFT(){
    return baroFT;
}

QString GPSPoint::getAscent(){
    return ascent;
}

QString GPSPoint::getAscentFM(){
    return ascentFM;
}

QString GPSPoint::getPlat(){
    return plat;
}

QString GPSPoint::getPlon(){
    return plon;
}

QString GPSPoint::getPacket(){
    return packet;
}

QString GPSPoint::getIntTemp(){
    return intemp;
}

QString GPSPoint::getIntTempF(){
    return intempF;
}

QString GPSPoint::getExtTemp(){
    return extemp;
}

QString GPSPoint::getStatus(){
    return status;
}

QString GPSPoint::getCmdAck(int index){
    return cmdAckList.at(index);
}

QString GPSPoint::getAeroStatus(int index){
    return aeroStatusList.at(index);
}

QString GPSPoint::getRocketStatus(int index){
    return rocketStatusList.at(index);
}

QString GPSPoint::getStatusLine(){
    return statusText;
}

bool GPSPoint::getArm(){
    return arm;
}

bool GPSPoint::getDrogue(){
    return drogue;
}

bool GPSPoint::getFire(){
    return fire;
}

QString GPSPoint::dmmDD(QString ddmm){
    double temp = ddmm.toDouble();
    double deg = fmod(temp, 100);
    int dec = temp / 100;
    return QString::number(dec + (deg / 60), 'f', 5);
}

QString GPSPoint::gpsDistance(QString lat1, QString lon1, QString lat2, QString lon2){
    double tempLat1 = UnitConversions::degToRad(lat1.toDouble());
    double tempLat2 = UnitConversions::degToRad(lat2.toDouble());
    double theta = gpsHeading(lat1, lon1, lat2, lon2).toDouble();
    return QString::number((tempLat2 - tempLat1) / cos(UnitConversions::degToRad(theta)) * EARTH_RAD, 'f', 1);
}

QString GPSPoint::gpsDistanceFT(QString lat1, QString lon1, QString lat2, QString lon2){
    double tempLat1 = UnitConversions::degToRad(lat1.toDouble());
    double tempLat2 = UnitConversions::degToRad(lat2.toDouble());
    double theta = gpsHeading(lat1, lon1, lat2, lon2).toDouble();
    return QString::number((tempLat2 - tempLat1) / cos(UnitConversions::degToRad(theta)) * EARTH_RAD_FEET, 'f', 1);
}

QString GPSPoint::gpsDistanceMi(QString lat1, QString lon1, QString lat2, QString lon2){
    double tempLat1 = UnitConversions::degToRad(lat1.toDouble());
    double tempLat2 = UnitConversions::degToRad(lat2.toDouble());
    double theta = gpsHeading(lat1, lon1, lat2, lon2).toDouble();
    return QString::number((tempLat2 - tempLat1) / cos(UnitConversions::degToRad(theta)) * EARTH_RAD_MILES, 'f', 3);
}

QString GPSPoint::gpsHeading(QString lat1, QString lon1, QString lat2, QString lon2){
    double tempLat1 = UnitConversions::degToRad(lat1.toDouble());
    double tempLon1 = UnitConversions::degToRad(lon1.toDouble());
    double tempLat2 = UnitConversions::degToRad(lat2.toDouble());
    double tempLon2 = UnitConversions::degToRad(lon2.toDouble());
    double denominator = log((tan((tempLat2 / 2) + (PI / 4))) / (tan((tempLat1 / 2) + (PI / 4))));
    double numerator = (tempLon2 - tempLon1);
    return QString::number(fmod((UnitConversions::radToDeg(atan2(numerator, denominator)) + 360), 360), 'f', 1);
}

QString GPSPoint::gpsAzmiuth(QString lat1, QString lon1, QString alt1, QString lat2, QString lon2, QString alt2){
    double tempAlt1 = alt1.toDouble();
    double tempAlt2 = alt2.toDouble();
    double dist = GPSPoint::gpsDistance(lat1, lon1, lat2, lon2).toDouble();
    return QString::number(UnitConversions::radToDeg(atan((tempAlt2 - tempAlt1) / dist)), 'f', 1);
}

//public static String dmmDMS(String dmm){
//    if(dmm.split(".")[0].length() < 3){
//        dmm = "0" + dmm;
//    }
//    String deg = dmm.substring(0,3);
//    String min = dmm.substring(3,5);
//    String decmin = dmm.substring(5);
//    return deg + "\u00B0 " + min + "' " + Math.round(Double.parseDouble(decmin) * 60) + "\"";
//}

//public static String dmsDD(String dms){
//    String deg = dms.substring(0,3);
//    String min = dms.substring(3,5);
//    String sec = dms.substring(5);
//    double temp = Double.parseDouble(deg) + (FormatUtil.sixDecimals((Double.parseDouble(min) * 60 + Double.parseDouble(sec)) / 3600));
//    return String.valueOf(temp);
//}

void GPSPoint::setArm(){
    arm = true;
}

void GPSPoint::setDrogue(){
    drogue = true;
}

void GPSPoint::setFire(){
    fire = true;
}

void GPSPoint::setDisarm(){
    arm = false;
    drogue = false;
    fire = false;
}
