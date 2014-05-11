#include "dialogsendfeedback.h"
#include "ui_dialogsendfeedback.h"
#include "configmanager.h"
#include <QDebug>
#include <QUrl>
#include <QNetworkReply>
#include <QMessageBox>

#ifndef OS_LINUX
#include <QUrlQuery>
#endif

DialogSendFeedback::DialogSendFeedback(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSendFeedback)
{
    ui->setupUi(this);
    connect(&_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}

DialogSendFeedback::~DialogSendFeedback()
{
    delete ui;
}

void DialogSendFeedback::accept()
{
    QUrl url("http://texiteasy.com/feedbacks/create");

#ifdef OS_LINUX
    url.addQueryItem("data[Feedback][email]", this->ui->lineEdit->text());
    url.addQueryItem("data[Feedback][message]", this->ui->plainTextEdit->toPlainText());
    url.addQueryItem("data[Feedback][soft_id]", ConfigManager::Instance.softId());
    url.addQueryItem("data[Feedback][system_info]", ConfigManager::Instance.systemInfo());
    url.addQueryItem("data[Feedback][version]", CURRENT_VERSION);
    QByteArray data = url.encodedQuery() ;
#else
    QUrlQuery urlquery;
    urlquery.addQueryItem("data[Feedback][email]", this->ui->lineEdit->text());
    urlquery.addQueryItem("data[Feedback][message]", this->ui->plainTextEdit->toPlainText());
    urlquery.addQueryItem("data[Feedback][soft_id]", ConfigManager::Instance.softId());
    urlquery.addQueryItem("data[Feedback][system_info]", ConfigManager::Instance.systemInfo());
    urlquery.addQueryItem("data[Feedback][version]", CURRENT_VERSION);
    url.setQuery(urlquery);
    QByteArray data = url.query().toUtf8();
#endif
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    _manager.post(req, data);
    QDialog::accept();
}
void DialogSendFeedback::replyFinished(QNetworkReply* reply)
{
    if(reply->error())
    {
        QMessageBox::information(this, trUtf8("Erreur"), trUtf8("Erreur: ")+reply->errorString());
    }else
    {
        QMessageBox::information(this, trUtf8("Merci"), trUtf8("Merci pour votre message."));
    }
    this->deleteLater();
}

