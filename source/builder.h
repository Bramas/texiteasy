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

#ifndef BUILDER_H
#define BUILDER_H
#include <QObject>
#include <QList>
#include <QProcess>
#include <QStringList>

class File;




class Builder : public QObject
{
        Q_OBJECT
public:
    Builder(File * file);
    struct Output{
        QString type;
        QString message;
        QString line;
    };
    ~Builder();
    void setFile(File * file);

    const QList<Builder::Output> & simpleOutput() const { return _simpleOutPut; }
    static QString Error;
    static QString Warning;

public slots:
    void builTex(QString command);
    void bibtex();
    void onFinished(int exitCode,QProcess::ExitStatus exitStatus);
    void onError(QProcess::ProcessError processError);
    void onStandartOutputReady();
    bool setupPathEnvironment();

signals:
    void statusChanged(QString);
    void outputUpdated(QString);
    void pdfChanged();
    void error();
    void success();
    void started();

private:
    bool checkOutput();
    File * file;
    QString _basename;
    QProcess * process;
    QString _lastOutput;
    QStringList _commands;
    QList<Builder::Output> _simpleOutPut;
};

#endif // BUILDER_H
