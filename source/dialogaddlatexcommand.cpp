#include "dialogaddlatexcommand.h"
#include "ui_dialogaddlatexcommand.h"

DialogAddLatexCommand::DialogAddLatexCommand(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAddLatexCommand)
{
    ui->setupUi(this);
    this->ui->comboBox->addItem(trUtf8("Personalisé"),"");
    this->ui->comboBox->addItem("PdfLatex","pdflatex.exe -synctex=1 -shell-escape -interaction=nonstopmode -enable-write18 %1");
    this->ui->comboBox->addItem("XeLatex","xelatex -synctex=1 -interaction=nonstopmode %1");
    this->ui->comboBox->addItem("Latexmk","latexmk -e \"$pdflatex=q/pdflatex -synctex=1 -interaction=nonstopmode/\" -pdf %1");
    this->ui->comboBox->addItem("Latex + dvipdfm","latex -interaction=nonstopmode %1 ; dvipdfm %1.dvi");
    connect(this->ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentItemChanged(int)));
}

DialogAddLatexCommand::~DialogAddLatexCommand()
{
    delete ui;
}
void DialogAddLatexCommand::selectCustomItem()
{
    ui->comboBox->setCurrentIndex(0);
}

void DialogAddLatexCommand::onCurrentItemChanged(int index)
{
    if(index == 0)
    {
        disconnect(this->ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(selectCustomItem()));
        disconnect(this->ui->lineEdit_name, SIGNAL(textChanged(QString)), this, SLOT(selectCustomItem()));
        return;
    }
    this->ui->lineEdit_name->setText(this->ui->comboBox->itemText(index));
    this->ui->lineEdit->setText(this->ui->comboBox->itemData(index).toString());

    connect(this->ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(selectCustomItem()));
    connect(this->ui->lineEdit_name, SIGNAL(textChanged(QString)), this, SLOT(selectCustomItem()));
}

QString DialogAddLatexCommand::command()
{
    return ui->lineEdit->text();
}
QString DialogAddLatexCommand::name()
{
    return ui->lineEdit_name->text();
}
