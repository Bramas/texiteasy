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

#include "builder.h"
#include "file.h"
#include <QMessageBox>
#include <QDebug>
#include <QSettings>
#include "configmanager.h"

QString Builder::Error = QObject::tr("Erreur");
QString Builder::Warning = QObject::tr("Warning");

Builder::Builder(File * file) :
    file(file),
    process(new QProcess())
{
    connect(this->process,SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onFinished(int,QProcess::ExitStatus)));
    connect(this->process,SIGNAL(error(QProcess::ProcessError)), this, SLOT(onError(QProcess::ProcessError)));
    connect(this->process,SIGNAL(readyReadStandardOutput()), this, SLOT(onStandartOutputReady()));
}

Builder::~Builder()
{
#ifdef DEBUG_DESTRUCTOR
    qDebug()<<"delete Builder";
#endif
    delete process;
}

void Builder::setFile(File *file)
{
    this->file = file;
}

void Builder::pdflatex()
{
    if(this->file->getFilename().isEmpty())
    {
        return;
    }
    emit started();
    _lastOutput = QString("");
    _simpleOutPut.clear();
    _basename = this->file->fileInfo().baseName();

    QSettings settings;
    process->setWorkingDirectory(this->file->getPath());
    //QString command = settings.value("latexPath").toString()+pdflatexExe+" -output-directory=\""+this->file->getPath()+"\" -aux-directory="+this->file->getAuxPath()+" -synctex=1 -shell-escape -interaction=nonstopmode -enable-write18 \""+this->file->getFilename()+"\"";
    QString command = ConfigManager::Instance.pdflatexCommand(true).arg(_basename);//.arg(this->file->getPath()).arg(this->file->getAuxPath());
    qDebug()<<"start pdflatex : "<<command;
    if(this->process->state() != QProcess::NotRunning)
    {
        this->process->kill();
    }
    process->start(command);
}

void Builder::bibtex()
{
    if(this->file->getFilename().isEmpty())
    {
        return;
    }
    emit started();
    QSettings settings;
    _lastOutput = QString("");
    _simpleOutPut.clear();
    _basename = this->file->fileInfo().baseName();

    process->setWorkingDirectory(this->file->getPath());
    QString command = ConfigManager::Instance.bibtexCommand(true).arg(_basename);//.arg(".texiteasy");//this->file->getPath()).arg();//this->file->getAuxPath());
    qDebug()<<command;
    process->start(command);
    //process->start(settings.value("latexPath").toString()+"bibtex --include-directory=\""+this->file->getPath()+"\" \""+this->file->getAuxPath()+"/"+_basename+"\"");
}

void Builder::onError(QProcess::ProcessError processError)
{
    qDebug()<<process->errorString();
    QMessageBox::warning(0, tr("Erreur"), tr("Erreur")+" "+QString::number(processError)+" : "+trUtf8("La compilation n'a pas pu démarrer."));
}

void Builder::onFinished(int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/)
{
    //qDebug()<<_lastOutput;
    this->file->refreshLineNumber();
    if(!checkOutput())
    {
        emit error();
        emit statusChanged(QString::fromUtf8("Terminé avec des erreurs"));
        return;
    }
    emit statusChanged(QString::fromUtf8("Terminé avec succés"));
    emit success();
    emit pdfChanged();
}
void Builder::onStandartOutputReady()
{
    QString output = this->process->readAllStandardOutput();
    _lastOutput.append(output);
    emit outputUpdated(_lastOutput);
}

bool Builder::checkOutput()
{
    if(_lastOutput.indexOf("Output written on ") != -1)
    {
        //_simpleOutPut << "Output written on \""+this->_basename+".pdf\"";
        return true;
    }
    if(_lastOutput.indexOf("Database file ") != -1)
    {
        //_simpleOutPut << "Success";
        return true;
    }


    QStringList lines = _lastOutput.split('\n');
    QString errorMessage;
    bool errorState = false;
    bool firsLineError = true;

    foreach(const QString &line, lines)
    {
        if(!errorState && line.length() && line.at(0) == QChar('!'))
        {
            errorMessage = line;
            errorState = true;
            firsLineError = false;
        }
        else
        if(errorState)
        {
            if(firsLineError)
            {

            }
            if(!line.length())
            {
                firsLineError = false;
            }
            else
            if(line.at(0) == QChar('l'))
            {
                Builder::Output outputItem;

                QString lineCopy = line;
                lineCopy.replace(QRegExp("^l\\.([0-9]+).*$"),"\\1");
                outputItem.line = lineCopy;
                outputItem.type = Builder::Error;
                lineCopy = line;
                lineCopy.replace(QRegExp("^l\\.[0-9]+(.*)$"),"\\1");
                outputItem.message = errorMessage+lineCopy;
                _simpleOutPut.append(outputItem);

                errorState = false;
            }
        }
    }

    return false;
}
