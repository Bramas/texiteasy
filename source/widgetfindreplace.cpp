#include "widgetfindreplace.h"
#include "ui_widgetfindreplace.h"
#include "widgettextedit.h"
#include "configmanager.h"
#include <QDebug>
#include <QString>
#include <QLine>

WidgetFindReplace::WidgetFindReplace(WidgetTextEdit *parent) :
    QWidget(parent),
    ui(new Ui::WidgetFindReplace),
    _widgetTextEdit(parent)
{
    ui->setupUi(this);

    QWidget * line = ui->line;
    ui->verticalLayout->removeWidget(line);

    line->setParent(this);
    line->setGeometry(0,0,width(), 1);

    //this->setLayout(this->ui->horizontalLayout);

    connect(ui->pushButtonFind, SIGNAL(clicked()), this, SLOT(find()));
    connect(ui->pushButtonFindPrev, SIGNAL(clicked()), this, SLOT(findBackward()));
    connect(ui->pushButtonReplace, SIGNAL(clicked()), this, SLOT(replace()));
    connect(ui->pushButtonReplaceAndFind, SIGNAL(clicked()), this, SLOT(replaceAndFind()));
    connect(ui->pushButtonReplaceAll, SIGNAL(clicked()), this, SLOT(replaceAll()));

    connect(ui->lineEditFind, SIGNAL(returnPressed()), this, SLOT(find()));

}

WidgetFindReplace::~WidgetFindReplace()
{
    delete ui;
}

void WidgetFindReplace::initTheme()
{
    this->setStyleSheet("background: "+
                        ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("line-number").background().color())+
                        "; color:"+
                        ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color())+
                        "; border: 1px solid "+
                        ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("line-number").foreground().color())+
                        ";");

    this->ui->lineEditFind->setStyleSheet("border: 1px solid "+
                                          ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("line-number").foreground().color())+
                                          ";");
    this->ui->pushButtonFindPrev->setStyleSheet("border: 1px solid "+
                                          ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("line-number").foreground().color())+
                                          ";");
    this->ui->lineEditReplace->setStyleSheet("border: 1px solid "+
                                          ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("line-number").foreground().color())+
                                          ";");
    this->ui->groupBox->setStyleSheet("border: 0px;");
    this->ui->pushButtonFind->setStyleSheet("border: 1px solid "+
                                            ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("line-number").foreground().color())+
                                            ";");
    this->ui->pushButtonClose->setStyleSheet("border: 1px solid "+
                                            ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("line-number").foreground().color())+
                                            ";");
    this->ui->pushButtonReplace->setStyleSheet("border: 1px solid "+
                                            ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("line-number").foreground().color())+
                                            ";");
    this->ui->pushButtonReplaceAll->setStyleSheet("border: 1px solid "+
                                            ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("line-number").foreground().color())+
                                            ";");
    this->ui->pushButtonReplaceAndFind->setStyleSheet("border: 1px solid "+
                                            ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("line-number").foreground().color())+
                                            ";");
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
bool WidgetFindReplace::findBackward(int from, bool canStartOver)
{
    return find(from, canStartOver, true);
}

bool WidgetFindReplace::find(int from, bool canStartOver, bool backward)
{
    QTextDocument::FindFlags options = QTextDocument::FindFlag(0x00000);
    if(this->ui->checkBoxCasse->isChecked())
    {
        options = options | QTextDocument::FindCaseSensitively;
    }
    if(backward)
    {
        options = options | QTextDocument::FindBackward;
    }
    if(from == -1)
    {
        if(backward)
        {
            from = _widgetTextEdit->textCursor().selectionStart();
        }
        else
        {
            from = _widgetTextEdit->textCursor().selectionEnd();
        }
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
        if(backward)
        {
            return find(_widgetTextEdit->toPlainText().size(), false, backward);
        }
        else
        {
            return find(0, false);
        }
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

