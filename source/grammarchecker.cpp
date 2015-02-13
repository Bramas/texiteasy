#include "grammarchecker.h"

#include <QUrl>
#include <QNetworkReply>
#include <QDebug>
#include <QMessageBox>
#include <QDomDocument>
#include <QDomNode>


GrammarChecker::GrammarChecker()
{
    connect(&_process, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(processFinished(int,QProcess::ExitStatus)));
}

void GrammarChecker::check(QString text)
{
    qDebug()<<"Check Grammar:";
    qDebug()<<text;
    QString command = "java -jar \"/Users/quentinbramas/Projects/texiteasy/LanguageTool-2.8/languagetool-commandline.jar\" -l en-US --api -d WHITESPACE_RULE";
    _process.start(command);
    _process.waitForReadyRead(3000);
    qDebug()<<"----------- READY READ:";
    _process.write(text.toUtf8());
    _process.write("\n\n");
    _process.kill();
}

void GrammarChecker::processFinished(int, QProcess::ExitStatus exitStatus)
{
    QString output = _process.readAll();
    qDebug()<<output;
    if(exitStatus == QProcess::CrashExit && false)
    {
        QMessageBox::information(0, trUtf8("Erreur"), trUtf8("Erreur: ")+_process.errorString());
    }
    else
    {
        QDomDocument doc;
        doc.setContent(output);

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


