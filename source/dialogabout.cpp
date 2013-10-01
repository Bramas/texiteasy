#include "dialogabout.h"
#include "ui_dialogabout.h"

DialogAbout::DialogAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAbout)
{
    ui->setupUi(this);
    QString content("<center>");
    content += APPLICATION_NAME;
    content += ". Version ";
    content += CURRENT_VERSION;
    content += ".</center>";

    this->ui->textEdit->setHtml(content);
}

DialogAbout::~DialogAbout()
{
    delete ui;
}
