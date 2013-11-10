#include "dialogabout.h"
#include "ui_dialogabout.h"
#include "configmanager.h"
DialogAbout::DialogAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAbout)
{
    ui->setupUi(this);
    QString content("<center><strong>");
    content += APPLICATION_NAME;
    content += ". Version ";
    content += CURRENT_VERSION;
    content += ".<br>";
    content += "Copyright (c) 2013 by Quentin BRAMAS</strong><br>"
            "<a href='http://texiteasy.com'>texiteasy.com</a></center>"
            "<br>"
            "This program is licensed to you under the terms of the GNU General Public License Version 3 as published by the Free Software Foundation."
            "<br>"
            "<br>"
            "Compiled With:<br>"
            "- Qt "
            QT_VERSION_STR
            ", Licence GPL<br>"
            "- Poppler "+ConfigManager::Instance.popplerVersion()+", Licence GPL<br>"
            "<br>"
            "<br>"
            "TexitEasy contains code from :<br>"
            "- SyncTeX ( <a href='http://itexmac.sourceforge.net/SyncTeX.html'>url</a> (C) Jerome Laurens - Licence: GPL)<br>"
            "- Texmaker ( <a href='http://www.xm1math.net/texmaker/'>url</a> (C) Pascal Brachet - Licence: GPL)<br>"
            "- Hunspell ( <a href='http://hunspell.sourceforge.net/'>url</a> - Licence GPL)";

    this->ui->textEdit->setReadOnly(true);
    this->ui->textEdit->setOpenExternalLinks(true);
    this->ui->textEdit->setHtml(content);

}

DialogAbout::~DialogAbout()
{
    delete ui;
}
