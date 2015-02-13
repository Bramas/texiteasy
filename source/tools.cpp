#include "tools.h"

#include <QDebug>
#include <QProcess>
#include <QApplication>

void Tools::Log(QString msg)
{
    //qDebug()<<msg;
}


void Tools::RebootApplication()
{
    QProcess::startDetached(QApplication::applicationFilePath(), QStringList("-n"));
    exit(12);
}
