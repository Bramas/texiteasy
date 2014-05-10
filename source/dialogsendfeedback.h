#ifndef DIALOGSENDFEEDBACK_H
#define DIALOGSENDFEEDBACK_H

#include <QDialog>
#include <QNetworkAccessManager>

namespace Ui {
class DialogSendFeedback;
}
class QNetworkReply;

class DialogSendFeedback : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSendFeedback(QWidget *parent = 0);
    ~DialogSendFeedback();

public slots:
    void accept();
    void replyFinished(QNetworkReply*);

private:
    Ui::DialogSendFeedback *ui;
    QNetworkAccessManager _manager;
};

#endif // DIALOGSENDFEEDBACK_H
