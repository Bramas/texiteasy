#include "download.h"
#include <QFile>
#include <QDebug>
#include <QTimer>



Download::Download()
    : QObject(0)
{
    _started = false;
    _isVersionDownloaded = false;
    _isUrlDownloaded = false;
    //connect(&manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(downloadFinished(QNetworkReply*)));
    versionReply = manager.get(QNetworkRequest(QUrl(QString::fromUtf8(LAST_VERSION_URL))));
    urlReply = manager.get(QNetworkRequest(QUrl(QString::fromUtf8(TEXITEASY_UPDATE_WEBSITE))));

    QObject::connect(versionReply, SIGNAL(finished()), this, SLOT(onVersionDownloaded()));
    QObject::connect(urlReply, SIGNAL(finished()), this, SLOT(onUrlDownloaded()));

}

Download::~Download()
{
}

void Download::onVersionDownloaded() {
    _mutex.lock();
    _version = versionReply->readAll();
    qDebug()<<"Version : "<<_version;
    _isVersionDownloaded = true;
    _mutex.unlock();
    emit versionDownloaded("Texiteasy "+_version);
    QTimer::singleShot(1, this, SLOT(download()));
}
void Download::onUrlDownloaded() {
    _mutex.lock();
    _url = urlReply->header(QNetworkRequest::LocationHeader).toUrl();
    qDebug()<<"Url : "<<_url;
    _isUrlDownloaded = true;
    _mutex.unlock();
    QTimer::singleShot(1, this, SLOT(download()));
}

void Download::onDataDownloaded() {
    qDebug()<<"Finished";
    if(dataReply->error())
    {
        qDebug()<<"With Error";
        return;
    }
    const QByteArray sdata = dataReply->readAll();
    QFile localFile(filename());
    if (!localFile.open(QIODevice::WriteOnly))
        return;
    localFile.write(sdata);
    localFile.close();

    qDebug()<<"Saved : "<<filename();
    emit done();
}

void Download::download() {
    _mutex.lock();
    if(_started)
    {
        _mutex.unlock();
        return;
    }
    _started = true;
    _mutex.unlock();

#ifdef OS_WINDOWS
    _filename = "texiteasy_"+_version+".exe";
#else
#ifdef OS_MAC
    _filename = "texiteasy_"+_version+".dmg";
#else
#ifdef OS_LINUX
    _filename = "texiteasy_"+_version+".deb";
#endif
#endif
#endif

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

void Download::onDownloadProgress(qint64 recieved, qint64 total) {
    qDebug() << recieved << "/" << total;
    if(total)
    {
        qint64 p = recieved * 100;
        p /= total;
        emit downloadProgress(p);
    }
}
void Download::onError(QNetworkReply::NetworkError e) {
    qDebug() << "Error : " << e;
    qDebug() << dataReply->errorString();
}

