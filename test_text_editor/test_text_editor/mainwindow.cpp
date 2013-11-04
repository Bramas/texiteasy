#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "texteditor.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(new TextEditor(this));
}

MainWindow::~MainWindow()
{
    delete ui;
}
