#include "widgetfindreplace.h"
#include "ui_widgetfindreplace.h"
#include "widgettextedit.h"
#include <QDebug>
#include <QString>

WidgetFindReplace::WidgetFindReplace(WidgetTextEdit *parent) :
    QWidget(parent),
    ui(new Ui::WidgetFindReplace),
    _widgetTextEdit(parent)
{
    ui->setupUi(this);

    //this->setLayout(this->ui->horizontalLayout);

    connect(ui->pushButtonFind, SIGNAL(clicked()), this, SLOT(find()));
    connect(ui->pushButtonReplace, SIGNAL(clicked()), this, SLOT(replace()));
    connect(ui->pushButtonReplaceAndFind, SIGNAL(clicked()), this, SLOT(replaceAndFind()));
    connect(ui->pushButtonReplaceAll, SIGNAL(clicked()), this, SLOT(replaceAll()));

    connect(ui->lineEditFind, SIGNAL(returnPressed()), this, SLOT(find()));

}

WidgetFindReplace::~WidgetFindReplace()
{
    delete ui;
}

QPushButton * WidgetFindReplace::pushButtonClose()
{
    return ui->pushButtonClose;
}
void WidgetFindReplace::open()
{
    this->ui->lineEditFind->setFocus();
    QString selectedText = _widgetTextEdit->textCursor().selectedText();
    if(!selectedText.isEmpty())
    {
        this->ui->lineEditFind->setText(selectedText);
        this->ui->lineEditFind->selectAll();
    }
    else
    {
        this->ui->lineEditFind->setText("");
    }
}

bool WidgetFindReplace::find(int from, bool canStartOver)
{
    QTextDocument::FindFlag options = QTextDocument::FindFlag(0x00000);
    if(this->ui->checkBoxCasse->isChecked())
    {
        options = QTextDocument::FindCaseSensitively;
    }
    if(from == -1)
    {
        from = _widgetTextEdit->textCursor().position();
    }

    QTextCursor findResult;
    if(this->ui->checkBoxRegex->isChecked())
    {
        QRegExp exp(this->ui->lineEditFind->text(), ui->checkBoxCasse->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);
        findResult = _widgetTextEdit->document()->find(exp, from, options);
    }
    else
    {
        QString exp = this->ui->lineEditFind->text();
        findResult = _widgetTextEdit->document()->find(exp, from, options);
    }
    if(!findResult.isNull())
    {
        _widgetTextEdit->setTextCursor(findResult);
        return true;
    }

    if(canStartOver)
    {
        return find(0, false);
    }
    return false;
}

void WidgetFindReplace::replace()
{
    QTextCursor cursor = _widgetTextEdit->textCursor();
    QString selectedText = cursor.selectedText();
    QString replacement = ui->lineEditReplace->text();
    QString newText;
    if(this->ui->checkBoxRegex->isChecked())
    {
        QRegExp exp(this->ui->lineEditFind->text(), ui->checkBoxCasse->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);
        newText = selectedText.replace(exp,replacement);
    }
    else
    {
        QString exp = this->ui->lineEditFind->text();
        newText = selectedText.replace(exp,replacement);
    }
    cursor.removeSelectedText();
    cursor.insertText(newText);
    _widgetTextEdit->setTextCursor(cursor);
}

bool WidgetFindReplace::replaceAndFind()
{
    this->replace();
    return this->find();
}
void WidgetFindReplace::replaceAll()
{
    if(find(0, false))
    {
        do
        {
            replace();
        }
        while(find(-1, false));
    }
}

void WidgetFindReplace::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
           this->ui->retranslateUi(this);
    } else
        QWidget::changeEvent(event);
}

