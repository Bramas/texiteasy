#include <QGuiApplication>
#include <QQuickView>

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

#ifdef MAC_OS_X_VERSION_10_0
    QQuickView viewer(QUrl::fromLocalFile("/Users/quentinbramas/Projects/texiteasy/texiteasy-repository/plugins/helpers/latex_editor/qml/main.qml"));
#else
    QQuickView viewer(QUrl::fromLocalFile("D:/Projects/texiteasy/sourceRepository/plugins/helpers/latex_editor/qml/main.qml"));
#endif
    viewer.show();
    return a.exec();
}
