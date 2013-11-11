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
#include <QtGlobal>
#include <QSettings>
#include "configmanager.h"

QString Builder::Error = QObject::tr("Erreur");
QString Builder::Warning = QObject::tr("Warning");

Builder::Builder(File * file) :
    file(file),
    process(new QProcess(this)),
    _hiddingProcess(new QProcess(this))
{
    connect(this->process,SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onFinished(int,QProcess::ExitStatus)));
    connect(this->process,SIGNAL(error(QProcess::ProcessError)), this, SLOT(onError(QProcess::ProcessError)));
    connect(this->process,SIGNAL(readyReadStandardOutput()), this, SLOT(onStandartOutputReady()));
    connect(this->process,SIGNAL(readyReadStandardError()), this, SLOT(onStandartOutputReady()));
    setupPathEnvironment();
    connect(&ConfigManager::Instance, SIGNAL(changed()), this, SLOT(setupPathEnvironment()));
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

bool Builder::setupPathEnvironment()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString extraPath = ConfigManager::Instance.latexPath();
#ifdef OS_MAC
    if (extraPath.isEmpty())
    {
        env.insert("PATH", env.value("PATH") + ":/usr/bin:/usr/sbin:/sbin:/usr/local/bin:/usr/texbin:/sw/bin");
    }
    else
    {
        env.insert("PATH", env.value("PATH") + ":/usr/bin:/usr/sbin:/sbin:/usr/local/bin:/usr/texbin:/sw/bin:"+extraPath);
    }
    process->setProcessEnvironment(env);
#endif
#ifdef OS_WIN
    if (!extraPath.isEmpty())
    {
        env.insert("PATH", env.value("PATH") + ";"+extraPath);
        process->setProcessEnvironment(env);
    }
#endif
#ifdef OS_LINUX
    if (!extraPath.isEmpty())
    {
        env.insert("PATH", env.value("PATH") + ":"+extraPath);
        process->setProcessEnvironment(env);
    }
#endif
    //Very Important, everything is in there because process->setEnvironement is for child process
    return qputenv("PATH", env.value("PATH").toLatin1());
}

void Builder::builTex(QString command)
{
    if(this->file->getFilename().isEmpty())
    {
        return;
    }


    emit started();
    _lastOutput = QString("");
    _simpleOutPut.clear();
    _commands.clear();
    _basename = this->file->fileInfo().baseName();

    if(this->process->state() != QProcess::NotRunning)
    {
        this->process->kill();
    }
    process->setWorkingDirectory(this->file->getPath());
    if(ConfigManager::Instance.hideAuxFiles())
    {
        _hiddingProcess->setWorkingDirectory(this->file->getPath());
#ifdef OS_WINDOWS
        QString commandHide = QString("attrib -h \"%1.aux\"").arg(_basename);
        _hiddingProcess->start(commandHide);
        _hiddingProcess->waitForReadyRead();
        commandHide = QString("attrib -h \"%1.log\"").arg(_basename);
        _hiddingProcess->start(commandHide);
        _hiddingProcess->waitForReadyRead();
        commandHide = QString("attrib -h \"%1.synctex.gz\"").arg(_basename);
        _hiddingProcess->start(commandHide);
        _hiddingProcess->waitForFinished();
#else
    #ifdef OS_MAC


    #else
        #ifdef OS_LINX


        #endif
    #endif
#endif
    }
    command.replace(QRegExp("(%1(\.[a-zA-Z0-9]+){0,1})"),"\"\\1\"");
    command = command.arg(_basename);

    _commands = command.split(';');
    command = QString(_commands.front()).trimmed();
    _commands.pop_front();
    qDebug()<<"start building : "<<command;
    _lastOutput.append(command+"\n\n");
    process->start(command);
}

void Builder::clean()
{
    if(this->file->getFilename().isEmpty())
    {
        return;
    }
    QDir dir(file->getPath());
    QString basename = this->file->fileInfo().baseName();
    dir.remove(basename+".aux");
    dir.remove(basename+".log");
    dir.remove(basename+".synctex.gz");
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
    if(!_commands.isEmpty())
    {
        QString command = QString(_commands.front()).trimmed();
        _commands.pop_front();
        qDebug()<<"continue building with : "<<command;
        _lastOutput.append("\n----------------------------------\n"+command+"\n\n");
        process->start(command);
        return;
    }
    emit statusChanged(QString::fromUtf8("Terminé avec succés"));
    emit success();
    emit pdfChanged();
    if(ConfigManager::Instance.hideAuxFiles())
    {
        this->hideAuxFiles();
    }
}
void Builder::onStandartOutputReady()
{
    QString output = this->process->readAllStandardOutput();
    _lastOutput.append(output);
    output = this->process->readAllStandardError();
    _lastOutput.append(output);
    emit outputUpdated(_lastOutput);
}
void Builder::hideAuxFiles()
{
    if(this->file->getFilename().isEmpty())
    {
        return;
    }

    _hiddingProcess->setWorkingDirectory(this->file->getPath());
    QString basename = this->file->fileInfo().baseName();
#ifdef OS_WINDOWS
    QString command = QString("attrib +h \"%1.aux\"").arg(basename);
    _hiddingProcess->start(command);
    _hiddingProcess->waitForFinished();
    command = QString("attrib +h \"%1.log\"").arg(basename);
    _hiddingProcess->start(command);
    _hiddingProcess->waitForFinished();
    command = QString("attrib +h \"%1.synctex.gz\"").arg(basename);
    _hiddingProcess->start(command);
    _hiddingProcess->waitForFinished();
#else
    #ifdef OS_MAC


    #else
        #ifdef OS_LINX


        #endif
    #endif
#endif
}

bool Builder::checkOutput()
{
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

    return _simpleOutPut.isEmpty();
}
