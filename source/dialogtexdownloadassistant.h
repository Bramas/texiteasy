#ifndef DIALOGTEXDOWNLOADASSISTANT_H
#define DIALOGTEXDOWNLOADASSISTANT_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QFile>

namespace Ui {
class DialogTexDownloadAssistant;
}

class DialogTexDownloadAssistant : public QDialog
{
    Q_OBJECT

public:
    explicit DialogTexDownloadAssistant(QWidget *parent = 0);
    ~DialogTexDownloadAssistant();
public slots:
    void accept();
private slots:
    void enableOk();
    void onDownloaded();
    void onDownloadProgress(qint64 recieved, qint64 total);
    void rebootApplication();
    void onError(QNetworkReply::NetworkError);

private:
    typedef enum State { UNKNOWN, DOWNLOADING_URL, DOWNLOADING_EXE, WAITING_REBOOT } State;


    Ui::DialogTexDownloadAssistant *ui;
    QNetworkAccessManager  * _manager;
    QNetworkReply * _dataReply;
    State _state;
    QFile * _executable;
};

#endif // DIALOGTEXDOWNLOADASSISTANT_H
