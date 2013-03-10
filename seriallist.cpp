#include "seriallist.h"

QStringList serialComList;
QTextStream out(stdout);

QStringList SerialList::getSerialPorts() {

    serialComList.clear();

    DIR *dirPointer;
    struct dirent *listPointer;

    const char *checkString = "tty.u";
    const char *tempString = "";
    const char *dirFileName = "";

    dirPointer = opendir("/dev");

    if (dirPointer != NULL) {
        while ((listPointer = readdir(dirPointer))){
            dirFileName = listPointer->d_name;
            tempString = strstr(dirFileName, checkString);
            if(tempString != NULL){
                serialComList << QString(tempString);
            }
        }
        closedir(dirPointer);
    } else {
        perror("Couldn't open the directory");
    }

    return serialComList;

}

void SerialList::printPortList(){
        for(int i = 0; i < serialComList.length(); i++){
            out << serialComList.at(i) << "\n";
        }
}

