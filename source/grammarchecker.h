#ifndef GRAMMARCHECKER_H
#define GRAMMARCHECKER_H

#include <QString>
#include <QNetworkAccessManager>
class QNetworkReply;


class GrammarChecker : public QObject
{
    Q_OBJECT
public:
    GrammarChecker();
    void check(QString text);

private slots:
    void replyFinished(QNetworkReply* reply);
private:

    QNetworkAccessManager _manager;
};

#endif // GRAMMARCHECKER_H
