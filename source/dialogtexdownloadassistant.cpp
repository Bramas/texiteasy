#include "dialogtexdownloadassistant.h"
#include "ui_dialogtexdownloadassistant.h"
#include "configmanager.h"
#include "tools.h"

#include <QPushButton>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QProcess>

DialogTexDownloadAssistant::DialogTexDownloadAssistant(QWidget *parent) :
    QDialog(parent),
    _executable(0),
    ui(new Ui::DialogTexDownloadAssistant)
{
    _state = UNKNOWN;
    ui->setupUi(this);
    this->ui->stackedWidget->setCurrentIndex(0);
    this->ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    connect(this->ui->radioButton_ignore, SIGNAL(clicked()), this, SLOT(enableOk()));
    connect(this->ui->radioButton_download, SIGNAL(clicked()), this, SLOT(enableOk()));
    connect(this->ui->radioButton_setPath, SIGNAL(clicked()), this, SLOT(enableOk()));
    connect(this->ui->pushButton_reboot, SIGNAL(clicked()), this, SLOT(rebootApplication()));
    //connect(this, SIGNAL(accepted()), this, SLOT(onAccept()));
}

DialogTexDownloadAssistant::~DialogTexDownloadAssistant()
{
    delete ui;
    if(_executable)
    {
        delete _executable;
    }
}
void DialogTexDownloadAssistant::rebootApplication()
{
     Tools::RebootApplication();
}

void DialogTexDownloadAssistant::enableOk()
{
    this->ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}


void DialogTexDownloadAssistant::accept()
{
    this->ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    if(this->ui->radioButton_ignore->isChecked())
    {
        qDebug()<<"ignore ";
        QDialog::accept();
        return;
    }
    if(this->ui->radioButton_setPath->isChecked())
    {
        qDebug()<<"set path ";
        QDialog::accept();
        return;
    }
    if(!this->ui->radioButton_download->isChecked())
    {
        qDebug()<<"not download ";
        QDialog::accept();
        return;
    }
    QDir tmpDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation));
    _executable = new QFile(tmpDir.absoluteFilePath("texiteasytmp-latex-installer.exe"));
    if(!_executable->open(QFile::WriteOnly))
    {
        qDebug()<<"unable to create tmp file "<<_executable->fileName();
        return;
    }
    _state = DOWNLOADING_URL;
    _manager = new QNetworkAccessManager(this);
    _dataReply = _manager->get(QNetworkRequest(QUrl(QString::fromUtf8(LATEX_URL))));
    connect(_dataReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    connect(_dataReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(onDownloadProgress(qint64,qint64)));
    connect(_dataReply, SIGNAL(finished()), this, SLOT(onDownloaded()));
    this->ui->progressBar->setValue(0);
    this->ui->stackedWidget->setCurrentIndex(1);
}
void DialogTexDownloadAssistant::onError(QNetworkReply::NetworkError error)
{
    qDebug()<<"Error "<<error;
}
void DialogTexDownloadAssistant::onDownloadProgress(qint64 recieved, qint64 total)
{
    if(total > 10000) // So that we are sure we are downloading the big executable
    {
        this->ui->progressBar->setValue(100.0*recieved/total);
    }
}
void DialogTexDownloadAssistant::onDownloaded()
{
    if(_dataReply->attribute(QNetworkRequest::RedirectionTargetAttribute).isValid())
    {
        QUrl url = _dataReply->url().resolved(_dataReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl());
        qDebug()<<"redirect to "<<url;
        _dataReply = _manager->get(QNetworkRequest(url));
        connect(_dataReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
        connect(_dataReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(onDownloadProgress(qint64,qint64)));
        connect(_dataReply, SIGNAL(finished()), this, SLOT(onDownloaded()));
        this->ui->label_filename->setText(trUtf8("Téléchargement de %1").arg("\""+url.fileName()+"\""));
        return;
    }
    if(_state == DOWNLOADING_URL)
    {
        QString url = _dataReply->readAll();
        qDebug()<<"URL "<<url;
        if(QRegExp("^(https?:\\/\\/)?([\\da-zA-Z\\.-]+)\.([a-zA-Z\\.]{2,6})([\\/\\w\\.-]*)*\\/?$").exactMatch(url))
        {
            _state = DOWNLOADING_EXE;
            _dataReply = _manager->get(QNetworkRequest(QUrl(url)));
            connect(_dataReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
            connect(_dataReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(onDownloadProgress(qint64,qint64)));
            connect(_dataReply, SIGNAL(finished()), this, SLOT(onDownloaded()));
        }
        else
        {
            qDebug()<<url<<" is not an URL";
        }
        return;
    }
    if(_state == DOWNLOADING_EXE)
    {
        qDebug()<<"file downloaded";
        if(!_executable->isOpen())
        {
            qDebug()<<_executable->fileName()<<" is no longueur open";
            return;
        }
        _executable->write(_dataReply->readAll());
        _executable->close();
        qDebug()<<"start "<<_executable->fileName();
        QDesktopServices::openUrl(QUrl("file:///"+_executable->fileName(), QUrl::TolerantMode));
        this->ui->buttonBox->setVisible(false);
        this->ui->stackedWidget->setCurrentIndex(2);
        _state = WAITING_REBOOT;
        return;
    }
}
