#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "download.h"
#include <QProcess>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dl = new Download();
    connect(dl, SIGNAL(versionDownloaded(QString)), this->ui->label, SLOT(setText(QString)));
    connect(dl, SIGNAL(downloadProgress(int)), this->ui->progressBar, SLOT(setValue(int)));
    connect(dl, SIGNAL(done()), this, SLOT(onDownloaded()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onDownloaded()
{
    qDebug()<<"launch : "<<dl->filename();
    QProcess * p = new QProcess();
    //connect(p, SIGNAL(finished(int)), this, SLOT(onFinished(int)));
    p->start(dl->filename());
    this->close();
}

void MainWindow::onFinished(int)
{
    this->close();
}
