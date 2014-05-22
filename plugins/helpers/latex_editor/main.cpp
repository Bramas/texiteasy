#include <QGuiApplication>
#include <QQuickView>
#include <QQuickWindow>

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    QQuickView viewer(QUrl::fromLocalFile("D:/Projects/texiteasy/sourceRepository/plugins/helpers/latex_editor/qml/main.qml"));
    viewer.setResizeMode(QQuickView::SizeRootObjectToView);
    viewer.show();
    return a.exec();
}
