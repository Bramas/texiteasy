#include "application.h"
#include <QFileOpenEvent>
#include "updatechecker.h"

Application::Application(int argc, char *argv[]) :
    QApplication(argc, argv)
{

    UpdateChecker * updateChecker = new UpdateChecker();
}

#ifdef OS_MAC
bool Application::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::FileOpen:
        if(!static_cast<QFileOpenEvent *>(
                    event)->file().isEmpty())
        {
            emit requestOpenFile(static_cast<QFileOpenEvent *>(
                 event)->file());
        }
        return true;
    default:
        return QApplication::event(event);
    }
}
#endif
