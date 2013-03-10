#ifndef SERIALLIST_H
#define SERIALLIST_H

#include <sys/types.h>
#include <dirent.h>
#include <QString>
#include <QStringList>
#include <QTextStream>

class SerialList {

public:

    static QStringList getSerialPorts();
    static void printPortList();

private:

};

#endif // SERIALLIST_H
