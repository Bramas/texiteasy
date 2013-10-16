#include "updatechecker.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include <QString>
#include <QMessageBox>
#include "configmanager.h"

UpdateChecker::UpdateChecker()
{
    connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(onFinished(QNetworkReply*)));

    QNetworkRequest request;
    request.setUrl(QUrl("https://raw.github.com/Bramas/texiteasy/master/VERSION"));
    request.setRawHeader("User-Agent", QString(QString("TexitEasy ")+CURRENT_VERSION).toLatin1());
    this->get(request);
}

void UpdateChecker::onFinished(QNetworkReply *reply)
{
    QString version = reply->readLine();
    version = version.trimmed();

    if(!version.compare(CURRENT_VERSION)) // if same version
    {
        return;
    }
    //if there is a new version
    if(ConfigManager::Instance.isThisVersionHaveToBeReminded(version))
    {
        if( 1 == QMessageBox::information(0, trUtf8("Nouvelle version"), trUtf8("Un nouvelle version est disponnible, vous pouvez la télécharger depuis le site officiel")+
                                 " "+QString("<a href='http://texiteasy.com'>texiteasy.com</a><br><br>")+
                                 trUtf8("Votre version: ")+CURRENT_VERSION+QString("<br>")+trUtf8("Nouvelle version:")+version,
                                 trUtf8("La prochaine fois"),trUtf8("Ne plus me prevenir de cette mise à jour")
                                 ))
        {
            //if this answer is "dont remind me"
            ConfigManager::Instance.dontRemindMeThisVersion(version);
        }
    }
}
