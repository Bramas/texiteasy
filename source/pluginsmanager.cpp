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
//#include "qpycore_public_api.h"


#include <python.h>
#include <sip.h>


PluginsManager PluginsManager::Instance;

PluginsManager::PluginsManager()
{
}
typedef const QMetaObject*  (*GetQMetaObjectFunction)(PyTypeObject* type);

void PluginsManager::test()
{
    Py_Initialize();

    qDebug()<<"version "<<Py_GetVersion()<<endl;

    FILE *fin = fopen("/Users/quentinbramas/Projects/texiteasy/texiteasy-repository/plugins/test.py","r+");
    PyRun_SimpleFile(fin,"foo");

    PyObject *mainDict = PyModule_GetDict(PyImport_Import(PyString_FromString("__main__")));

    PyObject *t = PyDict_GetItemString(mainDict, "pluginClassName");

    PyObject* tStr = PyObject_Str(t);
    QString className = PyString_AsString(tStr);
    qDebug()<<"load plugin: "<<className.toLatin1().data();


    PyObject *claus = PyDict_GetItemString(mainDict, className.toLatin1().data());
    PyObject * pTuple = PyTuple_New (1);

    PyObject *instance = PyObject_CallObject(claus, NULL);


    PyObject *nameObj = PyObject_GetAttrString(instance, "name");
    PyObject* nameRepresentation = PyObject_Str(nameObj);
    QString name(PyString_AsString(nameRepresentation));

    _sipApi = (const sipAPIDef *)PyCapsule_Import("sip._C_API", 0);

    /*
    GetQMetaObjectFunction pyqt5_get_qmetaobject = (GetQMetaObjectFunction)_sipApi->api_import_symbol("pyqt5_get_qmetaobject");*/


    if(_sipApi->api_can_convert_to_type(instance, _sipApi->api_type_from_py_type_object(instance->ob_type), 0))
    {
        qDebug()<<"Conversion... ";
        int state = 0, iserr = 0;
        QDialog * d = (QDialog*)_sipApi->api_convert_to_type(instance, _sipApi->api_type_from_py_type_object(instance->ob_type),instance, 0, &state, &iserr);
        if(d)
        {
            qDebug()<<className<<" : "<<d->windowTitle();
            PythonHelper p;
            p.dialog = d;
            p.className = className;
            p.pyObject = instance;
            p.sipType = _sipApi->api_type_from_py_type_object(instance->ob_type);
            _pythonHelpers.insert(className, p);
        }
    }
    else
    {
         qDebug()<<"cannot be converted ";
    }

    qDebug()<<"finished python ui!\n";

    PyErr_Print();
    //Py_Finalize();
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

    helper.dialog->exec();
}

void PluginsManager::createMenu(QMenu * parent)
{
    /*
    foreach(HelperInterface* helper, _helpers)
    {
        parent->addAction(helper->name(), this, SLOT(callHelper()));
    }*/
    foreach(const PythonHelper &helper, _pythonHelpers)
    {
        QAction *a = new QAction(helper.dialog->windowTitle(), parent);
        a->setProperty("pythonClassName", helper.className);
        connect(a, SIGNAL(triggered()), this, SLOT(callHelper()));
        parent->addAction(a);
    }

}

