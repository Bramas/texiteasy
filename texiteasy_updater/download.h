#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QMutex>
#include <QSemaphore>
#include <QStandardPaths>

class Download : public QObject
{
    Q_OBJECT
public:
    explicit Download();
    ~Download();
    QString filename() { return path() + "/" + _filename; }
    QString path() { return QStandardPaths::writableLocation(QStandardPaths::TempLocation); }

    QString version() { return _version; }

private:
    QNetworkAccessManager manager;

signals:
    void done();
    void downloadProgress(int);
    void versionDownloaded(QString);
public slots:
    void onError(QNetworkReply::NetworkError);
    void onDataDownloaded();
    void onDownloadProgress(qint64 recieved, qint64 total);
    void onVersionDownloaded();
    void onUrlDownloaded();
private slots:
    void download();
private:


    QNetworkReply * versionReply;
    QNetworkReply * urlReply;
    QNetworkReply * dataReply;
    QUrl _url;
    QString _version;
    bool _started;
    bool _isVersionDownloaded;
    bool _isUrlDownloaded;
    QString _filename;
    QMutex _mutex;
};
#endif // DOWNLOAD_H
