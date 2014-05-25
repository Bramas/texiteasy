#include "updatechecker.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include <QString>
#include <QMessageBox>
#include <QApplication>
#include <QTimer>
#include <QProcess>
#include <stdio.h>
#include <iostream>
#include <windows.h>
#include "configmanager.h"
#ifdef OS_WINDOWS
    #include "dialogdownloadupdate.h"
#endif

QString UpdateChecker::_version = "";

UpdateChecker::UpdateChecker(QWidget * parent) : _parent(parent)
{
    connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(onFinished(QNetworkReply*)));

    QNetworkRequest request;
    request.setUrl(QUrl(LAST_VERSION_URL));
    request.setHeader(QNetworkRequest::UserAgentHeader, QString(QString("TexitEasy ")+CURRENT_VERSION));
    request.setRawHeader("texiteasy-soft-id", ConfigManager::Instance.softId().toLatin1());
    this->get(request);
}

void UpdateChecker::proposeUpdateDialog(QWidget * parent)
{

#ifdef OS_WINDOWS
       QMessageBox proposeUpdateDialog(trUtf8("Nouvelle version"),
                                       trUtf8("Un nouvelle version est disponnible.")+QString(" [<a href='http://texiteasy.com'>?</a>]<br><br>")+trUtf8("Votre version: ")+CURRENT_VERSION+QString("<br>")+trUtf8("Nouvelle version:")+_version,
                                       QMessageBox::Information,
                                       QMessageBox::Yes | QMessageBox::Escape,
                                       QMessageBox::No,
                                       QMessageBox::Cancel | QMessageBox::Default,
            parent, Qt::Sheet);

        proposeUpdateDialog.setButtonText(QMessageBox::Cancel,
              trUtf8("Mettre à jour"));
        proposeUpdateDialog.setButtonText(QMessageBox::No,
            trUtf8("Ne plus me prevenir de cette mise à jour"));
        proposeUpdateDialog.setButtonText(QMessageBox::Yes,
            trUtf8("La prochaine fois"));

        int rep = proposeUpdateDialog.exec();
        switch(rep)
        {
        case QMessageBox::No:
           rep = 1; break;
        case QMessageBox::Yes:
           rep = 0; break;
        case QMessageBox::Cancel:
           rep = 2; break;
        }

#else
        int rep = QMessageBox::information(0, trUtf8("Nouvelle version"), trUtf8("Un nouvelle version est disponnible, vous pouvez la télécharger depuis le site officiel")+
                                           " "+QString("<a href='")+TEXITEASY_UPDATE_WEBSITE+QString("'>texiteasy.com</a><br><br>")+
                                           trUtf8("Votre version: ")+CURRENT_VERSION+QString("<br>")+trUtf8("Nouvelle version:")+_version,
                                                    trUtf8("La prochaine fois"),trUtf8("Ne plus me prevenir de cette mise à jour")
                                           );
#endif
        if(1 == rep)
        {
            //if this answer is "dont remind me"
            ConfigManager::Instance.dontRemindMeThisVersion(_version);
        }
        else
        if(2 == rep)
        {
#ifdef OS_WINDOWS
            //qDebug()<<ConfigManager::Instance.applicationPath()+"/texiteasy_updat.exe";
            //QProcess * p = new QProcess;
            //p->start(ConfigManager::Instance.applicationPath()+"/texiteasy_updat.exe");
            DialogDownloadUpdate * d = new DialogDownloadUpdate(0);
            d->show();
#endif
        }
}

void UpdateChecker::onFinished(QNetworkReply *reply)
{
    _version = reply->readLine();
    _version = _version.trimmed();
    if(reply->error() != QNetworkReply::NoError || _version.isEmpty())
    {
        qDebug()<<"Unable to find the last version : "<<reply->errorString();
        return;
    }
    if(!_version.compare(CURRENT_VERSION)) // if same version
    {
        return;
    }
    if(_version.contains("http://"))
    {
        QStringList a = _version.split(';'); // url;message
        int rep = QMessageBox::information(_parent, trUtf8("Nouvelle version"), a.at(1).arg(a.at(0)));
        return;
    }
    QStringList a = _version.split('.');
    if(a.count()<3) return;
    int a0 = a.at(0).toInt();
    int a1 = a.at(1).toInt();
    int a2 = a.at(2).toInt();
    QStringList b = QString(CURRENT_VERSION).split('.');
    if(b.count()<3) return;
    int b0 = b.at(0).toInt();
    int b1 = b.at(1).toInt();
    int b2 = b.at(2).toInt();
    if(      b0 > a0 ||
            (b0 == a0 && b1 > a1) ||
            (b0 == a0 && b1 == a1 && b2 > a2)) // The local version is a beta that is not yet release officialy
    {
        return;
    }

    qDebug()<<"New version is available : "<<_version;
    // tell the configmanager to send a signal that the version is outdated
    // send it in 1s because every thing is maybe not loaded yet.
    QTimer::singleShot(1000,&ConfigManager::Instance, SLOT(signalVersionIsOutdated()));
#ifdef OS_WINDOWS

    if(QFile("texiteasy_upgrade.log").exists())
    {
        QMessageBox::information(_parent,  trUtf8("Erreur pendant la mise à jour?"), trUtf8("Si une mise à jour ne s'est pas correctement déroulée, il est conseillé de telecharger la dernière version de TexitEasy directement sur le site officielle."));
    }
#endif
    if(ConfigManager::Instance.isThisVersionHaveToBeReminded(_version))
    {
        proposeUpdateDialog(_parent);
    }
}
