#ifndef GRAMMARCHECKER_H
#define GRAMMARCHECKER_H

#include <QString>
#include <QNetworkAccessManager>
#include <QProcess>
class QNetworkReply;


class GrammarChecker : public QObject
{
    Q_OBJECT
public:
    GrammarChecker();
    void check(QString text);

private slots:
    void processFinished(int, QProcess::ExitStatus exitStatus);
private:

    QProcess _process;
};

#endif // GRAMMARCHECKER_H
