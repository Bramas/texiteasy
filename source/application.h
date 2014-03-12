#ifndef APPLICATION_H
#define APPLICATION_H

#include <QString>

#include "singleapp/qtsingleapplication.h"

class Application : public QtSingleApplication
{
    Q_OBJECT
public:
    explicit Application(const QString &id, int & argc, char ** argv );

signals:
    void requestOpenFile(QString filename);
public slots:

protected:
#ifdef OS_MAC
    bool event(QEvent *);
public slots:
    void sendRequestFile();
private:
    QString _filename;
#endif

};

#endif // APPLICATION_H
