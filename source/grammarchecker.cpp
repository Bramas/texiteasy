#include "grammarchecker.h"

#include <QUrl>
#include <QNetworkReply>
#include <QDebug>
#include <QMessageBox>
#include <QDomDocument>
#include <QDomNode>

const QString LangageToolServerAddress = "https://languagetool.org:8081/?language=en-US&disabled=WHITESPACE_RULE&text=";

GrammarChecker::GrammarChecker()
{
    connect(&_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}

void GrammarChecker::check(QString text)
{

    QUrl url(LangageToolServerAddress+text);
/*
#ifdef OS_LINUX
    QByteArray data = url.encodedQuery() ;
#else
    QUrlQuery urlquery;
    url.setQuery(urlquery);
    QByteArray data = url.query().toUtf8();
#endif*/
    QNetworkRequest req(url);
    //req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    _manager.get(req);
}

void GrammarChecker::replyFinished(QNetworkReply* reply)
{
    if(reply->error())
    {
        QMessageBox::information(0, trUtf8("Erreur"), trUtf8("Erreur: ")+reply->errorString());
    }
    else
    {
        //QMessageBox::information(this, trUtf8("Merci"), trUtf8("Merci pour votre message."));
        //qDebug()<<reply->readAll();
        QDomDocument doc;
        doc.setContent(reply->readAll());

        QDomElement docElem = doc.documentElement();
        QDomNode n = docElem.firstChild();
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            n = n.nextSibling();
            if(e.isNull() || e.tagName().compare("error")) {
                continue;
            }
            qDebug()<<e.attribute("fromx")<<"-"<<e.attribute("tox")<<": "<<e.attribute("msg")<<e.attribute("context")<<" ("<<e.attribute("contextoffset")<<","<<e.attribute("offset")<<")";

        }
    }
    this->deleteLater();
}


