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
#include <python.h>
#include <QDialog>
#include <QMetaObject>
#include <QMetaClassInfo>
//#include "qpycore_public_api.h"
#include <sip.h>


#include <Python.h>

#include <QMutex>


/*
#include "qpycore_api.h"
#include "qpycore_objectified_strings.h"
#include "qpycore_pyqtboundsignal.h"
#include "qpycore_pyqtmethodproxy.h"
#include "qpycore_pyqtproperty.h"
#include "qpycore_pyqtpyobject.h"
#include "qpycore_pyqtsignal.h"
#include "qpycore_pyqtslotproxy.h"
#include "qpycore_types.h"

#include "sipAPIQtCore.h"


// The objectified strings.
PyObject *qpycore_dunder_name;
PyObject *qpycore_dunder_pyqtsignature;


// Perform any required post-initialisation.
void qpycore_post_init(PyObject *module_dict)
{
    // Disable the automatic destruction of C++ objects.
    sipSetDestroyOnExit(0);

    // Add the meta-type to the module dictionary.
    if (PyDict_SetItemString(module_dict, "pyqtWrapperType",
                (PyObject *)&qpycore_pyqtWrapperType_Type) < 0)
        Py_FatalError("PyQt5.QtCore: Failed to set pyqtWrapperType type");

    // Initialise the pyqtProperty type and add it to the module dictionary.
    if (PyType_Ready(&qpycore_pyqtProperty_Type) < 0)
        Py_FatalError("PyQt5.QtCore: Failed to initialise pyqtProperty type");

    if (PyDict_SetItemString(module_dict, "pyqtProperty",
                (PyObject *)&qpycore_pyqtProperty_Type) < 0)
        Py_FatalError("PyQt5.QtCore: Failed to set pyqtProperty type");

    // Initialise the pyqtSignal type and add it to the module dictionary.
    if (PyType_Ready(&qpycore_pyqtSignal_Type) < 0)
        Py_FatalError("PyQt5.QtCore: Failed to initialise pyqtSignal type");

    if (PyDict_SetItemString(module_dict, "pyqtSignal",
                (PyObject *)&qpycore_pyqtSignal_Type) < 0)
        Py_FatalError("PyQt5.QtCore: Failed to set pyqtSignal type");

    // Initialise the pyqtBoundSignal type and add it to the module dictionary.
    if (PyType_Ready(&qpycore_pyqtBoundSignal_Type) < 0)
        Py_FatalError("PyQt5.QtCore: Failed to initialise pyqtBoundSignal type");

    if (PyDict_SetItemString(module_dict, "pyqtBoundSignal",
                (PyObject *)&qpycore_pyqtBoundSignal_Type) < 0)
        Py_FatalError("PyQt5.QtCore: Failed to set pyqtBoundSignal type");

    // Initialise the private pyqtMethodProxy type.
    if (PyType_Ready(&qpycore_pyqtMethodProxy_Type) < 0)
        Py_FatalError("PyQt5.QtCore: Failed to initialise pyqtMethodProxy type");

    // Register the C++ type that wraps Python objects.
    PyQt_PyObject::metatype = qRegisterMetaType<PyQt_PyObject>("PyQt_PyObject");
    qRegisterMetaTypeStreamOperators<PyQt_PyObject>("PyQt_PyObject");

    // Register the lazy attribute getter.
    if (sipRegisterAttributeGetter(sipType_QObject, qpycore_get_lazy_attr) < 0)
        Py_FatalError("PyQt5.QtCore: Failed to register attribute getter");

    // Objectify some strings.
#if PY_MAJOR_VERSION >= 3
    qpycore_dunder_pyqtsignature = PyUnicode_FromString("__pyqtSignature__");
#else
    qpycore_dunder_pyqtsignature = PyString_FromString("__pyqtSignature__");
#endif

    if (!qpycore_dunder_pyqtsignature)
        Py_FatalError("PyQt5.QtCore: Failed to objectify '__pyqtSignature__'");

#if PY_MAJOR_VERSION >= 3
    qpycore_dunder_name = PyUnicode_FromString("__name__");
#else
    qpycore_dunder_name = PyString_FromString("__name__");
#endif

    if (!qpycore_dunder_name)
        Py_FatalError("PyQt5.QtCore: Failed to objectify '__name__'");

    // Embed the configuration.
    PyObject *config = PyDict_New();

    if (!config)
        Py_FatalError("PyQt5.QtCore: Failed to create PYQT_CONFIGURATION dict");

#if PY_MAJOR_VERSION >= 3
    PyObject *sip_flags = PyUnicode_FromString("@@PYQT_SIP_FLAGS@@");
#else
    PyObject *sip_flags = PyString_FromString("@@PYQT_SIP_FLAGS@@");
#endif

    if (!sip_flags)
        Py_FatalError("PyQt5.QtCore: Failed to create PYQT_CONFIGURATION.sip_flags");

    if (PyDict_SetItemString(config, "sip_flags", sip_flags) < 0)
        Py_FatalError("PyQt5.QtCore: Failed to set PYQT_CONFIGURATION.sip_flags");

    Py_DECREF(sip_flags);

    if (PyDict_SetItemString(module_dict, "PYQT_CONFIGURATION", config) < 0)
        Py_FatalError("PyQt5.QtCore: Failed to set PYQT_CONFIGURATION dict");

    Py_DECREF(config);

    // Create the mutex that serialises access to the slot proxies.  We don't
    // use a statically initialised one because Qt needs some things to be
    // initialised first (at least for Windows) and this is the only way to
    // guarantee things are done in the right order.
    PyQtSlotProxy::mutex = new QMutex(QMutex::Recursive);
}


*/




PluginsManager PluginsManager::Instance;

PluginsManager::PluginsManager()
{
}
typedef const QMetaObject*  (*GetQMetaObjectFunction)(PyTypeObject* type);

void PluginsManager::test()
{
#ifdef OS_WINDOWS
    qDebug() << getenv("PYTHONPATH");
    qDebug() << getenv("PYTHONHOME");
    qDebug() << getenv("PATH");

    QString p = getenv("PATH");
    p += ";C:\\Python27";
    _putenv_s("PYTHONPATH","C:\\Python27\\Lib");
    _putenv_s("PYTHONHOME","C:\\Python27\\Lib");
    _putenv_s("PATH",p.toLatin1().data());
    qDebug() << getenv("PYTHONPATH");
    qDebug() << getenv("PYTHONHOME");
    qDebug() << getenv("PATH");
#endif
    Py_Initialize();

    qDebug()<<"version "<<Py_GetVersion()<<endl;
#ifdef OS_WINDOWS
    FILE *fin = fopen("D:/Projects/texiteasy/sourceRepository/plugins/test.py","r+");
#else
    FILE *fin = fopen("/Users/quentinbramas/Projects/texiteasy/texiteasy-repository/plugins/test.py","r+");
#endif
    PyRun_SimpleFile(fin,"foo");

    PyObject *mainDict = PyModule_GetDict(PyImport_Import(PyString_FromString("__main__")));

    PyObject *t = PyDict_GetItemString(mainDict, "pluginClassName");

    PyObject* tStr = PyObject_Str(t);
    QString className = PyString_AsString(tStr);
    qDebug()<<"load plugin: "<<className.toLatin1().data();


    PyObject *claus = PyDict_GetItemString(mainDict, className.toLatin1().data());
    PyObject * pTuple = PyTuple_New (1);

    PyObject *instance = PyObject_CallObject(claus, NULL);

    PyObject *name = PyObject_GetAttrString(instance, "name");

    PyObject* nameRepresentation = PyObject_Repr(name);
    qDebug()<<PyString_AsString(nameRepresentation);

    const sipAPIDef* sipApi = (const sipAPIDef *)PyCapsule_Import("sip._C_API", 0);

    GetQMetaObjectFunction pyqt5_get_qmetaobject = (GetQMetaObjectFunction)sipApi->api_import_symbol("pyqt5_get_qmetaobject");


    if(sipApi->api_can_convert_to_type(instance, sipApi->api_type_from_py_type_object(instance->ob_type), 0))
    {
        qDebug()<<"Conversion... ";
        int state = 0, iserr = 0;
        QDialog * d = (QDialog*)sipApi->api_convert_to_type(instance, sipApi->api_type_from_py_type_object(instance->ob_type),instance, 0, &state, &iserr);
        qDebug()<<d->windowTitle();
        //d->exec();
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
    QAction * action = qobject_cast<QAction*>(sender());
    if(!action || !_helpers.contains(action->text()))
    {
        return;
    }
    WidgetFile * currentFile = FileManager::Instance.currentWidgetFile();
    if(!currentFile)
    {
        return;
    }
    _helpers.value(action->text())->exec(currentFile->widgetTextEdit()->textCursor());
}

void PluginsManager::createMenu(QMenu * parent)
{
    foreach(HelperInterface* helper, _helpers)
    {
        parent->addAction(helper->name(), this, SLOT(callHelper()));
    }
}

