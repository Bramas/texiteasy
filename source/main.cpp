/***************************************************************************
 *   copyright       : (C) 2013 by Quentin BRAMAS                          *
 *   http://texiteasy.com                                                  *
 *                                                                         *
 *   This file is part of texiteasy.                                       *
 *                                                                         *
 *   texiteasy is free software: you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   texiteasy is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with texiteasy.  If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                         *
 ***************************************************************************/

/**
 * @file main.cpp
 * @brief TexitEasy main c file
 * @author Quentin BRAMAS
 *
 *
 */

#include "mainwindow.h"
#include "application.h"
#include "configmanager.h"
#include "filemanager.h"
#include "macroengine.h"
#include "updatechecker.h"
#include "dialogdownloadupdate.h"
#include <QSettings>
#include <QFontDatabase>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QProcess>
#include <QMessageBox>

void upgrade()
{
#ifdef OS_WINDOWS
    /*
    QString command = "";
    QDir d = QDir(QApplication::applicationDirPath());
    d.cdUp();
    if(d.dirName().contains("program", Qt::CaseInsensitive))
    {
        switch(QSysInfo::windowsVersion())
        {
        case QSysInfo::WV_NT:
        case QSysInfo::WV_2000:
        case QSysInfo::WV_XP:
        case QSysInfo::WV_2003:
            command = "texiteasy_upgrade.exe "+ConfigManager::Instance.updateFiles();
            break;
        default:
            command = "elevate texiteasy_upgrade.exe "+ConfigManager::Instance.updateFiles();
            break;
        }
    }
    else
    {
        command = "elevate texiteasy_upgrade.exe "+ConfigManager::Instance.updateFiles();
    }



    qDebug()<<"[main.c] launch : "<<command;
    QProcess * p = new QProcess();
    p->start(command);*/
#endif
}

int main(int argc, char *argv[])
{
#ifdef LIB_DEPLOY
#ifdef OS_MAC
     QDir dir(QFileInfo(argv[0]).path());  // e.g. appdir/Contents/MacOS
     dir.cdUp();
     dir.cd("PlugIns");  // e.g. appdir/Contents/PlugIns
     QCoreApplication::setLibraryPaths(QStringList(dir.absolutePath()));
#endif
#ifdef OS_WINDOWS
     QDir dir(QFileInfo(argv[0]).path());  // e.g. appdir
     dir.cd("PlugIns");  // e.g. appdir/PlugIns
     QCoreApplication::setLibraryPaths(QStringList(dir.absolutePath()));
#endif
#endif
     Application a("TexitEasy",argc, argv);

     QStringList args = QCoreApplication::arguments();

    if ( a.isRunning() && !args.contains("-n") && !args.contains("--new-window"))
    {
        QString msg;
        msg = args.join("#!#");
        a.sendMessage( msg );
        return 0;
    }


    QFontDatabase::addApplicationFont(":/data/fonts/consola.ttf");
    QFontDatabase::addApplicationFont(":/data/fonts/consolab.ttf");
    QFontDatabase::addApplicationFont(":/data/fonts/consolai.ttf");
    QFontDatabase::addApplicationFont(":/data/fonts/consolaz.ttf");

    qDebug()<<QString("Start ")+APPLICATION_NAME+QString(" version ")+CURRENT_VERSION;
    ConfigManager::Instance.init(QFileInfo(QString::fromLocal8Bit(argv[0])).absolutePath());

#if QT_VERSION >= 0x050000
    ConfigManager::Instance.setDevicePixelRatio(a.devicePixelRatio());
#else
    ConfigManager::Instance.setDevicePixelRatio(1);
#endif

    if(!ConfigManager::Instance.updateFiles().isEmpty())
    {
        upgrade();
        return 0;
    }

    FileManager::Instance.init();
    MacroEngine::Instance.init();
    MainWindow w;
    w.show();
    a.connect(&a, SIGNAL(requestOpenFile(QString)),
                    &w, SLOT(open(QString)));
    a.connect(&a, SIGNAL(messageReceived(const QString &) ),
                    &w, SLOT(onOtherInstanceMessage(const QString &)));

    if(argc > 1)
    {
        QString filename = QString::fromLocal8Bit(argv[1]);
        w.open(filename);
    }

    UpdateChecker * updateChecker = new UpdateChecker(&w);

    int returnCode = a.exec();
    if(returnCode == CODE_INSTALL_AND_RESTART)
    {
        upgrade();
        return 0;
    }
    return returnCode;
}
