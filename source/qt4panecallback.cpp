#include "qt4panecallback.h"
#include "widgetfile.h"
#include "widgetstatusbar.h"
#include "ipane.h"
//#if QT_VERSION < QT_VERSION_CHECK(5,0,0)

Qt4PaneCallback::Qt4PaneCallback(QObject *parent) : QObject(parent)
{

}

void Qt4PaneCallback::onButtonClicked(bool)
{
    widget->togglePane(pane);
}
void Qt4PaneCallback::onPaneactionToggled(bool checked)
{
    button->setChecked(checked);
}
//#endif

