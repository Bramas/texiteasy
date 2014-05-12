#include "pluginsmanager.h"
#include "plugins_interfaces/helperinterface.h"

#include <QGuiApplication>
#include <QDir>
#include <QPluginLoader>
#include <QDebug>
#include <QMenu>
#include <QAction>

PluginsManager PluginsManager::Instance;

PluginsManager::PluginsManager()
{
}

void PluginsManager::loadPlugins()
{
    QDir pluginsDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
#ifndef LIB_DEPLOY
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cd("Plugins");
#endif
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    pluginsDir.cd("plugins");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin) {
            HelperInterface * helper = dynamic_cast<HelperInterface *>(plugin);
            if (helper)
            {
                qDebug()<<"plugin helper : "<<fileName;
                _helpers.insert(helper->name(), helper);
            }
        }
    }
}

void PluginsManager::callHelper()
{
    QAction * action = qobject_cast<QAction*>(sender());
    if(!action || !_helpers.contains(action->text()))
    {
        return;
    }
    _helpers.value(action->text())->show();
}

void PluginsManager::createMenu(QMenu * parent)
{
    foreach(HelperInterface* helper, _helpers)
    {
        parent->addAction(helper->name(), this, SLOT(callHelper()));
    }
}

