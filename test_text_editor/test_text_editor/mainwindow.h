#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}
class Tester;
class TextEditor;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void onTextChanged();
    
private:
    TextEditor * _textEditor;
    Tester * _tester;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
