#include "pluginsmanager.h"
#include "widgetfile.h"
#include "widgettextedit.h"
#include "filemanager.h"
#include "plugins_interfaces/helperinterface.h"

#include <QGuiApplication>
#include <QDir>
#include <QPluginLoader>
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QDialog>
#include <QMetaObject>
#include <QMetaClassInfo>


#include <QtQuick/QQuickView>


PluginsManager PluginsManager::Instance;

PluginsManager::PluginsManager()
{
}

void PluginsManager::test()
{

    QQuickView * view = new QQuickView();
    view->setSource(QUrl::fromLocalFile("/Users/quentinbramas/Projects/texiteasy/texiteasy-repository/plugins/test.qml"));
    view->show();
    //QQuickItem *object = view.rootObject();
}

void PluginsManager::loadPlugins()
{
    test();
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

#ifndef LIB_DEPLOY
    pluginsDir.cdUp();
    pluginsDir.cd("plugins");
#endif

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
    /*QAction * action = qobject_cast<QAction*>(sender());
    if(!action || !_helpers.contains(action->text()))
    {
        return;
    }
    WidgetFile * currentFile = FileManager::Instance.currentWidgetFile();
    if(!currentFile)
    {
        return;
    }
    _helpers.value(action->text())->exec(currentFile->widgetTextEdit()->textCursor());*/

    /*
    QAction * action = qobject_cast<QAction*>(sender());
    if(!action || !_pythonHelpers.contains(action->property("pythonClassName").toString()))
    {
        return;
    }
    WidgetFile * currentFile = FileManager::Instance.currentWidgetFile();
    if(!currentFile)
    {
        return;
    }
    PythonHelper helper = _pythonHelpers.value(action->property("pythonClassName").toString());

    PyObject* cursorObj = _sipApi->api_convert_from_type((void*)(new QTextCursor(currentFile->widgetTextEdit()->textCursor())), _sipApi->api_find_type("QTextCursor"), NULL);

    PyObject_CallMethodObjArgs(helper.pyObject, PyString_FromString("setTextCursor"), cursorObj, NULL);

    helper.dialog->exec();*/
}

void PluginsManager::createMenu(QMenu * parent)
{
    /*
    foreach(HelperInterface* helper, _helpers)
    {
        parent->addAction(helper->name(), this, SLOT(callHelper()));
    }
    foreach(const PythonHelper &helper, _pythonHelpers)
    {
        QAction *a = new QAction(helper.dialog->windowTitle(), parent);
        a->setProperty("pythonClassName", helper.className);
        connect(a, SIGNAL(triggered()), this, SLOT(callHelper()));
        parent->addAction(a);
    }*/

}

