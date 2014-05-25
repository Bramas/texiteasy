#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QNetworkAccessManager>
#include <QWidget>

class QNetworkReply;

class UpdateChecker : public QNetworkAccessManager
{
    Q_OBJECT
public:
    UpdateChecker(QWidget * parent);
    static void proposeUpdateDialog(QWidget * parent = 0);

private slots:
    void onFinished(QNetworkReply * reply);

private:
    static QString _version;
    QWidget * _parent;
};

#endif // UPDATECHECKER_H
