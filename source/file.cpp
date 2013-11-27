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

#include "file.h"
#include "builder.h"
#include "viewer.h"
#include "widgettextedit.h"
#include "filemanager.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QTextCodec>
#include <QTimer>
#include <QDebug>
#include <QRegExp>
#include <QMessageBox>

#define AUTO_SAVE 120000

AssociatedFile AssociatedFile::NoAssociation = { AssociatedFile::NONE, QString()};

File::File(WidgetFile *widgetFile, WidgetTextEdit* widgetTextEdit, QString filename) :
    _autoSaveTimer(new QTimer),
    builder(new Builder(this)),
    _codec(QTextCodec::codecForLocale()->name()),
    filename(filename),
    _modified(false),
    viewer(new Viewer(this)),
    _widgetTextEdit(widgetTextEdit),
    _widgetFile(widgetFile)
{
    _format = UNKNOWN;
    connect(_autoSaveTimer, SIGNAL(timeout()), this, SLOT(autoSave()));
}
File::~File()
{
#ifdef DEBUG_DESTRUCTOR
    qDebug()<<"delete File";
#endif
    removeAutosaveFile();
    delete builder;
    delete _autoSaveTimer;
}
void File::removeAutosaveFile()
{
    QFile f(getAutoSaveFilename());
    if(f.exists())
    {
        f.remove();
    }
}

void File::save(bool recursively)
{
    this->save(QString(""), recursively);
}

void File::save(QString filename, bool recursively)
{
    //Check the filename
    if(!filename.isEmpty())
    {
        this->filename = filename;
    }
    if(this->filename.isEmpty())
    {
        return;
    }
    if(!filename.isEmpty() && !this->_texDirectives.contains("root"))
    {
        this->setRootFilename(this->filename);
    }
    if(_modified)
    {
        this->data = this->_widgetTextEdit->toPlainText();
    }
    // Save
    QFile file(this->filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out.setCodec(_codec.toLatin1());
    out << this->data;
    file.close();

    if(recursively)
    {
        foreach(File* associtedFile, _openAssociatedFiles)
        {
            associtedFile->save(true);
        }
    }
    removeAutosaveFile();

    this->setModified(false);
    _lastSaved = this->fileInfo().lastModified();
    _autoSaveTimer->stop();
    _autoSaveTimer->start(AUTO_SAVE);
}

const QString File::open(QString filename, QString codec)
{
    // Get the filename

    if(filename.isEmpty())
    {
        if(this->filename.isEmpty())
        {

            //this->filename = QFileDialog::getOpenFileName(this->_parent,tr("Ouvrir un fichier"));
            if(this->filename.isEmpty())
            {
                return QString("");
            }
        }
    }
    else
    {
        this->filename = filename;
    }

    QString loadedFilename = this->filename;
    bool autosaveLoad = false;

    if(QFile(getAutoSaveFilename()).exists())
    {
        if(0 == QMessageBox::warning(0, trUtf8("auto savefile exists"),
                             trUtf8("An autosave file of %1 exists (maybe caused by a wrong shutdown of %2). Do you want to load the autosave?").arg(fileInfo().fileName()).arg(APPLICATION_NAME),
                             trUtf8("Load the autosave"),
                             trUtf8("Ignore")))
        {
            loadedFilename = getAutoSaveFilename();
            autosaveLoad = true;
        }

    }

    // Open the file

    QFile file(loadedFilename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString("");

    this->data = QString("");

    QTextStream in(&file);
    if(codec.isEmpty())
    {
        in.setCodec("UTF-8");
    }
    else
    {
        in.setCodec(codec.toLatin1());
    }

    data = in.readAll();

    this->findTexDirectives(); // find directive before look for associative files
    if(codec.isEmpty() && _texDirectives.contains("encoding"))
    {
        codec = _texDirectives.value("encoding");
        if(codec.compare("utf8", Qt::CaseInsensitive) && codec.compare("utf-8", Qt::CaseInsensitive) && codec.compare("utf 8", Qt::CaseInsensitive))
        {
            qDebug()<<"reload "<<codec;
            return this->open(this->filename, codec);
        }
    }

    if(codec.isEmpty() && data.contains(QString::fromUtf8("�")))
    {
        this->open(this->filename,"ISO 8859-1");
        return QString("");
    }
    this->_codec = in.codec()->name();

    if(!fileInfo().suffix().compare("tex"))
    {
        this->_format = TEX;
    }
    else if(!fileInfo().suffix().compare("bib"))
    {
        this->_format = BIBTEX;
    }

    this->_widgetTextEdit->setText(this->data);
    this->lookForAssociatedFiles();


    //Update Root Filename;
    if(!_texDirectives.contains("root"))
    {
          setRootFilename(getFilename());
    }
    else
    {
        QString rootfile = _texDirectives.value("root");
        QDir dir(rootfile);
        if(!dir.isAbsolute())
        {
            rootfile = getPath()+rootfile;
        }
        if(rootfile.right(4).compare(".tex"))
        {
            rootfile += ".tex";
        }
        setRootFilename(rootfile);
    }


    this->refreshLineNumber();
    this->setModified(autosaveLoad);
    _autoSaveTimer->stop();
    _autoSaveTimer->start(AUTO_SAVE);
    _lastSaved = this->fileInfo().lastModified();
    return this->data;

}

void File::refreshLineNumber()
{
    int lineNumber = this->_widgetTextEdit->document()->blockCount();
    //qDebug()<<"refreshLineNumber "<<lineNumber;
    _lineNumberSinceLastBuild.clear();
    for(int idx = 0; idx < lineNumber; ++idx)
    {
        _lineNumberSinceLastBuild.insert(idx,idx);
    }
}
void File::insertLine(int lineNumber, int lineCount)
{
    //qDebug()<<"insertLine "<<lineNumber<<" : "<<lineCount;
    for(int idx = lineNumber; idx < _lineNumberSinceLastBuild.size(); ++idx)
    {
        _lineNumberSinceLastBuild.insert(idx,_lineNumberSinceLastBuild.value(idx) - lineCount);
    }

    for(int idx = 0; idx < lineCount; ++idx)
    {
        _lineNumberSinceLastBuild.insert(_lineNumberSinceLastBuild.size(),_lineNumberSinceLastBuild.value(_lineNumberSinceLastBuild.size()-1)+1);
    }
    for(int idx = 0; idx > lineCount; --idx)
    {
        _lineNumberSinceLastBuild.remove(_lineNumberSinceLastBuild.size()-1);
    }
}

void File::autoSave()
{
    if(this->filename.isEmpty())
    {
        return;
    }
    if(!_modified)
    {
       return;
    }

    QFile file(this->getAutoSaveFilename());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;


    this->data = this->_widgetTextEdit->toPlainText();

    QTextStream out(&file);
    out.setCodec(_codec.toLatin1());
    //out.setGenerateByteOrderMark(true);
    out << this->data;
    file.close();
#ifdef OS_WINDOWS
    QProcess::execute(QString("attrib +h \"%1\"").arg(this->getAutoSaveFilename()));
#endif
}
void File::findTexDirectives()
{
    if(this->data.isEmpty())
    {
        return;
    }
    _texDirectives.clear();
    QRegExp directivePattern("%[ ]*\\![ ]*TEX[ ]*([a-z\\-]+)[ ]*=([^\\n]+)\\n", Qt::CaseInsensitive);
    int index = -1;
    while(-1 != (index = this->data.indexOf(directivePattern, index + 1)))
    {
        QString directiveName = directivePattern.capturedTexts().at(1);
        QString directiveValue = directivePattern.capturedTexts().at(2);
        directiveValue.replace("TS-","",Qt::CaseInsensitive);
        _texDirectives.insert(directiveName.trimmed().toLower(), directiveValue.trimmed());
    }
}

void File::lookForAssociatedFiles()
{
    if(this->data.isEmpty())
    {
        return;
    }
    _associatedFiles.clear();

    // Look for bibtex Files
    QRegExp patternBib("\\\\bibliography\\{([^\\}]*)\\}");
    int index = this->data.indexOf(patternBib);
    if(index != -1)
    {
        QString bibFilename = this->getPath();

        bibFilename += patternBib.capturedTexts().last();
        bibFilename += ".bib";
        if(QFile::exists(bibFilename))
        {
            AssociatedFile asso;
            asso.type = AssociatedFile::BIBTEX;
            asso.filename = bibFilename;
            _associatedFiles.append(asso);
        }
    }

    // Look for input tex Files
    QRegExp patternInput("\\\\input\\{([^\\}]*)\\}");
    index = -1;
    while((index = this->data.indexOf(patternInput, index + 1)) != -1)
    {
        QString texFilename = this->getPath();
        texFilename += patternInput.capturedTexts().last();
        if(!texFilename.contains(QRegExp("\\.[a-zA-Z]{1,4}$")))
        {
            texFilename += ".tex";
        }
        if(QFile::exists(texFilename))
        {
            AssociatedFile asso;
            asso.type = AssociatedFile::INPUT;
            asso.filename = texFilename;
            _associatedFiles.append(asso);
        }
    }
}

QStringList File::bibtexFiles() const
{
    QStringList list;
    foreach(AssociatedFile file, _associatedFiles)
    {
        if(file.type == AssociatedFile::BIBTEX)
        {
            list.append(file.filename);
        }
    }
    return list;
}
AssociatedFile File::associationWith(QString filename)
{
    foreach(AssociatedFile assoc, _associatedFiles)
    {
        if(!assoc.filename.compare(filename))
        {
            return assoc;
        }
    }
    return AssociatedFile::NoAssociation;
}
void File::addOpenAssociatedFile(File * openAssocitedFile)
{
    _openAssociatedFiles.append(openAssocitedFile);
}
void File::removeOpenAssociatedFile(File * openAssocitedFile)
{
    _openAssociatedFiles.removeAll(openAssocitedFile);
}

