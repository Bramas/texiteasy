/***************************************************************************
 *   copyright       : (C) 2013 by Quentin BRAMAS                          *
 *   http://texiteasy.com                                                  *
 *                                                                         *
 *   This file is part of texiteasy.                                       *
 *                                                                         *
 *   texiteasy is free software: you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   texiteasy is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with texiteasy.  If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                         *
 ***************************************************************************/

#include "dialogconfig.h"
#include "configmanager.h"
#include "ui_dialogconfig.h"
#include "mainwindow.h"
#include "dialogkeysequence.h"
#include "dialogaddlatexcommand.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFontDatabase>
#include <QRegExp>
#include <QFileDialog>
#include <QDesktopServices>
#include <QAction>

DialogConfig::DialogConfig(MainWindow *parent) :
    QDialog(parent),
    ui(new Ui::DialogConfig),
    _mainWindows(parent)
{
    ui->setupUi(this);
    connect(this->ui->pushButton_saveAndQuit,SIGNAL(clicked()),this,SLOT(saveAndClose()));
    connect(this->ui->pushButton_save,SIGNAL(clicked()),this,SLOT(save()));
    connect(this->ui->pushButton_quit,SIGNAL(clicked()),this,SLOT(close()));
    connect(this->ui->listWidget, SIGNAL(currentRowChanged( int )), this, SLOT(changePage( int )));
    connect(this->ui->checkBox_replaceDefaultFont, SIGNAL(toggled(bool)), this->ui->comboBox_fontFamilly, SLOT(setEnabled(bool)));
    connect(this->ui->tableWidget_keyBinding, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(configureShortCut(QTableWidgetItem*)));

    connect(this->ui->pushButton_addLatex, SIGNAL(clicked()), this, SLOT(addNewCommand()));
    connect(this->ui->tableWidgetLatexCommands, SIGNAL(itemSelectionChanged()), this, SLOT(onCurrentLatexCommandChanged()));
    connect(this->ui->pushButton_removeLatex, SIGNAL(clicked()), this, SLOT(deleteSelectedLatex()));
    connect(this->ui->pushButton_browseLatexPath, SIGNAL(clicked()), this, SLOT(selectBinDirectory()));
    connect(this->ui->pushButton_svnPath, SIGNAL(clicked()), this, SLOT(selectSvnDirectory()));
    connect(this->ui->OpenCustoCompletionFolderPushButton, SIGNAL(clicked()), this, SLOT(openCustoCompletionFolder()));

    this->ui->listWidget->setCurrentRow(0);

    this->ui->pushButton_save->setVisible(false);

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
    QString currentDictionary = ConfigManager::Instance.currentDictionary();
    foreach(QString dico, ConfigManager::Instance.dictionnaries())
    {
        this->ui->comboBoxDictionary->addItem(dico);
        if(!currentDictionary.compare(dico,Qt::CaseInsensitive))
        {
            this->ui->comboBoxDictionary->setCurrentIndex(this->ui->comboBoxDictionary->count()-1);
        }
    }

}

DialogConfig::~DialogConfig()
{
    delete ui;
}

void DialogConfig::selectSvnDirectory(void)
{
    QString folder = QFileDialog::getExistingDirectory(this, trUtf8("Executables Svn"), qgetenv("PROGRAMFILES"), 0);
    if(folder.isEmpty())
    {
        return;
    }
    this->ui->lineEdit_svnPath->setText(folder);
}
void DialogConfig::selectBinDirectory(void)
{
    QString folder = QFileDialog::getExistingDirectory(this, trUtf8("Executables Latex"), qgetenv("PROGRAMFILES"), 0);
    if(folder.isEmpty())
    {
        return;
    }
    this->ui->lineEdit_latexPath->setText(folder);
}

void DialogConfig::changePage(int currentRow)
{
    //switch(currentRow)
    {
        //case 1:
            this->ui->stackedWidget->setCurrentIndex(currentRow);
    }
}

void DialogConfig::openCustoCompletionFolder()
{
    QDesktopServices::openUrl(QUrl("file:///" + ConfigManager::Instance.customCompletionFolder()));
}

void DialogConfig::saveAndClose()
{
    this->save();
    this->accept();
}
void DialogConfig::save()
{

    // Page Svn

    ConfigManager::Instance.setSvnEnable(this->ui->checkBox_enableSvn->isChecked());
    ConfigManager::Instance.setSvnPath(this->ui->lineEdit_svnPath->text());


    // Page Builder:


    ConfigManager::Instance.setHideAuxFiles(this->ui->checkBoxHideAuxFiles->isChecked());

    ConfigManager::Instance.setBibtexCommand(this->ui->lineEdit_bibtex->text());
    //ConfigManager::Instance.setPdflatexCommand(this->ui->lineEdit_pdflatex->text());
    ConfigManager::Instance.setLatexPath(this->ui->lineEdit_latexPath->text());
    ConfigManager::Instance.setDefaultLatex(this->ui->comboBoxDefaultLatex->currentText());
    QStringList listName, listCommand;
    for(int index = 0; index < ui->tableWidgetLatexCommands->rowCount(); ++index)
    {
        listName << ui->tableWidgetLatexCommands->item(index, 0)->text();
        listCommand << ui->tableWidgetLatexCommands->item(index, 1)->text();
    }
    ConfigManager::Instance.setLatexCommandNames(listName);
    ConfigManager::Instance.setLatexCommands(listCommand);

    // Page General

    ConfigManager::Instance.setOpenLastSessionAtStartup(this->ui->checkBoxOpenLastSessionAtStartup->isChecked());
    ConfigManager::Instance.setLanguage(this->ui->comboBoxLanguages->currentText());
    ConfigManager::Instance.setDictionary(this->ui->comboBoxDictionary->currentText());
    ConfigManager::Instance.setAutoSaveDuration(this->ui->spinBoxAutoSaveDuration->value()*1000);

    // Page Editor
    //

    ConfigManager::Instance.setLineWrapped(this->ui->checkBoxLineWrapped->isChecked());
    ConfigManager::Instance.setDollarAuto(this->ui->checkBoxDollarAuto->isChecked());
    ConfigManager::Instance.setReplaceDefaultFont(this->ui->checkBox_replaceDefaultFont->isChecked());
    _mainWindows->setTheme(this->ui->comboBox_themes->currentText());
    ConfigManager::Instance.setPointSize(this->ui->spinBoxPointSize->value());
    if(this->ui->checkBox_replaceDefaultFont->isChecked())
    {
        ConfigManager::Instance.setFontFamily(this->ui->comboBox_fontFamilly->currentText());
    }
    ConfigManager::Instance.setUsingSpaceIndentation(this->ui->checkBoxIndentationUsingSpace->isChecked());
    ConfigManager::Instance.setTabWidth(this->ui->spinBoxTabWidth->value());
    ConfigManager::Instance.setBlockChangeMarkerEnable(this->ui->checkBoxBlockChangeMarker->isChecked());
    if(this->ui->checkBoxEnableAutoCorrector->isChecked()) {
        ConfigManager::Instance.setPanes(QStringList("Auto Corrector"));
    } else {
        ConfigManager::Instance.setPanes(QStringList());
    }
    // Page Shortcut

    QSettings settings;
    settings.beginGroup("shortcuts");
    QList<QKeySequence> list;
    for (int row = 0; row < _actionsList.count() && row < this->ui->tableWidget_keyBinding->rowCount(); ++row) {
        QAction *action = _actionsList[row];
        list.clear();
        QStringList strList;
        foreach(QString s, this->ui->tableWidget_keyBinding->item(row, 1)->text().split(","))
        {
            strList << s.trimmed();
            list << QKeySequence(s.trimmed(), QKeySequence::NativeText);
        }
        action->setShortcuts(list);
        settings.setValue(action->objectName(), strList);
    }

    // Page Complétion

    QStringList newCompletionFiles;
    QListWidgetItem *elem;
    for (int i=0; i < ui->completionListWidget->count(); i++) {
        elem = ui->completionListWidget->item(i);
        if (elem->checkState()==Qt::Checked)
            newCompletionFiles.append(elem->data(Qt::DecorationRole).toString());
    }
    ConfigManager::Instance.setCompletionFiles(newCompletionFiles);

    ConfigManager::Instance.sendChangedSignal();

}
void DialogConfig::initShortcutsTable()
{
    _actionsList.clear();
    this->ui->tableWidget_keyBinding->clear();
    ui->tableWidget_keyBinding->setRowCount(0);
    this->addEditableActions(_mainWindows->findChildren<QAction *>());
    this->addEditableActions(_mainWindows->actions());
}

void DialogConfig::show()
{
    QSettings settings;
    settings.beginGroup("theme");

    initShortcutsTable();
    // Page Svn

    this->ui->checkBox_enableSvn->setChecked(ConfigManager::Instance.isSvnEnable());
    this->ui->lineEdit_svnPath->setText(ConfigManager::Instance.svnPath());

    // Page General

    this->ui->checkBoxOpenLastSessionAtStartup->setChecked(ConfigManager::Instance.openLastSessionAtStartup());
    this->ui->spinBoxAutoSaveDuration->setValue(ConfigManager::Instance.autoSaveDuration()/1000);

    // Page Builder:

    this->ui->checkBoxHideAuxFiles->setChecked(ConfigManager::Instance.hideAuxFiles());

    currentLatexName = ConfigManager::Instance.defaultLatex();
    this->ui->lineEdit_bibtex->setText(ConfigManager::Instance.bibtexCommand());
    this->ui->lineEdit_latexPath->setText(ConfigManager::Instance.latexPath());

    ui->tableWidgetLatexCommands->clear();
    ui->tableWidgetLatexCommands->setSelectionBehavior(QAbstractItemView::SelectRows);
    int nbRow = ConfigManager::Instance.latexCommands().count();
    ui->tableWidgetLatexCommands->setRowCount(nbRow);
    for(int index = 0; index < nbRow; ++index)
    {
        QString name;
        if(index < ConfigManager::Instance.latexCommandNames().count())
        {
            name = ConfigManager::Instance.latexCommandNames().at(index);
        }
        QString command = ConfigManager::Instance.latexCommands().at(index);
        QTableWidgetItem * item;
        item = new QTableWidgetItem(name);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
        this->ui->tableWidgetLatexCommands->setItem(index, 0, item);
        item = new QTableWidgetItem(command);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
        this->ui->tableWidgetLatexCommands->setItem(index, 1, item);
    }
    updateComboboxDefaultLatex();


    // Page Editor

    this->ui->checkBoxLineWrapped->setChecked(ConfigManager::Instance.isLineWrapped());
    this->ui->checkBoxDollarAuto->setChecked(ConfigManager::Instance.isDollarAuto());
    this->ui->checkBox_replaceDefaultFont->setChecked(ConfigManager::Instance.isDefaultFontReplaced());
    this->ui->checkBoxBlockChangeMarker->setChecked(ConfigManager::Instance.isBlockChangeMarkerEnable());
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
        this->ui->comboBox_themes->addItem(theme);
        if(!currentTheme.compare(theme,Qt::CaseInsensitive))
        {
            this->ui->comboBox_themes->setCurrentIndex(this->ui->comboBox_themes->count()-1);
        }
    }
    this->ui->spinBoxPointSize->setValue(ConfigManager::Instance.getTextCharFormats("normal").font().pointSize());

    this->ui->spinBoxTabWidth->setValue(ConfigManager::Instance.tabWidth());
    this->ui->checkBoxIndentationUsingSpace->setChecked(ConfigManager::Instance.isUsingSpaceIndentation());

    {
        QTableWidgetItem * item = new QTableWidgetItem(trUtf8("Nom"));
        ui->tableWidgetLatexCommands->setHorizontalHeaderItem(0, item);
        item = new QTableWidgetItem(trUtf8("Commande"));
        ui->tableWidgetLatexCommands->setHorizontalHeaderItem(1, item);

        item = new QTableWidgetItem(trUtf8("Action"));
        ui->tableWidget_keyBinding->setHorizontalHeaderItem(0, item);
        item = new QTableWidgetItem(trUtf8("Raccourci"));
        ui->tableWidget_keyBinding->setHorizontalHeaderItem(1, item);
    }
    if(ConfigManager::Instance.panes().contains("Auto Corrector")) {
        this->ui->checkBoxEnableAutoCorrector->setChecked(true);
    }


    // Page Completion

    QDir completionDir(":/completion");
    QStringList completionFiles = ConfigManager::Instance.completionFiles();
    foreach(const QString& elem, completionDir.entryList(QDir::Files | QDir::Readable)) {
        QListWidgetItem *item = new QListWidgetItem(elem, ui->completionListWidget);
        QString filename = ":/completion/"+elem;
        item->setData(Qt::DecorationRole, filename);
        item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
        if (completionFiles.contains(filename))
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);
    }

    ui->CustomCompletionFolderLineEdit->setText(ConfigManager::Instance.customCompletionFolder());

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
            QString name = action->text();
            if(action->objectName() == "actionDefaultCommandLatex")
            {
                name = trUtf8("Compilation par défault");
            }
            this->ui->tableWidget_keyBinding->insertRow(row);
            this->ui->tableWidget_keyBinding->setItem(row, 0, new QTableWidgetItem(name));
            QStringList shortcuts;
            foreach(const QKeySequence &s, action->shortcuts())
            {
                shortcuts << s.toString(QKeySequence::NativeText);
            }

            this->ui->tableWidget_keyBinding->setItem(row, 1, new QTableWidgetItem(shortcuts.join(", ")));
            this->ui->tableWidget_keyBinding->item(row,0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            _actionsList.append(action);
            ++row;
        }

}
void DialogConfig::addNewCommand()
{
    DialogAddLatexCommand dialog(this);
    if(!dialog.exec())
    {
        return;
    }
    QString command = dialog.command();
    QString name = dialog.name();
    if(name.isEmpty() || command.isEmpty())
    {
        return;
    }
    int nbRow = this->ui->tableWidgetLatexCommands->rowCount();

    this->ui->tableWidgetLatexCommands->insertRow(nbRow);

    QTableWidgetItem * item;
    item = new QTableWidgetItem(name);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    this->ui->tableWidgetLatexCommands->setItem(nbRow, 0, item);
    item = new QTableWidgetItem(command);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    this->ui->tableWidgetLatexCommands->setItem(nbRow, 1, item);

    onCurrentLatexCommandChanged();
    updateComboboxDefaultLatex();
}
void DialogConfig::onCurrentLatexCommandChanged()
{
    if(ui->tableWidgetLatexCommands->rowCount()<2)
    {
        ui->pushButton_removeLatex->setEnabled(false);
        return;
    }
    if(ui->tableWidgetLatexCommands->selectedItems().isEmpty())
    {
        ui->pushButton_removeLatex->setEnabled(false);
        return;
    }
    ui->pushButton_removeLatex->setEnabled(true);
}
void DialogConfig::deleteSelectedLatex()
{
    if(ui->tableWidgetLatexCommands->selectedItems().isEmpty())
    {
        return;
    }

    int row = ui->tableWidgetLatexCommands->selectedItems().first()->row();
    ui->tableWidgetLatexCommands->removeRow(row);
    updateComboboxDefaultLatex();
}
void DialogConfig::updateComboboxDefaultLatex()
{
    bool defaultExist = false;
    this->ui->comboBoxDefaultLatex->clear();
    for(int row = 0; row < this->ui->tableWidgetLatexCommands->rowCount(); ++row)
    {
        QString name = ui->tableWidgetLatexCommands->item(row,0)->text();
        this->ui->comboBoxDefaultLatex->addItem(name);
        if(!name.compare(currentLatexName))
        {
            defaultExist = true;
            this->ui->comboBoxDefaultLatex->setCurrentIndex(ui->comboBoxDefaultLatex->count() - 1);
        }
    }
    if(!defaultExist)
    {
        currentLatexName = this->ui->comboBoxDefaultLatex->currentText();
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
