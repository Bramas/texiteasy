#include "qmlloader.h"
#include "ui_qmlloader.h"

qmlLoader::qmlLoader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::qmlLoader)
{
    ui->setupUi(this);
}

qmlLoader::~qmlLoader()
{
    delete ui;
}
