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

#include "widgetinsertcommand.h"
#include "ui_widgetinsertcommand.h"
#include "configmanager.h"
#include "widgettextedit.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>
#include <QProcess>
#include <QIcon>

WidgetInsertCommand * WidgetInsertCommand::_instance = 0;

WidgetInsertCommand::WidgetInsertCommand() :
    QWidget(0),
    ui(new Ui::WidgetInsertCommand),
    _widgetTextEdit(0)
{
    ui->setupUi(this);
    this->setVisible(false);

    mathEnvIndex = 2;
    commandIndex = 0;
    groupIndex = 1;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName("localhost");
    db.setDatabaseName(ConfigManager::Instance.commandDatabaseFilename());
    if(!db.open())
    {
        qDebug()<<"Database "<<ConfigManager::Instance.commandDatabaseFilename()<<" not found : "<<db.lastError();
        return;
    }
    //qDebug()<<db.tables();
    /*{
        QStringList addList;
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\leftrightarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\rightarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\mapsto', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\longleftarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\longleftrightarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\longrightarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\longmapsto', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\downarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\updownarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\uparrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nwarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\searrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nearrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\swarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\textdownarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\textuparrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\textleftarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\textrightarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nleftarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nleftrightarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nrightarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\hookleftarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\hookrightarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\twoheadleftarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\twoheadrightarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\leftarrowtail', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\rightarrowtail', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\Leftarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\Leftrightarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\Rightarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\Longleftarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\Longleftrightarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\Longrightarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\Updownarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\Uparrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\Downarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nLeftarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nLeftrightarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nRightarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\leftleftarrows', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\leftrightarrows', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\rightleftarrows', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\rightrightarrows', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\downdownarrows', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\upuparrows', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\circlearrowleft', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\circlearrowright', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\curvearrowleft', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\curvearrowright', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\Lsh', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\Rsh', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\looparrowleft', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\looparrowright', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\dashleftarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\dashrightarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\leftrightsquigarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\rightsquigarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\Lleftarrow', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\leftharpoondown', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\rightharpoondown', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\leftharpoonup', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\rightharpoonup', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\rightleftharpoons', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\leftrightharpoons', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\downharpoonleft', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\upharpoonleft', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\downharpoonright', 'arrows');";
        addList<<" INSERT INTO commands (command, command_group) VALUES ('\\upharpoonright', 'arrows');";

        foreach(QString q, addList)
        {
            QSqlQuery query(q,db);
        }
    }*/
    QSqlQuery query("SELECT command, command_group FROM commands",db);
    //query.prepare;
    /*
     *  query.bindValue(":id", 1001);
        query.bindValue(":forename", "Bart");
        query.bindValue(":surname", "Simpson");
        */
    //query.exec();

    this->ui->tabWidget->clear();
    int colCount = 0;
    while (query.next()) {
        QString command = query.value(commandIndex).toString().trimmed();
        QString group = query.value(groupIndex).toString().trimmed();
        if(!_tabslabel.contains(group))
        {
            QTableWidget * table = new QTableWidget(0,9);
            table->horizontalHeader()->setDefaultSectionSize(43);
            table->horizontalHeader()->setStretchLastSection(true);
            table->horizontalHeader()->hide();
            table->verticalHeader()->hide();
            this->ui->tabWidget->addTab(table, group);
            connect(table, SIGNAL(itemActivated(QTableWidgetItem*)), this, SLOT(onCellActivated(QTableWidgetItem*)));
            _tabslabel.append(group);
            colCount = 0;
        }

        QTableWidget * table = dynamic_cast<QTableWidget*>(this->ui->tabWidget->widget(_tabslabel.indexOf(group)));

        if( ! ( colCount%10 ))
        {
            table->insertRow(table->rowCount());
        }

        QTableWidgetItem *newItem = new QTableWidgetItem();
        QString commandName = command;
        QString iconName=":/data/commands/"+command.replace(QRegExp("[^a-zA-Z]"),"_").replace(QRegExp("([A-Z])"),"-\\1")+".png";
        newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        newItem->setIcon(QIcon(iconName));
        newItem->setData(Qt::StatusTipRole, commandName);
        newItem->setToolTip(commandName);
        //newItem->setText(commandName);
        table->setItem(table->rowCount() - 1, colCount%10, newItem);


        ++colCount;
    }

    db.close();
    //saveCommandsToPng();
}

WidgetInsertCommand::~WidgetInsertCommand()
{
#ifdef DEBUG_DESTRUCTOR
    qDebug()<<"delete WidgetInsertCommand";
#endif
/*    for(int i=0; i < ui->tabWidget->count(); ++i)
    {
        QTableWidget * table = dynamic_cast<QTableWidget*>(this->ui->tabWidget->widget(i));

        table->clear();
        delete table;
    }
*/
    delete ui;
}

void WidgetInsertCommand::onCellActivated(QTableWidgetItem * item)
{
    QString command = item->data(Qt::StatusTipRole).toString();
    if(_widgetTextEdit)
    {
        _widgetTextEdit->insertPlainText(command);
        _widgetTextEdit->setFocus();
    }
    emit commandActivated(command);
}
void WidgetInsertCommand::setParent(WidgetTextEdit *parent)
{
    _widgetTextEdit = parent; QWidget::setParent(parent);
}

void WidgetInsertCommand::saveCommandsToPng()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName("localhost");
    db.setDatabaseName(ConfigManager::Instance.commandDatabaseFilename());
    if(!db.open())
    {
        qDebug()<<"Database commands.sqlite not found : "<<db.lastError();
        return;
    }

    QSqlQuery query("SELECT command, command_group, math_environment FROM commands",db);

    QDir().mkdir("commands");
    QString tempTexFilename = "C:/tmp/tmp.tex";
    QString tempDviFilename = "C:/tmp/tmp.dvi";
    while (query.next()) {
        QString command = query.value(commandIndex).toString().trimmed();
        QString group = query.value(groupIndex).toString().trimmed();
        bool mathEnv = query.value(mathEnvIndex).toInt();
        qDebug()<<command<<" "<<group<<" "<<mathEnv;

        {
            QFile temp(tempTexFilename);
            temp.open(QFile::WriteOnly | QFile::Text);
            temp.write(QString(QString("\\documentclass{article}\\pagestyle{empty}\\usepackage{amsmath,textcomp,amssymb,makeidx,mathrsfs}\\begin{document}")+
                       (mathEnv?QString("$"):QString())+
                       command+
                       (mathEnv?QString("$"):QString())+
                       QString("\\end{document}")).toLatin1().data());
            temp.close();
        }

        QProcess process;
        process.setWorkingDirectory("C:/tmp");
        QString tempPngFilename = QDir().currentPath()+"/commands/"+command.replace(QRegExp("[^a-zA-Z]"),"_").replace(QRegExp("([A-Z])"),"-\\1")+".png";
        QString commandLatex = ConfigManager::Instance.latexCommand().arg(tempTexFilename);
        qDebug()<<commandLatex;
        process.start(commandLatex);
        qDebug()<<process.waitForFinished();
        qDebug()<<process.readAll();
        qDebug()<<"dvipng -T tight -x 1200 -z 9 \""+tempDviFilename+"\" -o \""+tempPngFilename+"\"";
        process.start("dvipng -T tight -x 1200 -z 9 \""+tempDviFilename+"\" -o \""+tempPngFilename+"\"");
        process.waitForFinished();
    }
}
