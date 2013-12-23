#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}
class Download;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onDownloaded();
    void onFinished(int);

private:
    Ui::MainWindow *ui;
     Download * dl;
};

#endif // MAINWINDOW_H
