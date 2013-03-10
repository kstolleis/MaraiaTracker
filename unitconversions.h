#ifndef UNITCONVERSIONS_H
#define UNITCONVERSIONS_H

#include <math.h>
#include <QString>

#ifndef PI
#define PI 3.14159265358979323846264338327950288419716939937510
#endif

class UnitConversions
{
public:

    UnitConversions();

    static double radToDeg(double rad);
    static double degToRad(double deg);
    static double meterToFt(double meter);
    static double CtoF(double meter);
    static QString CtoF(QString meter);
    static QString meterToFt(QString meter);
    static double meterToMile(double meter);
    static QString meterToMile(QString meter);
    static double ftToMeter(double ft);
    static double mpsFPM(double mps);
    static QString mpsFPM(QString mps);

};

#endif // UNITCONVERSIONS_H
