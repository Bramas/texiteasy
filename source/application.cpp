#include "application.h"
#include <QFileOpenEvent>
#include <QTimer>
#include <QDebug>

Application::Application(const QString &appId, int & argc, char ** argv ) : QtSingleApplication (appId, argc, argv )
{

}

#ifdef OS_MAC
bool Application::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::FileOpen:
        _filename = static_cast<QFileOpenEvent *>(
                    event)->file();
        QTimer::singleShot(200,this,SLOT(sendRequestFile()));
        return true;
    default:
        return QApplication::event(event);
    }
}
void Application::sendRequestFile()
{
    emit requestOpenFile(_filename);
}

#endif
