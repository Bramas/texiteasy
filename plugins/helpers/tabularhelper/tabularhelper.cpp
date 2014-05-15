#include "tabularhelper.h"
#include <QtWidgets/QMessageBox>

TabularHelper::TabularHelper()
{
}

int TabularHelper::exec(const QTextCursor &cursor_c)
{
    QTextCursor cursor(cursor_c);
    cursor.insertText("Insérer un tableau");
    return 0;
}
