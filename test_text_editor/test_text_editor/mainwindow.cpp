#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "texteditor.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _tester = new Tester();
    _tester->setPlainText("");
    _textEditor = new TextEditor(this, _tester);
    this->setCentralWidget(_textEditor);
    _tester->show();
    connect(_tester, SIGNAL(keyPressed()), this, SLOT(onTextChanged()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::onTextChanged()
{

}
