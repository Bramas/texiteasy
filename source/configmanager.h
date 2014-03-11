/***************************************************************************
 *   copyright       : (C) 2013 by Quentin BRAMAS                          *
 *   http://texiteasy.com                                                  *
 *                                                                         *
 *   This file is part of texiteasy.                                       *
 *                                                                         *
 *   texiteasy is free software: you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   texiteasy is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with texiteasy.  If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H


#include <QObject>
#include <QMap>
#include <QString>
#include <QTextCharFormat>
#include <QSettings>
#include <QDebug>
#include <QMutex>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>

class QWidget;

class ConfigManager : public QObject
{
    Q_OBJECT

public:

    static ConfigManager Instance;

    QTextCharFormat getTextCharFormats(QString key = QString("normal"))
    {
        //_charFormatMutex.lock();
        QTextCharFormat format(this->textCharFormats->value(key,this->textCharFormats->value("normal")));
        //_charFormatMutex.unlock();
        return format;
    }

    ~ConfigManager();

    QString settingsPath() const { return _settingsPath; }

    void setDevicePixelRatio(qreal ratio) { _devicePixelRatio = ratio; }
    qreal devicePixelRatio() { return _devicePixelRatio; }

    static const QStringList DefaultLatexCommandNames;
    static const QStringList DefaultLatexCommands;

    int tabWidth() { QSettings settings; return settings.value("tabWidth", 4).toInt(); }
    void setTabWidth(int tabW) { QSettings settings; settings.setValue("tabWidth", tabW); emit tabWidthChanged(); }

    bool isUsingSpaceIndentation() { QSettings settings; return settings.value("spaceIndentation", true).toBool(); }
    void setUsingSpaceIndentation(bool use) { QSettings settings; settings.setValue("spaceIndentation", use); }
    QString tabToString()
    {
        QSettings settings;
        if(settings.value("spaceIndentation", true).toBool())
        {
            return QString().fill(' ',settings.value("tabWidth", 4).toInt());
        }
        return QString("\t");
    }

    bool darkTheme() { return 120 > this->getTextCharFormats().background().color().value(); }

    void changePointSizeBy(int delta);
    void setPointSize(int size);
    void setReplaceDefaultFont(bool replace);
    bool isDefaultFontReplaced() { QSettings settings; return settings.value("theme/replaceDefaultFont").toBool(); }
    int pointSize() { QSettings settings; return settings.value("theme/pointSize").toInt(); }
    void setFontFamily(QString family);

    void            setMainWindow(QWidget * mainWindow);
    void            save(void);
    bool            load(QString theme = QString());
    QString         colorToString(const QColor & color) { return "rgb("+QString::number(color.red())+","+QString::number(color.green())+","+QString::number(color.blue())+")";}

    QString         textCharFormatToString(QTextCharFormat charFormat, QTextCharFormat defaultFormat = QTextCharFormat());
    QTextCharFormat stringToTextCharFormat(QString string, QTextCharFormat defaultFormat = QTextCharFormat());

    void            setOpenFilesWhenClosing(QStringList files, QStringList fileCursorPositions, int tabIndex)
    {
        QSettings settings;
        settings.setValue("openTabIndexWhenClosing", tabIndex);
        settings.setValue("openFilesWhenClosing", files);
        settings.setValue("openFileCursorPositionsWhenClosing", fileCursorPositions);
    }
    int     openTabIndexWhenClosing()
    {
        QSettings settings;
        return settings.value("openTabIndexWhenClosing", 0).toInt();
    }
    QStringList     openFilesWhenClosing()
    {
        QSettings settings;
        return settings.value("openFilesWhenClosing").toStringList();
    }
    QStringList     openFileCursorPositionsWhenClosing()
    {
        QSettings settings;
        return settings.value("openFileCursorPositionsWhenClosing").toStringList();
    }
    void            setOpenLastSessionAtStartup(bool open) { QSettings settings; settings.setValue("openLastSessionAtStartup", open); }
    bool            openLastSessionAtStartup() { QSettings settings; return settings.value("openLastSessionAtStartup", true).toBool(); }


    bool            isDollarAuto() {  QSettings settings; return settings.value("dollarAuto", true).toBool();  }
    void            setDollarAuto(bool b) {  QSettings settings; settings.setValue("dollarAuto", b);  }

    QStringList     themesList();
    const QString&  theme() { return _theme; }
    QString         themePath();

    QStringList     languagesList();
    QString         language()                      { QSettings settings; return settings.value("language").toString(); }
    void            setLanguage(QString language)   { QSettings settings; settings.setValue("language", language); applyTranslation(); }
    void            applyTranslation();

    QStringList     dictionnaries();
    void            setDictionary(QString dico )    { QSettings settings; settings.setValue("defaultDictionary", dico); }
    QString         currentDictionaryFilename()     { return dictionaryPath()+currentDictionary(); }
    QString         currentDictionary()
    {
        QSettings settings;
        QString dico = settings.value("defaultDictionary").toString();
        if(dictionnaries().contains(dico))
        {
            return dico;
        }
        return NoDictionnary;
    }
    static QString NoDictionnary;
    /**
     * @brief dictionnaryPath end with a separator character
     * @return
     */
    QString         dictionaryPath();
    void            addToDictionnary(QString dico, QString word);
    QStringList     userDictionnary(QString dico);
    QString popplerVersion();

    void checkRevision();
    void recursiveCopy(QString from, QString to, QFile::Permissions permission);

    bool hideAuxFiles() { QSettings settings; return settings.value("builder/hideAuxFiles", true).toBool(); }
    void setHideAuxFiles(bool hide) { QSettings settings; settings.setValue("builder/hideAuxFiles", hide); }

    QStringList latexCommandNames()
                {  QSettings settings; return settings.value("builder/latexCommandNames").toStringList();  }
    void        setLatexCommandNames(QStringList list)
                {  QSettings settings; settings.setValue("builder/latexCommandNames", list);  }
    QStringList latexCommands()
                {  QSettings settings; return settings.value("builder/latexCommands").toStringList();  }
    void    setLatexCommands(QStringList list)
                {  QSettings settings; settings.setValue("builder/latexCommands", list);  }

    void    setDefaultLatex(QString name) {  QSettings settings; settings.setValue("builder/defaultLatex", name);  }
    QString defaultLatex() {  QSettings settings; return settings.value("builder/defaultLatex").toString();  }
    QString latexCommand(QString name = QString::null);


    QString bibtexCommand(bool fullPath = false) { QSettings settings; return (fullPath ? settings.value("builder/latexPath").toString() : QString(""))+settings.value("builder/bibtex").toString(); }
    QString pdflatexCommand(bool fullPath = false) { QSettings settings; return (fullPath ? settings.value("builder/latexPath").toString() : QString(""))+settings.value("builder/pdflatex").toString(); }
    QString latexPath() { QSettings settings; return settings.value("builder/latexPath").toString(); }
    QString applicationPath() { return _applicationPath; }

    QString commandDatabaseFilename() { QSettings settings; return settings.value("commandDatabaseFilename").toString(); }

    void setBibtexCommand(QString command) { QSettings settings; settings.setValue("builder/bibtex", command); }
    void setPdflatexCommand(QString command) { QSettings settings; settings.setValue("builder/pdflatex", command); }
    void setLatexPath(QString path) { QSettings settings; settings.setValue("builder/latexPath", path); }

    bool isPdfSynchronized() { QSettings settings; return settings.value("pdfSynchronized", true).toBool(); }

    bool pdfViewerInItsOwnWidget() { QSettings settings; return settings.value("pdfViewerItsOwnWidget", false).toBool(); }

    bool splitEditor() { QSettings settings; return settings.value("splitEditor", false).toBool(); }

    bool isThisVersionHaveToBeReminded(QString version);
    void dontRemindMeThisVersion(QString version);

    QString lastFolder() { QSettings settings; return settings.value("lastFolder").toString(); }


    QString         macrosPath();


    static QString Extensions;
    static QString MacroSuffix;
    static const QStringList CodecsAvailable;
    QString dataLocation();

    void init(QString in_applicationPath);
public slots:
    /**
     * @brief signalVersionIsOutdated
     * call this function to emit the versionIsOutdated() signal
     */
    void signalVersionIsOutdated() { emit versionIsOutdated(); }

    void setPdfSynchronized(bool pdfSynchronized) { QSettings settings; settings.setValue("pdfSynchronized", pdfSynchronized); }
    void setPdfViewerInItsOwnWidget(bool b) { QSettings settings; settings.setValue("pdfViewerItsOwnWidget", b); }
    void setSplitEditor(bool split) { QSettings settings; settings.setValue("splitEditor", split); }
    void openThemeFolder();
    void openUpdateWebsite() { QString link = TEXITEASY_UPDATE_WEBSITE;
                               QDesktopServices::openUrl(QUrl(link)); }

    void sendChangedSignal() { emit changed(); }
signals:

    /**
     * @brief versionIsOutdated [signal]
     * emitted when we detect that the version is outdated
     */
    void versionIsOutdated();
    void changed();
    void tabWidthChanged();
private:
    void replaceDefaultFont();
    ConfigManager();

    qreal _devicePixelRatio;
    QApplication * _application;
    QMutex _charFormatMutex;
    QWidget * mainWindow;
    QMap<QString,QTextCharFormat> * textCharFormats;
    QString _theme;
    QString _pdflatexExe;
    QString _settingsPath;
    QString _applicationPath;
};



#endif // CONFIGMANAGER_H
