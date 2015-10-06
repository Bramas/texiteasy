#include "emptywidget.h"
#include "ui_emptywidget.h"
#include "configmanager.h"

EmptyWidget::EmptyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EmptyWidget)
{
    ui->setupUi(this);
    this->setStyleSheet("QLabel { color: "+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("tab-text").foreground().color())+"; }");
}

EmptyWidget::~EmptyWidget()
{
    delete ui;
}
