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
    QString show() const;
    QString name() const { return trUtf8("Insérer un tableau"); }
};

#endif // TABULARHELPER_H
