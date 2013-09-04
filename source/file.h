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

#ifndef FILE_H
#define FILE_H

#include <QObject>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QRegExp>
#include <QMap>
#include <QFileInfo>

class Viewer;
class Builder;
class WidgetTextEdit;
class QTimer;
class File : public QObject
{
    Q_OBJECT
public:
    explicit File(WidgetTextEdit * widgetTextEdit, QString filename = "");

    /**
     * @brief getData
     * @return the QString data
     * call this after open()
     */
    QString getData() { return this->data; }

    /**
     * @brief setData
     * @param data
     * call this before save()
     */
    void setData(QString data) { this->data = data; }

    /**
     * @brief open a file
     * @param filename
     * if filename is empty, the filename given during the constructor is used.
     * if filename is not empty, it will replace the current filename.
     */
    const QString open(QString filename = "", QString codec = "");
    /**
     * @brief save the file
     * @param filename
     */
    void save(QString filename = "");

    /**
     * @brief refreshLineNumber
     */
    void refreshLineNumber();

    /**
     * @brief Insert a line in the document
     * @param lineNumber
     * @param lineCount : how many lines
     * This is used to remeber which have been had since the last build
     */
    void insertLine(int lineNumber, int lineCount = 1);
    /**
     * @brief getBuildedLine : get The line number corresponding to the file when it was builded
     * @param a block number
     * @return the line number corresponding to the file when it was builded
     */
    int getBuildedLine(int block) {
        //qDebug()<<"getLine : "<<block<<" : "<<this->_lineNumberSinceLastBuild.value(block,-1);
        // convert block to line : + 1
        return this->_lineNumberSinceLastBuild.value(block, block) + 1;
    }

    /**
     * @brief getFilename
     * @return the current full filename
     */
    QString getFilename() { return this->filename; }

    /**
     * @brief getPath
     * @return the path of the current file
     */
    QString getPath() { QString s(this->filename); return s.replace(QRegExp("^(([^\\\\\\/]*[\\\\\\/])*)[^\\\\\\/]*$"),"\\1"); }

    /**
     * @brief getAuxPath
     * @return the auxilary directory
     */
    QString getAuxPath() {
        QDir dir;
        if(!dir.exists(this->getPath()+".texiteasy"))
        {
            dir.mkpath( this->getPath()+".texiteasy");
        }
        return this->getPath()+".texiteasy";//+dir.separator();
    }

    QString getAutoSaveFilename()
    {
        QDir dir;
        return this->getAuxPath()+dir.separator()+this->fileInfo().baseName()+"_autosave.tex";
    }

    /**
     * @brief fileInfo
     * @return the file info
     */
    QFileInfo fileInfo() const { return QFileInfo(this->filename); }

    /**
     * @brief getPdfFilename
     * @return the path where the pdf is located (filename but .tex is replaced by .pdf)
     */
    QString getPdfFilename(){
        QString s(this->filename); return s.replace(QRegExp("\\.tex$"),".pdf");
    }

    /**
     * @brief getBuilder
     * @return the Builder used to compile the file
     */
    Builder * getBuilder() { return this->builder; }

    /**
     * @brief (not used) getViewer
     * @return the Viewer
     */
    Viewer * getViewer() { return this->viewer; }

    QString codec() { return this->_codec; }

    bool isModified() { return this->_modified; }


public slots:
    /**
     * @brief autoSave
     */
    void autoSave();
    /**
     * @brief setModified
     */
    void setModified(bool mod = true) {
        this->_modified = mod;
    }
    void create(void)
    {
        this->_modified = false;
        this->data=QString("");
        this->filename = QString("");
    }

private:
    QTimer * _autoSaveTimer;
    Builder * builder;
    QString _codec;
    QString data;
    QString filename;
    bool _modified;
    Viewer * viewer;
    WidgetTextEdit * _widgetTextEdit;

    QMap<int,int> _lineNumberSinceLastBuild;
};

#endif // FILE_H
