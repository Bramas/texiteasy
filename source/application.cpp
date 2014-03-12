#include "application.h"
#include <QFileOpenEvent>
#include <QTimer>
#include <QDebug>
#include "updatechecker.h"

Application::Application(const QString &appId, int & argc, char ** argv ) : QtSingleApplication (appId, argc, argv )
{
    UpdateChecker * updateChecker = new UpdateChecker();
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
