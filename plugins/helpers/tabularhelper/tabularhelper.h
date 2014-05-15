#ifndef TABULARHELPER_H
#define TABULARHELPER_H

#include "tabularhelper_global.h"
#include <QObject>
#include "../../../source/plugins_interfaces/helperinterface.h"


class TabularHelper : public QObject, public HelperInterface
{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID HelperInterface_iid FILE "tabularhelper.json")
    Q_INTERFACES(HelperInterface)

public:
    TabularHelper();
    QString name() const { return "Insérer un tableau"; }
    int exec(const QTextCursor &cursor_c);
};

#endif // TABULARHELPER_H
