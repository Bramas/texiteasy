#include "application.h"
#include <QFileOpenEvent>

Application::Application(int argc, char *argv[]) :
    QApplication(argc, argv)
{

}

#ifdef OS_MAC
bool Application::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::FileOpen:
        emit requestOpenFile(static_cast<QFileOpenEvent *>(
                 event)->file());
        return true;
    default:
        return QApplication::event(event);
    }
}
#endif
