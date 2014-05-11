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

        // Operators

addList<<" INSERT INTO commands (command, command_group) VALUES ('\\pm', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\diamond', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\oplus', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\mp', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\bigtriangleup', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\ominus', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\times', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\uplus', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\bigtriangledown', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\otimes', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\div', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\sqcap', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\triangleleft', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\oslash', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\odot', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\triangleright', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\sqcup', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\ast', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\star', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\vee', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\circ', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\bigcirc', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\setminus', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\dagger', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\bullet', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\wedge', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\ddagger', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\wr', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\cdot', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\amalg', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\models', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\equiv', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\geq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\leq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\prec', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\succ', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\sim', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\perp', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\mid', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\simeq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\succeq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\preceq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\ll', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\gg', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\asymp', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\parallel', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\bowtie', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\approx', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\supset', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\subset', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\subseteq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\supseteq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\cong', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\neq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\doteq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\sqsupseteq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\sqsubseteq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\smile', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\frown', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\in', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\ni', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\propto', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('>', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('<', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\dashv', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\vdash', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\sum', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\bigcap', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\bigodot', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\prod', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\bigsqcup', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\coprod', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\bigotimes', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\bigcup\bigoplus', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\int', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\bigvee', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\biguplus', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\geqslant', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\leqslant', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\bigwedge', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\oint', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\lhd', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\rhd', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\unlhd', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\unrhd', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\dotplus', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\sqsupset', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\sqsubset', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\Join', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\smallsetminus', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\Cap', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\Cup', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\barwedge', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\boxtimes', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\boxminus', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\doublebarwedge', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\veebar', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\boxdot', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\boxplus', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\divideontimes', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\ltimes', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\curlywedge', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\rightthreetimes', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\rtimes', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\leftthreetimes', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\curlyvee', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\circleddash', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\circledast', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\circledcirc', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\eqslantless', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\leqq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\intercal', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\centerdot', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\lll', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\lesssim', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\lessapprox', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\lessgtr', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\approxeq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\lesseqgtr', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\lessdot', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\lesseqqgtr', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\backsim', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\fallingdotseq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\risingdotseq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\doteqdot', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\backsimeq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\subseteqq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\Subset', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\preccurlyeq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\vartriangleleft', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\precapprox', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\precsim', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\curlyeqprec', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\trianglelefteq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\vDash', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\Vvdash', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\smallsmile', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\geqq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\Bumpeq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\bumpeq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\smallfrown', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\eqslantgtr', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\gtrsim', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\gtrapprox', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\gtrdot', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\gtreqqless', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\gtreqless', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\gtrless', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\ggg', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\eqcirc', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\circeq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\triangleq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\thicksim', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\succcurlyeq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\vartriangleright', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\shortparallel', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\blacktriangleleft', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\Supset', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\succapprox', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\shortmid', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\varpropto', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\pitchfork', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\Vdash', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\succsim', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\supseteqq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\thickapprox', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\curlyeqsucc', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\trianglerighteq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\between', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\therefore', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\backepsilon', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\blacktriangleright', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\because', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nleqq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nleqslant', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nleq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nless', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\lnapprox', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\lneq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nprec', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\lneqq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\npreceq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\lvertneqq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\lnsim', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\precnsim', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nmid', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\ntrianglelefteq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\ntriangleleft', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nshortmid', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nsim', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nvDash', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\precnapprox', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nvdash', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nsubseteq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\subsetneq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\varsubsetneq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\subsetneqq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\ngeqslant', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\ngeq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\ngtr', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\varsubsetneqq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\ngeqq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\gneq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\gneqq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\gvertneqq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nsucceq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nsucc', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\gnapprox', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\gnsim', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\succnsim', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\succnapprox', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\ncong', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nshortparallel', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\ntrianglerighteq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\ntriangleright', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nVDash', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nparallel', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nsupseteq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\nsupseteqq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\supsetneq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\varsupsetneq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\varsupsetneqq', 'operators');";
addList<<" INSERT INTO commands (command, command_group) VALUES ('\\supsetneqq', 'operators');";



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
#ifdef OS_WIN
    QString tempTexFilename = "C:/tmp/tmp.tex";
    QString tempDviFilename = "C:/tmp/tmp.dvi";
#else
    QString tempTexFilename = "/tmp/tmp.tex";
    QString tempDviFilename = "/tmp/tmp.dvi";
#endif
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
#ifdef OS_WIN
        process.setWorkingDirectory("C:/tmp");
#else
        process.setWorkingDirectory("/tmp");
#endif
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
