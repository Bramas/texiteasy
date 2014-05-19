#include <QGuiApplication>
#include <QQuickView>

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    QQuickView viewer(QUrl::fromLocalFile("D:/Projects/texiteasy/sourceRepository/plugins/helpers/latex_editor/qml/main.qml"));
    viewer.show();
    return a.exec();
}
