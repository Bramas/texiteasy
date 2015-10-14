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
#include <QTextCodec>
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
    Builder::setupPathEnvironment(this->process);
    connect(&ConfigManager::Instance, SIGNAL(changed()), this, SLOT(setupPathEnvironment()));
}

Builder::~Builder()
{
#ifdef DEBUG_DESTRUCTOR
    qDebug()<<"delete Builder";
#endif
    process->deleteLater();
}

void Builder::setFile(File *file)
{
    this->file = file;
}

bool Builder::setupPathEnvironment()
{
    return Builder::setupPathEnvironment(this->process);
}

bool Builder::setupPathEnvironment(QProcess * process)
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString defaultPath = ConfigManager::Instance.defaultPathEnvironmentVariable();
#if PORTABLE_EXECUTABLE
    QDir dir(ConfigManager::Instance.applicationPath()+"/"+ConfigManager::Instance.latexPath());
    QString extraPath = dir.absolutePath();
#else
    QString extraPath = ConfigManager::Instance.latexPath();
#endif
#ifdef OS_MAC
    if (extraPath.isEmpty())
    {
        env.insert("PATH", defaultPath + ":/usr/bin:/usr/sbin:/sbin:/usr/local/bin:/usr/texbin:/sw/bin");
    }
    else
    {
        env.insert("PATH", defaultPath + ":/usr/bin:/usr/sbin:/sbin:/usr/local/bin:/usr/texbin:/sw/bin:"+
                   extraPath);
    }
    process->setProcessEnvironment(env);
#endif
#ifdef OS_WINDOWS
    if (!extraPath.isEmpty())
    {
        env.insert("PATH", defaultPath + ";"+extraPath);
        process->setProcessEnvironment(env);
    }
#endif
#ifdef OS_LINUX
    if (!extraPath.isEmpty())
    {
        env.insert("PATH", defaultPath + ":"+extraPath);
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
    if(this->process->state() != QProcess::NotRunning)
    {
        this->process->kill();
        this->process->waitForFinished();
        emit outputUpdated(trUtf8("Canceled"));
        return;
    }
    _commands.clear();
    _basename = this->file->rootBasename();
    process->setWorkingDirectory(this->file->getRootPath());
    if(ConfigManager::Instance.hideAuxFiles())
    {
        _hiddingProcess->setWorkingDirectory(this->file->getPath());
#ifdef OS_WINDOWS
        QDir().mkdir(this->file->getRootPath()+"/.texiteasy");
        command.replace(QRegExp(";((pdf|xe){0,1}(la){0,1}tex(mk){0,1}) "),"\\1 -aux-directory=.texiteasy ");
        command.replace(QRegExp("^((pdf|xe){0,1}(la){0,1}tex(mk){0,1}) "),"\\1 -aux-directory=.texiteasy ");
#else
    #ifdef OS_MAC


    #else
        #ifdef OS_LINX


        #endif
    #endif
#endif
    }
    command.replace(QRegExp("(%1(\\.[a-zA-Z0-9]+){0,1})"),"\"\\1\"");
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
    QDir dir(file->getRootPath());
    QString basename = this->file->rootBasename();
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
    if(this->process->state() != QProcess::NotRunning)
    {
        this->process->kill();
        this->process->waitForFinished();
        return;
    }
    _basename = this->file->rootBasename();

    process->setWorkingDirectory(this->file->getRootPath());
    QString command = ConfigManager::Instance.bibtexCommand().arg(_basename);//.arg(".texiteasy");//this->file->getPath()).arg();//this->file->getAuxPath());
    qDebug()<<command;
    process->start(command);
}

void Builder::onError(QProcess::ProcessError processError)
{
    qDebug()<<process->errorString();
    if(processError == QProcess::Crashed)
    {
        return;
    }
    QMessageBox::warning(0, tr("Erreur"), tr("Erreur")+" "+QString::number(processError)+" : "+trUtf8("La compilation n'a pas pu démarrer."));
}

void Builder::onFinished(int /*exitCode*/, QProcess::ExitStatus exitStatus)
{
    if(exitStatus == QProcess::CrashExit)
    {
        return;
    }
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
    while(this->process->bytesAvailable())
    {
        _lastOutput.append(process->read(1));
    }
/*
    //Old way to do it, but sometimes a character is missing
    QTextCodec* codec = QTextCodec::codecForUtfText("ISO 8859-1");
    QString output = codec->toUnicode(this->process->readAllStandardOutput());
    qDebug()<<output;
    _lastOutput.append(output);
*/
    emit outputUpdated(_lastOutput);
}
void Builder::hideAuxFiles()
{
    if(this->file->getFilename().isEmpty())
    {
        return;
    }

    _hiddingProcess->setWorkingDirectory(this->file->getRootPath());
    QString basename = this->file->rootBasename();
#ifdef OS_WINDOWS
     QString command = QString("attrib +h \".texiteasy\"");
    _hiddingProcess->start(command);
    _hiddingProcess->waitForFinished();
    command = QString("attrib +h \"%1.synctex.gz\"").arg(basename);
    _hiddingProcess->start(command);
    _hiddingProcess->waitForFinished();
    command = QString("attrib +h \"%1.synctex.gz\"").arg(basename);
    _hiddingProcess->start(command);
#else
    #ifdef OS_MAC
    QString command = QString("chflags hidden \"%1.aux\" ; chflags hidden \"%1.bbl\" ; chflags hidden \"%1.blg\" ; chflags hidden \"%1.out\" ; chflags hidden \"%1.log\" ; chflags hidden \"%1.fdb_latexmk\" ; chflags hidden \"%1.nav\" ; chflags hidden \"%1.fls\" ; chflags hidden \"%1.snm\" ;  chflags hidden \"%1.synctex.gz\" ").arg(basename);
    _hiddingProcess->start(command);
    #else
    #ifdef OS_LINUX

    QFile h(file->getPath()+QDir().separator()+".hidden");
    if(h.open(QFile::ReadWrite | QFile::Text))
    {
        QString source = h.readAll();
        if(!source.contains(QString("%1.aux").arg(basename)))
        {
            h.write(QString("\n%1.aux\n%1.bbl\n%1.blg\n%1.out\n%1.log\n%1.fdb_latexmk\n%1.nav\n%1.fls\n%1.snm\n%1.synctex.gz\n").arg(basename).toLatin1().data());
        }

    }


    #endif
    #endif
#endif
}

bool Builder::checkOutput()
{
    if(_lastOutput.indexOf("Database file ") != -1)
    {
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
    if(errorState)
    {
        Builder::Output outputItem;
        outputItem.line = "-1";
        outputItem.message = errorMessage;
        if(errorMessage.contains("@xdblarg"))
        {
            outputItem.message += " A '}' is missing";
        }
        outputItem.type = Builder::Error;
        _simpleOutPut.append(outputItem);
    }
    return _simpleOutPut.isEmpty();
}
