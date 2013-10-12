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

#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H


#include <QObject>
#include <QMap>
#include <QString>
#include <QTextCharFormat>
#include <QSettings>
#include <QDebug>
#include <QMutex>


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

    bool darkTheme() { return 120 > this->getTextCharFormats().background().color().value(); }

    void changePointSizeBy(int delta);
    void setPointSize(int size);
    void setReplaceDefaultFont(bool replace);
    bool isDefaultFontReplaced() { QSettings settings; return settings.value("theme/replaceDefaultFont").toBool(); }
    int pointSize() { QSettings settings; return settings.value("theme/pointSize").toInt(); }
    void setFontFamily(QString family);

    void setMainWindow(QWidget * mainWindow);
    void save(void);
    bool load(QString theme = QString());
    QString colorToString(const QColor & color) { return "rgb("+QString::number(color.red())+","+QString::number(color.green())+","+QString::number(color.blue())+")";}

    QString textCharFormatToString(QTextCharFormat charFormat, QTextCharFormat defaultFormat = QTextCharFormat());
    QTextCharFormat stringToTextCharFormat(QString string, QTextCharFormat defaultFormat = QTextCharFormat());

    QStringList themesList();
    const QString& theme() { return _theme; }

    void checkRevision();

    QString bibtexCommand(bool fullPath = false) { QSettings settings; return (fullPath ? settings.value("builder/latexPath").toString() : QString(""))+settings.value("builder/bibtex").toString(); }
    QString pdflatexCommand(bool fullPath = false) { QSettings settings; return (fullPath ? settings.value("builder/latexPath").toString() : QString(""))+settings.value("builder/pdflatex").toString(); }
    QString latexCommand(bool fullPath = false) { QSettings settings; return (fullPath ? settings.value("builder/latexPath").toString() : QString(""))+settings.value("builder/latex").toString(); }
    QString latexPath() { QSettings settings; return settings.value("builder/latexPath").toString(); }

    QString commandDatabaseFilename() { QSettings settings; return settings.value("commandDatabaseFilename").toString(); }

    void setBibtexCommand(QString command) { QSettings settings; settings.setValue("builder/bibtex", command); }
    void setPdflatexCommand(QString command) { QSettings settings; settings.setValue("builder/pdflatex", command); }
    void setLatexPath(QString path) { QSettings settings; settings.setValue("builder/latexPath", path); }

    bool isPdfSynchronized() { QSettings settings; return settings.value("pdfSynchronized", true).toBool(); }

    void init();
public slots:
    void setPdfSynchronized(bool pdfSynchronized) { QSettings settings; settings.setValue("pdfSynchronized", pdfSynchronized); }
    void openThemeFolder();

private:
    void replaceDefaultFont();
    ConfigManager();

    QMutex _charFormatMutex;
    QWidget * mainWindow;
    QMap<QString,QTextCharFormat> * textCharFormats;
    QString _theme;
    QString _pdflatexExe;
};



#endif // CONFIGMANAGER_H
