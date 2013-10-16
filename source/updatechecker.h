#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QNetworkAccessManager>

class QNetworkReply;

class UpdateChecker : public QNetworkAccessManager
{
    Q_OBJECT
public:
    UpdateChecker();
private slots:
    void onFinished(QNetworkReply * reply);
};

#endif // UPDATECHECKER_H
