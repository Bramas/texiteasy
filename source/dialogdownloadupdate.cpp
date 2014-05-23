
#ifdef OS_WINDOWS

#include "dialogdownloadupdate.h"
#include "ui_dialogdownloadupdate.h"

#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QDesktopServices>
#include <QProcess>

DialogDownloadUpdate::DialogDownloadUpdate(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDownloadUpdate)
{
    ui->setupUi(this);

    connect(this, SIGNAL(versionDownloaded(QString)), this->ui->label, SLOT(setText(QString)));
    connect(this, SIGNAL(downloadProgress(int)), this->ui->progressBar, SLOT(setValue(int)));
    connect(this, SIGNAL(done()), this, SLOT(onDownloaded()));

    _started = false;
    _isVersionDownloaded = false;
    _isUrlDownloaded = false;
    //connect(&manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(downloadFinished(QNetworkReply*)));
    versionReply = manager.get(QNetworkRequest(QUrl(QString::fromUtf8(LAST_VERSION_URL))));
    urlReply = manager.get(QNetworkRequest(QUrl(QString::fromUtf8(TEXITEASY_UPDATE_FILE_URL))));

    QObject::connect(versionReply, SIGNAL(finished()), this, SLOT(onVersionDownloaded()));
    QObject::connect(urlReply, SIGNAL(finished()), this, SLOT(onUrlDownloaded()));

}

DialogDownloadUpdate::~DialogDownloadUpdate()
{
    delete ui;
}

void DialogDownloadUpdate::onDownloaded()
{
    qDebug()<<"[DownloadUpdate] launch : "<<QString("D:/Projects/texiteasy/Installer-i")+
              "/texiteasy_deploy.exe "+
              this->filename()+" looooooool.exe";
    QProcess * p = new QProcess();
    //connect(p, SIGNAL(finished(int)), this, SLOT(onFinished(int)));
    p->start(QString("D:/Projects/texiteasy/Installer-i")+
             "/texiteasy_deploy.exe "+
             this->filename()+" looooooool.exe");



    //QDesktopServices::openUrl(QUrl("file:///"+filename(), QUrl::TolerantMode));


    this->close();
}

void DialogDownloadUpdate::onFinished(int)
{
    this->close();
}


void DialogDownloadUpdate::onVersionDownloaded() {
    _mutex.lock();
    _version = versionReply->readAll();
    qDebug()<<"[DownloadUpdate] Version : "<<_version;
    _isVersionDownloaded = true;
    _mutex.unlock();
    emit versionDownloaded("Texiteasy "+_version);
    QTimer::singleShot(1, this, SLOT(download()));
}
void DialogDownloadUpdate::onUrlDownloaded() {
    _mutex.lock();
    _url = urlReply->header(QNetworkRequest::LocationHeader).toUrl();
    qDebug()<<"[DownloadUpdate] Url : "<<_url;
    _isUrlDownloaded = true;
    _mutex.unlock();
    QTimer::singleShot(1, this, SLOT(download()));
}

void DialogDownloadUpdate::onDataDownloaded() {
    qDebug()<<"[DownloadUpdate] Finished";
    if(dataReply->error())
    {
        qDebug()<<"[DownloadUpdate] With Error";
        return;
    }
    const QByteArray sdata = dataReply->readAll();
    QFile localFile(filename());
    if (!localFile.open(QIODevice::WriteOnly))
        return;
    localFile.write(sdata);
    localFile.close();

    qDebug()<<"[DownloadUpdate] Saved : "<<filename();
    emit done();
}

void DialogDownloadUpdate::download() {
    _mutex.lock();
    if(!_isVersionDownloaded || !_isUrlDownloaded)
    {
        _mutex.unlock();
        return;
    }
    if(_started)
    {
        _mutex.unlock();
        return;
    }
    _started = true;
    _mutex.unlock();

#ifdef OS_WINDOWS
    _filename = "texiteasy_"+_version+".zip";
#else
#ifdef OS_MAC
    _filename = "texiteasy_"+_version+".dmg";
#else
#ifdef OS_LINUX
    _filename = "texiteasy_"+_version+".deb";
#endif
#endif
#endif
    qDebug()<<"[DownloadUpdate] request download "<<_url<<" in "<<_filename;
    if(QFile::exists(filename()))
    {
        emit done();
        return;
    }

    QNetworkRequest request(_url);
    dataReply = manager.get(request);
    QObject::connect(dataReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(onDownloadProgress(qint64,qint64)));
    QObject::connect(dataReply, SIGNAL(finished()), this, SLOT(onDataDownloaded()));
    QObject::connect(dataReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
}

void DialogDownloadUpdate::onDownloadProgress(qint64 recieved, qint64 total) {
    qDebug() << "[DownloadUpdate] " << recieved << "/" << total;
    if(total)
    {
        qint64 p = recieved * 100;
        p /= total;
        emit downloadProgress(p);
    }
}
void DialogDownloadUpdate::onError(QNetworkReply::NetworkError e) {
    qDebug() << "[DownloadUpdate] Error : " << e;
    qDebug() << dataReply->errorString();
}

#endif
