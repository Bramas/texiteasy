#ifndef PLUGINSMANAGER_H
#define PLUGINSMANAGER_H

#include <QMap>
#include <QString>
#include <QObject>

class HelperInterface;
class QMenu;
class QDialog;

struct _sipAPIDef;
typedef _sipAPIDef sipAPIDef;

struct _object;
typedef _object PyObject;

struct _sipTypeDef;
typedef _sipTypeDef sipTypeDef;

struct PythonHelper{
    QDialog * dialog;
    PyObject * pyObject;
    const sipTypeDef * sipType;
    QString className;
};

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

    const sipAPIDef* _sipApi;

    QMap<QString, HelperInterface*> _helpers;
    QMap<QString, PythonHelper> _pythonHelpers;
};

#endif // PLUGINSMANAGER_H
