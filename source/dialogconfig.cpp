/***************************************************************************
 *   copyright       : (C) 2013 by Quentin BRAMAS                          *
 *   http://texiteasy.com                                                  *
 *                                                                         *
 *   This file is part of texiteasy.                                          *
 *                                                                         *
 *   texiteasy is free software: you can redistribute it and/or modify        *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   texiteasy is distributed in the hope that it will be useful,             *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with texiteasy.  If not, see <http://www.gnu.org/licenses/>.       *                         *
 *                                                                         *
 ***************************************************************************/

#include "dialogconfig.h"
#include "configmanager.h"
#include "ui_dialogconfig.h"
#include "mainwindow.h"
#include "dialogkeysequence.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFontDatabase>
#include <QRegExp>

DialogConfig::DialogConfig(MainWindow *parent) :
    QDialog(parent),
    _parent(parent),
    ui(new Ui::DialogConfig)
{
    ui->setupUi(this);
    connect(this->ui->pushButton_saveAndQuit,SIGNAL(clicked()),this,SLOT(saveAndClose()));
    connect(this->ui->pushButton_save,SIGNAL(clicked()),this,SLOT(save()));
    connect(this->ui->pushButton_quit,SIGNAL(clicked()),this,SLOT(close()));
    connect(this->ui->listWidget, SIGNAL(currentRowChanged( int )), this, SLOT(changePage( int )));
    connect(this->ui->checkBox_replaceDefaultFont, SIGNAL(toggled(bool)), this->ui->comboBox_fontFamilly, SLOT(setEnabled(bool)));
    connect(this->ui->tableWidget_keyBinding, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(configureShortCut(QTableWidgetItem*)));

    // Page General
    QString currentLanguage = ConfigManager::Instance.language();
    foreach(QString language, ConfigManager::Instance.languagesList())
    {
        this->ui->comboBoxLanguages->addItem(language);
        if(!currentLanguage.compare(language,Qt::CaseInsensitive))
        {
            this->ui->comboBoxLanguages->setCurrentIndex(this->ui->comboBoxLanguages->count()-1);
        }
    }

}

DialogConfig::~DialogConfig()
{
    delete ui;
}

void DialogConfig::changePage(int currentRow)
{
    //switch(currentRow)
    {
        //case 1:
            this->ui->stackedWidget->setCurrentIndex(currentRow);
    }
}

void DialogConfig::saveAndClose()
{
    this->save();
    this->accept();
}
void DialogConfig::save()
{

    // Page General
    ConfigManager::Instance.setLanguage(this->ui->comboBoxLanguages->currentText());

    // Page Editor
    //

    ConfigManager::Instance.setReplaceDefaultFont(this->ui->checkBox_replaceDefaultFont->isChecked());
    _parent->setTheme(this->ui->comboBox_themes->currentText());
    ConfigManager::Instance.setPointSize(this->ui->spinBoxPointSize->value());
    if(this->ui->checkBox_replaceDefaultFont->isChecked())
    {
        ConfigManager::Instance.setFontFamily(this->ui->comboBox_fontFamilly->currentText());
    }


    // Page Builder:

    ConfigManager::Instance.setBibtexCommand(this->ui->lineEdit_bibtex->text());
    ConfigManager::Instance.setPdflatexCommand(this->ui->lineEdit_pdflatex->text());
    ConfigManager::Instance.setLatexPath(this->ui->lineEdit_latexPath->text());

    // Page Shortcut

    QSettings settings;
    settings.beginGroup("shortcuts");
    for (int row = 0; row < this->ui->tableWidget_keyBinding->rowCount(); ++row) {
        QAction *action = _actionsList[row];
        action->setShortcut(QKeySequence(this->ui->tableWidget_keyBinding->item(row, 1)->text()));
        settings.setValue(action->text(),action->shortcut());
    }

}
void DialogConfig::show()
{
    QSettings settings;
    settings.beginGroup("theme");



    // Page Builder:

    this->ui->lineEdit_bibtex->setText(ConfigManager::Instance.bibtexCommand());
    this->ui->lineEdit_pdflatex->setText(ConfigManager::Instance.pdflatexCommand());
    this->ui->lineEdit_latexPath->setText(ConfigManager::Instance.latexPath());

    // Page Editor

    this->ui->checkBox_replaceDefaultFont->setChecked(ConfigManager::Instance.isDefaultFontReplaced());
    this->ui->comboBox_fontFamilly->clear();
    QFontDatabase database;
    QString currentFamily = ConfigManager::Instance.getTextCharFormats("normal").font().family();
    foreach (const QString &family, database.families())
    {
        this->ui->comboBox_fontFamilly->addItem(family);
        if(!currentFamily.compare(family,Qt::CaseInsensitive))
        {
            this->ui->comboBox_fontFamilly->setCurrentIndex(this->ui->comboBox_fontFamilly->count()-1);
        }
    }
    QString currentTheme = ConfigManager::Instance.theme();
    foreach(QString theme, ConfigManager::Instance.themesList())
    {
        this->ui->comboBox_themes->addItem(theme.remove(QRegExp("\\.[^.]*$")));
        if(!currentTheme.compare(theme,Qt::CaseInsensitive))
        {
            this->ui->comboBox_themes->setCurrentIndex(this->ui->comboBox_themes->count()-1);
        }
    }
    this->ui->spinBoxPointSize->setValue(ConfigManager::Instance.getTextCharFormats("normal").font().pointSize());



    QDialog::show();
}

void DialogConfig::addEditableActions(const QList<QAction *> &actions)
{
        int row = this->ui->tableWidget_keyBinding->rowCount();
        foreach (QAction *action, actions) {
            if(action->objectName().isEmpty() ||
                    action->priority() == QAction::LowPriority)
            {
                continue;
            }
            this->ui->tableWidget_keyBinding->insertRow(row);
            this->ui->tableWidget_keyBinding->setItem(row, 0, new QTableWidgetItem(action->text()));
            this->ui->tableWidget_keyBinding->setItem(row, 1, new QTableWidgetItem(action->shortcut().toString()));
            this->ui->tableWidget_keyBinding->item(row,0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            _actionsList.append(action);
            ++row;
        }

}

void DialogConfig::configureShortCut(QTableWidgetItem *item)
{
    QString shortcut,data,newshortcut;
    if (item && item->column() == 1)
    {
        shortcut=item->text();
        data=item->data(Qt::UserRole).toString();
        //if (data=="key")
        {
            DialogKeySequence * keydlg = new DialogKeySequence(this);
            keydlg->setKeySequence(QKeySequence(shortcut));
            if ( keydlg->exec() )
            {
                newshortcut=keydlg->ui.lineEdit->text();
                if (!newshortcut.isEmpty())
                {
                    item->setText(newshortcut);
                    //item->setData(Qt::UserRole,QString("key"));
                }
                else
                {
                    item->setText("");//"none");
                    //item->setData(Qt::UserRole,QString("key"));
                }
            }
            delete (keydlg);
        }
    }
}
void DialogConfig::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
           this->ui->retranslateUi(this);
    } else
        QWidget::changeEvent(event);
}
