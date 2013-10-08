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
    content += ".</center><br><br>";
    content += "Copyright (c) 2013 by Quentin BRAMAS<br>"
            "<br>"
            "Project home site : <a href='http://texiteasy.com'>texiteasy.com</a><br>"
            "<br>"
            "This program is licensed to you under the terms of the GNU General Public License Version 3 as published by the Free Software Foundation."
            "<br>"
            "<br>"
            "TexitEasy contains code from :<br>"
            "- SyncTeX ( <a href='http://itexmac.sourceforge.net/SyncTeX.html'>url</a> (C) Jerome Laurens - Licence: GPL)<br>"
            "- Texmaker ( <a href='http://www.xm1math.net/texmaker/'>url</a> (C) Pascal Brachet - Licence: GPL)";

    this->ui->textEdit->setReadOnly(true);
    this->ui->textEdit->setOpenExternalLinks(true);
    this->ui->textEdit->setHtml(content);

}

DialogAbout::~DialogAbout()
{
    delete ui;
}
