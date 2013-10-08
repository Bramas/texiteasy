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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "widgetlinenumber.h"
#include "widgetpdfviewer.h"
#include "widgettextedit.h"
#include "widgetscroller.h"
#include "syntaxhighlighter.h"
#include "file.h"
#include "builder.h"
#include "dialogwelcome.h"
#include "dialogconfig.h"
#include "viewer.h"
#include "widgetpdfdocument.h"
#include "dialogclose.h"
#include "widgetfindreplace.h"
#include "minisplitter.h"
#include "widgetsimpleoutput.h"
#include "widgetproject.h"

#include <QAction>
#include <QScrollBar>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QSettings>
#include <QPushButton>
#include <QDebug>
#include <QMimeData>
#include <QString>
#include <QPalette>
#include <QPixmap>
#include <QTableWidget>
#include "configmanager.h"
#include "widgetconsole.h"
#include "widgetstatusbar.h"
#include "dialogabout.h"
#include "widgetfile.h"
#include "filemanager.h"
#include "widgettab.h"

#include <QList>

typedef QList<int> IntegerList;
Q_DECLARE_METATYPE(IntegerList)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    dialogConfig(new DialogConfig(this)),
    dialogWelcome(new DialogWelcome(this)),
    _emptyWidget(new WidgetEmpty(0))
{
    ui->setupUi(this);
    ConfigManager::Instance.setMainWindow(this);
    ConfigManager::Instance.init();

    _tabWidget = new WidgetTab();
    connect(_tabWidget, SIGNAL(currentChanged(WidgetFile*)), this, SLOT(onCurrentFileChanged(WidgetFile*)));
    connect(_tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    ui->verticalLayout->setMargin(0);
    ui->verticalLayout->setSpacing(0);
    ui->verticalLayout->setContentsMargins(0,0,0,0);
    ui->verticalLayout->addWidget(_tabWidget);
    ui->verticalLayout->addWidget(_emptyWidget);
    connect(_emptyWidget, SIGNAL(mouseDoubleClick()), this, SLOT(newFile()));



    _widgetStatusBar = new WidgetStatusBar(this);
    this->setStatusBar(_widgetStatusBar);
    connect(&FileManager::Instance, SIGNAL(cursorPositionChanged(int,int)), _widgetStatusBar, SLOT(setPosition(int,int)));


    QSettings settings;
    settings.beginGroup("mainwindow");
    if(settings.contains("geometry"))
    {
        this->setGeometry(settings.value("geometry").toRect());
    }

    //define background
    this->initTheme();



    // Connect menubar Actions
    connect(this->ui->actionDeleteLastOpenFiles,SIGNAL(triggered()),this,SLOT(clearLastOpened()));
    connect(this->ui->actionNouveau,SIGNAL(triggered()),this,SLOT(newFile()));
    connect(this->ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(this->ui->actionAbout, SIGNAL(triggered()), new DialogAbout(this), SLOT(show()));
    connect(this->ui->actionOpen,SIGNAL(triggered()),this,SLOT(open()));
    connect(this->ui->actionSave,SIGNAL(triggered()),&FileManager::Instance,SLOT(save()));
    connect(this->ui->actionSaveAs,SIGNAL(triggered()),&FileManager::Instance,SLOT(saveAs()));
    connect(this->ui->actionOpenConfigFolder, SIGNAL(triggered()), &ConfigManager::Instance, SLOT(openThemeFolder()));
    connect(this->ui->actionSettings,SIGNAL(triggered()),this->dialogConfig,SLOT(show()));

    connect(this->dialogConfig,SIGNAL(accepted()), &FileManager::Instance,SLOT(rehighlight()));
    connect(this->ui->actionUndo, SIGNAL(triggered()), &FileManager::Instance, SLOT(undo()));
    connect(this->ui->actionRedo, SIGNAL(triggered()), &FileManager::Instance, SLOT(redo()));
    connect(this->ui->actionCopy, SIGNAL(triggered()), &FileManager::Instance, SLOT(copy()));
    connect(this->ui->actionCut, SIGNAL(triggered()), &FileManager::Instance, SLOT(cut()));
    connect(this->ui->actionPaste, SIGNAL(triggered()), &FileManager::Instance, SLOT(paste()));
    connect(this->ui->actionFindReplace, SIGNAL(triggered()), &FileManager::Instance, SLOT(openFindReplaceWidget()));
    connect(this->ui->actionEnvironment, SIGNAL(triggered()), &FileManager::Instance, SLOT(wrapEnvironment()));
    connect(this->ui->actionPdfLatex,SIGNAL(triggered()), &FileManager::Instance,SLOT(pdflatex()));
    connect(this->ui->actionBibtex,SIGNAL(triggered()), &FileManager::Instance,SLOT(bibtex()));
    connect(this->ui->actionView, SIGNAL(triggered()), &FileManager::Instance,SLOT(jumpToPdfFromSource()));


    QAction * lastAction = this->ui->menuTh_me->actions().last();
    foreach(const QString& theme, ConfigManager::Instance.themesList())
    {
        QAction * action = new QAction(theme.left(theme.size()-10), this->ui->menuTh_me);
        action->setPriority(QAction::LowPriority);
        action->setCheckable(true);
        if(!theme.left(theme.size()-10).compare(ConfigManager::Instance.theme()))
        {
            action->setChecked(true);
        }
        this->ui->menuTh_me->insertAction(lastAction,action);
        connect(action, SIGNAL(triggered()), this, SLOT(changeTheme()));
    }
    this->ui->menuTh_me->insertSeparator(lastAction);

    settings.endGroup();
    lastAction = this->ui->menuOuvrir_R_cent->actions().last();
    QStringList lastFiles = settings.value("lastFiles").toStringList();
    foreach(const QString& file, lastFiles)
    {
        QAction * action = new QAction(file, this->ui->menuOuvrir_R_cent);
        action->setPriority(QAction::LowPriority);
        this->ui->menuOuvrir_R_cent->insertAction(lastAction,action);
        connect(action, SIGNAL(triggered()), this, SLOT(openLast()));
    }
    this->ui->menuOuvrir_R_cent->insertSeparator(lastAction);



    {
        QList<QAction *> actionsList = this->findChildren<QAction *>();
        QSettings settings;
        settings.beginGroup("shortcuts");
        //foreach(QAction * action, actionsList) {
        //    if(settings.contains(action->text()))
            {
                //action->setShortcut(QKeySequence(settings.value(action->text()).toString()));
            }
        //}
        dialogConfig->addEditableActions(actionsList);
    }


    return;
/*
    connect(this->widgetTextEdit->getCurrentFile()->getBuilder(), SIGNAL(statusChanged(QString)), this->statusBar(), SLOT(showMessage(QString)));//
    connect(_widgetConsole, SIGNAL(requestLine(int)), widgetTextEdit, SLOT(goToLine(int)));//




    //Display only the editor :
    {
        qRegisterMetaType<IntegerList>("IntegerList");
        qRegisterMetaTypeStreamOperators<IntegerList>("IntegerList");

        QList<int> sizes;
        if(settings.contains("leftSplitterEditorSize"))
        {
            sizes << settings.value("leftSplitterEditorSize").toInt();
        }
        else
        {
            sizes<<800;
        }
        if(settings.contains("leftSplitterReplaceSize"))
        {
            sizes << settings.value("leftSplitterReplaceSize").toInt();
        }
        else
        {
            sizes<<0;
        }
        if(settings.contains("leftSplitterSimpleOutputSize"))
        {
            sizes << settings.value("leftSplitterSimpleOutputSize").toInt();
            if(sizes.last()>0)
            {
                _widgetStatusBar->toggleErrorTable();
            }
        }
        else
        {
            sizes<<0;
        }
        if(settings.contains("leftSplitterConsoleSize"))
        {
            sizes << settings.value("leftSplitterConsoleSize").toInt();
            if(sizes.last()>0)
            {
                _widgetStatusBar->toggleConsole();
            }
        }
        else
        {
            sizes<<0;
        }
        _leftSplitter->setSizes(sizes);
        QList<int> mainSizes;
        if(settings.contains("mainSplitterEditorSize"))
        {
            mainSizes << settings.value("mainSplitterEditorSize").toInt();
            mainSizes << width() - settings.value("mainSplitterEditorSize").toInt();
        }
        else
        {
            mainSizes << width() / 2 << width() / 2;
        }
        _mainSplitter->setSizes(mainSizes);
    }*/

}

MainWindow::~MainWindow()
{
    QSettings settings;
    settings.beginGroup("mainwindow");
    settings.setValue("geometry", this->geometry());

    /*
    // Save settings
    {

        {
            QList<int> iList;
            iList = _mainSplitter->sizes();
            settings.setValue("mainSplitterEditorSize",iList[0]);
        }
        {
            QList<int> iList;
            iList = _leftSplitter->sizes();
            settings.setValue("leftSplitterEditorSize",iList[0]);
            settings.setValue("leftSplitterReplaceSize",iList[1]);
            settings.setValue("leftSplitterSimpleOutputSize",iList[2]);
            settings.setValue("leftSplitterConsoleSize",iList[3]);
        }
        settings.endGroup();
    }*/
    delete ui;
}

void MainWindow::focus()
{
    this->activateWindow();
}

void MainWindow::closeEvent(QCloseEvent * event)
{

    while(_tabWidget->count())
    {
        if(!this->closeTab(0))
        {
            event->ignore();
            return;
        }
    }
    event->accept();
/*    if(this->closeCurrentFile())
    {
        event->accept();
        return;
    }
    event->ignore();*/
}

void MainWindow::newFile()
{
    if(FileManager::Instance.newFile())
    {
        _tabWidget->addTab(FileManager::Instance.currentWidgetFile(), "untitled");
        _tabWidget->setCurrentIndex(_tabWidget->count()-1);
        _widgetStatusBar->closeConsole();
        _widgetStatusBar->closeErrorTable();
    }
    return;
}

void MainWindow::openLast()
{
    QString filename = dynamic_cast<QAction*>(sender())->text();
    open(filename);
}
void MainWindow::open(QString filename)
{
    QSettings settings;
    //get the filname
    if(filename.isEmpty())
    {
        filename = QFileDialog::getOpenFileName(this,tr("Ouvrir un fichier"),settings.value("lastFolder").toString(),tr("Latex (*.tex *.latex);;BibTex(*.bib)"));

        if(filename.isEmpty())
        {
            return;
        }
    }
    QFileInfo info(filename);
    settings.setValue("lastFolder",info.path());
    QString basename = info.baseName();
    //window title
    this->setWindowTitle(basename+" - texiteasy");
    //udpate the settings
    {
        QSettings settings;
        QStringList lastFiles = settings.value("lastFiles",QStringList()).toStringList();
        lastFiles.prepend(filename);
        lastFiles.removeDuplicates();
        while(lastFiles.count()>10) { lastFiles.pop_back(); }
        settings.setValue("lastFiles", lastFiles);
    }
    //open

    if(FileManager::Instance.open(filename))
    {
        _tabWidget->addTab(FileManager::Instance.currentWidgetFile(), FileManager::Instance.currentWidgetFile()->widgetTextEdit()->getCurrentFile()->fileInfo().baseName());
        _tabWidget->setCurrentIndex(_tabWidget->count()-1);
        _widgetStatusBar->closeConsole();
        _widgetStatusBar->closeErrorTable();
    }
    else
    {
        _tabWidget->setTabText(_tabWidget->currentIndex(), FileManager::Instance.currentWidgetFile()->widgetTextEdit()->getCurrentFile()->fileInfo().baseName());
    }

    this->statusBar()->showMessage(basename,4000);
    //this->_widgetStatusBar->setEncoding(this->widgetTextEdit->getCurrentFile()->codec());

}

void MainWindow::onCurrentFileChanged(WidgetFile * widget)
{
    this->closeCurrentWidgetFile();
    FileManager::Instance.setCurrent(widget);
    if(!widget)
    {

        ui->verticalLayout->addWidget(_emptyWidget);
        return;
    }
    ui->verticalLayout->addWidget(widget);
    widget->widgetTextEdit()->setFocus();
}

bool MainWindow::closeTab(int index)
{
    WidgetFile * widget = _tabWidget->widget(index);

    if(widget->widgetTextEdit()->getCurrentFile()->isModified())
    {
        DialogClose dialogClose(this);
        dialogClose.setMessage(tr(QString::fromUtf8("Le fichier %1 n'a pas été enregistré.").toLatin1()).arg(widget->widgetTextEdit()->getCurrentFile()->getFilename()));
        dialogClose.exec();
        if(dialogClose.confirmed())
        {
            if(dialogClose.saved())
            {
                widget->save();
            }
        }
        else
        {
            return false;
        }
    }

    _tabWidget->removeTab(index);
    if(!_tabWidget->count())
    {
        // The following is also call by the tabWidget but maybe after so we
        // make sure that it is call before closing the widget
        this->onCurrentFileChanged(0);
    }
    FileManager::Instance.close(widget);
    return true;
}

void MainWindow::clearLastOpened()
{
    QSettings settings;
    settings.setValue("lastFiles", QStringList());
    this->ui->menuOuvrir_R_cent->clear();
    this->ui->menuOuvrir_R_cent->insertAction(0,this->ui->actionDeleteLastOpenFiles);
}




void MainWindow::changeTheme()
{
    QString text = dynamic_cast<QAction*>(this->sender())->text();
    this->setTheme(text);

}
void MainWindow::setTheme(QString theme)
{
    foreach(QAction * action, this->ui->menuTh_me->actions())
    {
        if(action->text().compare(theme))
            action->setChecked(false);
        else
            action->setChecked(true);

    }
    ConfigManager::Instance.load(theme);
    this->initTheme();
    FileManager::Instance.rehighlight();
    FileManager::Instance.currentWidgetFile()->widgetTextEdit()->onCursorPositionChange();
}

void MainWindow::initTheme()
{
    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, ConfigManager::Instance.getTextCharFormats("linenumber").background().color());
    this->setAutoFillBackground(true);
    this->setPalette(Pal);
    FileManager::Instance.initTheme();
}

void MainWindow::closeCurrentWidgetFile()
{
    if(ui->verticalLayout->count() > 1)
    {
        QWidget * w = ui->verticalLayout->itemAt(1)->widget();
        ui->verticalLayout->removeWidget(w);
        w->setParent(0);
    }
}
