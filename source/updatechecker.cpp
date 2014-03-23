#include "updatechecker.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include <QString>
#include <QMessageBox>
#include <QTimer>
#include <QProcess>
#include "configmanager.h"
#ifdef OS_WINDOWS
    #include "dialogdownloadupdate.h"
#endif
UpdateChecker::UpdateChecker()
{
    connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(onFinished(QNetworkReply*)));

    QNetworkRequest request;
    request.setUrl(QUrl(LAST_VERSION_URL));
    request.setRawHeader("User-Agent", QString(QString("TexitEasy ")+CURRENT_VERSION).toLatin1());
    this->get(request);
}

void UpdateChecker::onFinished(QNetworkReply *reply)
{
    QString version = reply->readLine();
    version = version.trimmed();
    if(reply->error() != QNetworkReply::NoError || version.isEmpty())
    {
        qDebug()<<"Unable to find the last version : "<<reply->errorString();
        return;
    }
    if(!version.compare(CURRENT_VERSION)) // if same version
    {
        return;
    }
    QStringList a = version.split('.');
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

    qDebug()<<"New version is available : "<<version;
    // tell the configmanager to send a signal that the version is outdated
    // send it in 1s because every thing is maybe not loaded yet.
    QTimer::singleShot(1000,&ConfigManager::Instance, SLOT(signalVersionIsOutdated()));
    if(ConfigManager::Instance.isThisVersionHaveToBeReminded(version))
    {
#ifdef OS_WINDOWS
        int rep = QMessageBox::information(0, trUtf8("Nouvelle version"), trUtf8("Un nouvelle version est disponnible.")+"<br><br>"+
                                           trUtf8("Votre version: ")+CURRENT_VERSION+QString("<br>")+trUtf8("Nouvelle version:")+version,
                                                    trUtf8("La prochaine fois"),trUtf8("Ne plus me prevenir de cette mise à jour"), trUtf8("Mettre à jour")
                                           );
#else
        int rep = QMessageBox::information(0, trUtf8("Nouvelle version"), trUtf8("Un nouvelle version est disponnible, vous pouvez la télécharger depuis le site officiel")+
                                           " "+QString("<a href='")+TEXITEASY_UPDATE_WEBSITE+QString("'>texiteasy.com</a><br><br>")+
                                           trUtf8("Votre version: ")+CURRENT_VERSION+QString("<br>")+trUtf8("Nouvelle version:")+version,
                                                    trUtf8("La prochaine fois"),trUtf8("Ne plus me prevenir de cette mise à jour")
                                           );
#endif
        if(1 == rep)
        {
            //if this answer is "dont remind me"
            ConfigManager::Instance.dontRemindMeThisVersion(version);
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
}
