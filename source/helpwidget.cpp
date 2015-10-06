#include "helpwidget.h"
#include "ui_helpwidget.h"
#include "configmanager.h"

HelpWidget::HelpWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HelpWidget)
{
    ui->setupUi(this);
    this->setStyleSheet("QLabel { color: "+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("tab-text").foreground().color())+"; }");

#ifdef OS_MAC
    QString ControlShort = "Cmd";
    ui->label_Ctrl1->setText(ControlShort);
    ui->label_Ctrl2->setText(ControlShort);
    ui->label_Ctrl3->setText(ControlShort);
#endif

}

HelpWidget::~HelpWidget()
{
    delete ui;
}
