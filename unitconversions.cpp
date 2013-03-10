#include "unitconversions.h"

UnitConversions::UnitConversions(){
}

double UnitConversions::radToDeg(double rad){
    return rad * (180 / PI);
}

double UnitConversions::degToRad(double deg){
    return deg * (PI / 180);
}

double UnitConversions::ftToMeter(double ft){
    return ft / 3.2808399;
}

double UnitConversions::meterToFt(double meter){
    return meter * 3.2808399;
}

QString UnitConversions::meterToFt(QString meter){
    return QString::number(meterToFt(meter.toDouble()), 'f', 1);
}

double UnitConversions::CtoF(double celsius){
    return celsius * 9 / 5 + 32;
}

QString UnitConversions::CtoF(QString celsius){
    return QString::number(CtoF(celsius.toDouble()), 'f', 1);
}

double UnitConversions::meterToMile(double meter){
    return meterToFt(meter) / 5280;
}

QString UnitConversions::meterToMile(QString meter){
    return QString::number(meterToMile(meter.toDouble()), 'f', 3);
}

double UnitConversions::mpsFPM(double mps){
    return meterToFt(mps) * 60;
}

QString UnitConversions::mpsFPM(QString mps){
    return QString::number(mpsFPM(mps.toDouble()), 'f', 1);
}
