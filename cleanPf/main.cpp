
#include <stdio.h>
#include <QProcess>
#include <QFileInfo>
#include <QSettings>
#include <QCoreApplication>
#include <QStandardPaths>
#include "singleapp/qtsinglecoreapplication.h"

int main(int argc, char ** argv)
{

    QString appDir = QFileInfo(argv[0]).absolutePath();
    appDir += "/texiteasyPath.txt";
    FILE * file = fopen(appDir.toLatin1().data(), "r");
    //FILE * log = fopen("C:\\Users\\Wybot\\AppData\\Roaming\\TexitEasy\\log.txt", "w");
    //fwrite(appDir.toLatin1().data(), sizeof(char), appDir.toLatin1().size(), log);
    if(!file)
    {
        //fwrite("Unable to open", sizeof(char), 14, log);
        return 1;
    }
    char start[5000];
    size_t s = fread(start, sizeof(char), 5000, file);
    start[s] = '\0';


    QtSingleCoreApplication a("TexitEasy",argc, argv);

    QProcess* proc = new QProcess();
    QString str(start);
    QStringList args = a.arguments();
    args.pop_front();

    /*for(int i = 1; i < argc; ++i)
    {
        args << argv[i];
    }*/
   if ( a.isRunning() && !args.contains("-n") && !args.contains("--new-window"))
   {
       proc->start(str+"/TexitEasy.exe", args);
       return 0;
   }


    QCoreApplication::setOrganizationName("TexitEasy");
    QCoreApplication::setOrganizationDomain("texiteasy.com");
    QCoreApplication::setApplicationName("TexitEasy");
    QSettings::setDefaultFormat(QSettings::IniFormat);



    //fwrite(start, sizeof(char), s, log);
    QString dataLocation = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    QFile updateFile(dataLocation+"/updateFiles.zip");
    if(updateFile.exists())
    {
        QStringList l;
        //l << str+"/texiteasy_upgrade.exe";
        l << dataLocation+"/updateFiles.zip";
        l << str;
        proc->start(str+"/texiteasy_upgrade.exe", l);
        return 0;
    }
    QFile updateFileExe(dataLocation+"/updateFiles.exe");
    if(updateFileExe.exists())
    {
        QStringList l;
        updateFileExe.rename(dataLocation+"/updateTexitEasy.exe");
        //l << str+"/texiteasy_upgrade.exe";
        l << dataLocation+"/updateTexitEasy.exe";
       // l << str;
        proc->start(str+"/elevate.exe", l);
        return 0;
    }

    proc->start(str+"/TexitEasy.exe", args);

}

