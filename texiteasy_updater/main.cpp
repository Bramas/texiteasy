#include "mainwindow.h"
#include <QApplication>
#include <QCoreApplication>
#include <QDir>

int main(int argc, char *argv[])
{

#ifdef LIB_DEPLOY
    QDir dir(QFileInfo(argv[0]).path());  // e.g. appdir
    dir.cd("PlugIns");  // e.g. appdir/PlugIns
    QCoreApplication::setLibraryPaths(QStringList(dir.absolutePath()));
#endif

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
