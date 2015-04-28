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
#include <QList>
#include <QStringList>
#include <QFileInfo>
#include <QDateTime>
#include <QTimer>

#define AUTO_SAVE 40000

class Viewer;
class Builder;
class WidgetTextEdit;
class QTimer;
class WidgetFile;

struct AssociatedFile
{
    typedef enum Type {NONE, BIBTEX, INPUT, FIGURE} Type;
    AssociatedFile::Type type;
    QString filename;

    static AssociatedFile NoAssociation;
};

class File : public QObject
{
    Q_OBJECT
public:
    explicit File(WidgetFile * widgetFile, WidgetTextEdit * widgetTextEdit, QString filename = "");
    ~File();
    typedef enum Format {UNKNOWN, BIBTEX, TEX} Format;
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

    void save(bool recursively = false);
    /**
     * @brief save the file
     * @param filename
     * @param recursively
     * if recursively is true, then all opened associted files are also saved
     */
    void save(QString filename, bool recursively = false);

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
        // convert block to line : + 1
        return this->_lineNumberSinceLastBuild.value(block, block) + 1;
    }

    /**
     * @brief getFilename
     * @return the current full filename
     */
    QString getFilename() const { return this->filename; }

    QString getPath() const { QString s(this->filename); return s.replace(QRegExp("^(([^\\\\\\/]*[\\\\\\/])*)[^\\\\\\/]*$"),"\\1"); }

    QString rootBasename() const { return QFileInfo(rootFilename()).baseName(); }
    QString getRootPath() const { return QFileInfo(rootFilename()).path(); }

    QString rootFilename() const
    {
        return _rootFilename;
    }
    void setRootFilename(QString filename) {
        _rootFilename = filename;
    }
    /**
     * @brief getAuxPath (not used)
     * @return the auxilary directory
     */
    QString getAuxPath() const {
        QDir dir;
        if(!dir.exists(this->getPath()+".texiteasy"))
        {
            dir.mkpath( this->getPath()+".texiteasy");
        }
        return this->getPath()+".texiteasy";//+dir.separator();
    }

    QString getAutoSaveFilename() const
    {
#ifdef OS_WINDOWS
        return this->getPath()+"/~"+this->fileInfo().fileName()+"_autosave";
#else
        return this->getPath()+"/."+this->fileInfo().fileName()+"_autosave";
#endif
    }

    QFileInfo fileInfo() const { return QFileInfo(this->filename); }

    /**
     * @brief getPdfFilename
     * @return the path where the pdf is located (filename but .tex is replaced by .pdf)
     */
    QString getPdfFilename() const {
        QRegExp ext("\\.[a-zA-Z0-9]+$");
        if(this->rootFilename().indexOf(ext) != -1)
        {
            QString s(this->rootFilename()); return s.replace(ext, ".pdf");
        }
        return "";
    }

    Builder * getBuilder() { return this->builder; }

    /**
     * @brief (not used) getViewer
     * @return the Viewer
     */
    Viewer * getViewer() { return this->viewer; }

    QString codec() { return this->_codec; }
    void setCodec(QString codec) { this->_codec = codec; }

    bool isModified() { return this->_modified; }

    bool isUntitled() { return getFilename().isEmpty(); }

    QDateTime lastSaved() const { return _lastSaved; }

    void addOpenAssociatedFile(File * openAssocitedFile);
    Format format() { return _format; }
    const QList<File*> & openAssociatedFiles() { return _openAssociatedFiles; }

    const QList<AssociatedFile> & associatedFiles() const { return _associatedFiles; }
    QStringList bibtexFiles() const;
    AssociatedFile associationWith(QString filename);
    void removeOpenAssociatedFile(File * openAssocitedFile);
    WidgetFile * widgetFile() { return _widgetFile; }

    const QMap<QString, QString> & texDirectives(){ return _texDirectives; }

    void removeAutosaveFile();
public slots:
    /**
     * @brief autoSave
     */
    void autoSave();
    /**
     * @brief setModified, this will send the signal modified(bool)
     */
    void setModified(bool mod = true);

    /**
     * @brief change the codec to UTF-8
     */
    void setUtf8()
    {
        this->_codec = "UTF-8";
    }

    void create(void)
    {
        this->_modified = false;
        this->data=QString("");
        this->filename = QString("");
    }
signals:
    void modified(bool);
private:
    /**
     * @brief lookForAssociatedFiles parse the source, and find if there is some \input{} files or bitex, or figures
     */
    void lookForAssociatedFiles();
    void findTexDirectives();
    QTimer * _autoSaveTimer;
    QList<AssociatedFile> _associatedFiles;
    QList<File*> _openAssociatedFiles;
    Builder * builder;
    QString _codec;
    QString data;
    QString filename;
    QString _rootFilename;
    bool _modified;
    Viewer * viewer;
    WidgetTextEdit * _widgetTextEdit;
    WidgetFile * _widgetFile;
    Format _format;
    QMap<QString, QString> _texDirectives;

    QDateTime _lastSaved;

    QMap<int,int> _lineNumberSinceLastBuild;
};

#endif // FILE_H
