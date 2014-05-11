#include "tabularhelper.h"
#include <QtWidgets/QMessageBox>

TabularHelper::TabularHelper()
{
}

QString TabularHelper::show() const
{
    QMessageBox::information(0, "lol", "looool");
    return "";
}
