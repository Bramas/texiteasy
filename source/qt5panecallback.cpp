#include "qt5panecallback.h"
#include "widgetfile.h"
#include "widgetstatusbar.h"
#include "ipane.h"
//#if QT_VERSION < QT_VERSION_CHECK(5,0,0)

Qt5PaneCallback::Qt5PaneCallback(QObject *parent) : QObject(parent)
{

}

void Qt5PaneCallback::onButtonClicked(bool)
{
    widget->togglePane(pane);
}
void Qt5PaneCallback::onPaneactionToggled(bool checked)
{
    button->setChecked(checked);
}
//#endif

