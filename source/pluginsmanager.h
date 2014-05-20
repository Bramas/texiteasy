#ifndef PLUGINSMANAGER_H
#define PLUGINSMANAGER_H

#include <QMap>
#include <QString>
#include <QObject>

class HelperInterface;
class QMenu;
class QDialog;


class PluginsManager : public QObject
{
    Q_OBJECT
public:
    static PluginsManager Instance;
    void loadPlugins();
    void createMenu(QMenu * parent);
    void test();

public slots:
    void callHelper();

private:

    PluginsManager();


    QMap<QString, HelperInterface*> _helpers;
    //QMap<QString, PythonHelper> _qmlHelpers;
};

#endif // PLUGINSMANAGER_H
