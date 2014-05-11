#ifndef PLUGINSMANAGER_H
#define PLUGINSMANAGER_H

#include <QMap>
#include <QString>
#include <QObject>

class HelperInterface;
class QMenu;

class PluginsManager : public QObject
{
    Q_OBJECT
public:
    static PluginsManager Instance;
    void loadPlugins();
    void createMenu(QMenu * parent);

public slots:
    void callHelper();

private:

    PluginsManager();

    QMap<QString, HelperInterface*> _helpers;
};

#endif // PLUGINSMANAGER_H
