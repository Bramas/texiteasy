#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QString>

class Application : public QApplication
{
    Q_OBJECT
public:
    explicit Application(int argc, char *argv[]);

signals:
    void requestOpenFile(QString filename);
public slots:

protected:
#ifdef OS_MAC
    bool event(QEvent *);
#endif

};

#endif // APPLICATION_H
