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

#include "configmanager.h"
#include <QFont>
#include <QColor>
#include <QSettings>
#include <QDesktopServices>
#include <QProcess>
#include <QFileDialog>
#include <QTranslator>
#include <QApplication>
#if QT_VERSION < 0x050000
    #include <QDesktopServices>
#else
    #include <QStandardPaths>
#endif

#include <QMapIterator>
#include <QDir>
#include <QUrl>
#include <QMessageBox>
#include <QDebug>
#include <QCoreApplication>
#include <QRegExp>

#define DEBUG_THEME_PARSER(a)

ConfigManager ConfigManager::Instance;

QString ConfigManager::Extensions = trUtf8("Latex (*.tex *.latex);;BibTex(*.bib)");

ConfigManager::ConfigManager() :
    mainWindow(0),
    textCharFormats(new QMap<QString,QTextCharFormat>())
{

    QCoreApplication::setOrganizationName("TexitEasy");
    QCoreApplication::setOrganizationDomain("texiteasy.com");
    QCoreApplication::setApplicationName("TexitEasy");
    QSettings::setDefaultFormat(QSettings::IniFormat);
 }

void ConfigManager::init()
{

    qDebug()<<"Init ConfigManager";
    checkRevision();

    QSettings settings;


    if(!settings.contains("defaultDictionary"))
    {
        if(dictionnaries().contains(QLocale::system().name()))
        {
            settings.setValue("defaultDictionary", QLocale::system().name());
        }
        else
        {
            settings.setValue("defaultDictionary", "en_GB");
        }
    }

    if(!settings.contains("language"))
    {
        QString locale = QLocale::system().name().section('_', 0, 0);
        settings.setValue("language",locale);
    }

    settings.beginGroup("builder");

#if __MAC_10_6
    if(!settings.contains("latexPath"))
    {
        settings.setValue("latexPath","/usr/texbin/");
    }
#endif
    if(!settings.contains("bibtex"))
    {
        settings.setValue("bibtex","bibtex \"%1\"");
    }

    if(!settings.contains("defaultLatex"))
    {
        settings.setValue("defaultLatex","PdfLatex");
        settings.setValue("latexCommandNames", QStringList("PdfLatex"));
    #if OS_WINDOWS
        settings.setValue("latexCommands", QStringList("pdflatex.exe -synctex=1 -shell-escape -interaction=nonstopmode -enable-write18 %1"));
    #else
        settings.setValue("latexCommands", QStringList("pdflatex -synctex=1 -shell-escape -interaction=nonstopmode -enable-write18 %1"));
    #endif
    }

    settings.endGroup();

    settings.beginGroup("theme");
    if(!settings.contains("theme"))
    {
        settings.setValue("theme",QString("dark"));
    }
    if(!this->load())
    {
        QFont font("Consolas");
        font.setPointSize(settings.value("pointSize",12).toInt());
        QTextCharFormat charFormat;

        charFormat.setForeground(QColor(53,52,41));
        charFormat.setFont(font);
        charFormat.setBackground(QColor(250,250,250));
        textCharFormats->insert("normal",charFormat);
    }
    settings.endGroup();
    this->applyTranslation();
    return;

}
void ConfigManager::setMainWindow(QWidget * mainWindow)
{
    this->mainWindow = mainWindow;
}


ConfigManager::~ConfigManager()
{
    delete this->textCharFormats;
}

QString ConfigManager::textCharFormatToString(QTextCharFormat charFormat, QTextCharFormat defaultFormat)
{
    QString config;

    if(charFormat.font() != defaultFormat.font())
    {
        config += QString("font(");
        if(charFormat.font().family().compare(defaultFormat.font().family()))
        {
            config += QString("\"")+charFormat.font().family()+QString("\" ");
        }
        if(charFormat.font().pointSize() != defaultFormat.font().pointSize())
        {
            config += QString::number(charFormat.font().pointSize())+QString(" ");
        }
        if(charFormat.font().bold() != defaultFormat.font().bold())
        {
            config += (charFormat.font().bold()?"bold":"normal")+QString(" ");
        }
        config +=QString(") ");
    }

    if(charFormat.foreground().style() != Qt::NoBrush)
    {
        config += " foreground("+QString::number(charFormat.foreground().color().red())+
                ", "+QString::number(charFormat.foreground().color().green())+
                ", "+QString::number(charFormat.foreground().color().blue())+
                ") ";
    }
    if(charFormat.background().style() != Qt::NoBrush)
    {
        config += " background("+QString::number(charFormat.background().color().red())+
                    ", "+QString::number(charFormat.background().color().green())+
                    ", "+QString::number(charFormat.background().color().blue())+
                    ") ";
    }
    if(config.isEmpty())
    {
        config = "inherit";
    }
    return config;
}

QTextCharFormat ConfigManager::stringToTextCharFormat(QString string, QTextCharFormat defaultFormat)
{
    QTextCharFormat charFormat(defaultFormat);

    DEBUG_THEME_PARSER(qDebug()<<string);
    QRegExp pattern("([a-z]*)\\(([^\\)]*)\\)");
    QRegExp familyPattern("\\\"([^\\\"]*)\\\"");
    QRegExp pointSizePattern("[^0-9]([0-9]+)[^0-9]");
    int index;
    int length;
    index = string.indexOf(pattern);
    while(index != -1)
    {
        length = pattern.matchedLength();
        if(pattern.captureCount() < 2)
        {
            qDebug()<<(QString::fromUtf8("Erreur lors de la lecture du fichier Theme près de la ligne : ")+string).toLatin1();
            continue;
            //QMessageBox::warning(0, QObject::trUtf8("Erreur"), QObject::tr((QString::fromUtf8("Erreur lors de la lecture du fichier Theme près de la ligne : ")+string).toLatin1()));
        }
        if(!pattern.capturedTexts().at(1).compare("foreground"))
        {
            QStringList colors = pattern.capturedTexts().last().split(",");

            if(colors.count() < 3)
            {
                qDebug()<<(QString::fromUtf8("Erreur lors de la lecture du fichier Theme près de la ligne : ")+string).toLatin1();
                continue;
            }
            charFormat.setForeground(QBrush(QColor(colors.at(0).trimmed().toInt(),
                                                   colors.at(1).trimmed().toInt(),
                                                   colors.at(2).trimmed().toInt())));
            DEBUG_THEME_PARSER(qDebug()<<"Color : "<<colors.at(0).trimmed().toInt()<<", "<<colors.at(1).trimmed().toInt()<<", "<<colors.at(2).trimmed().toInt());
        }
        else if(!pattern.capturedTexts().at(1).compare("background"))
        {
            QStringList colors = pattern.capturedTexts().last().split(",");

            if(colors.count() < 3)
            {
                qDebug()<<(QString::fromUtf8("Erreur lors de la lecture du fichier Theme près de la ligne : ")+string).toLatin1();
                continue;
            }
            charFormat.setBackground(QBrush(QColor(colors.at(0).trimmed().toInt(),
                                                   colors.at(1).trimmed().toInt(),
                                                   colors.at(2).trimmed().toInt())));
            DEBUG_THEME_PARSER(qDebug()<<"BackgroundColor : "<<colors.at(0).trimmed().toInt()<<", "<<colors.at(1).trimmed().toInt()<<", "<<colors.at(2).trimmed().toInt());
        }
        else if(!pattern.capturedTexts().at(1).compare("font"))
        {
            QFont font(charFormat.font());
            if(pattern.capturedTexts().last().contains("bold",Qt::CaseInsensitive))
            {
                font.setBold(QFont::Bold);
                DEBUG_THEME_PARSER(qDebug()<<"Bold : bold");
            }
            else if(pattern.capturedTexts().last().contains("normal",Qt::CaseInsensitive))
            {
                font.setBold(QFont::Normal);
                DEBUG_THEME_PARSER(qDebug()<<"Bold : normal");
            }
            if(pattern.capturedTexts().last().indexOf(familyPattern) != -1)
            {
                font.setFamily(familyPattern.capturedTexts().last());
                DEBUG_THEME_PARSER(qDebug()<<"Family : "<<familyPattern.capturedTexts().last());
            }
            if(pattern.capturedTexts().last().indexOf(pointSizePattern) != -1)
            {
                font.setPointSize(pointSizePattern.capturedTexts().last().toInt());
                DEBUG_THEME_PARSER(qDebug()<<"PointSize : "<<pointSizePattern.capturedTexts().last().toInt());
            }
            charFormat.setFont(font);
        }
        index = string.indexOf(pattern, index + length);
    }
    return charFormat;
}

void ConfigManager::changePointSizeBy(int delta)
{
    this->setPointSize(this->pointSize()+(delta>0?1:-1));
    /*
    foreach(const QString &key, this->textCharFormats->keys())
    {
        QTextCharFormat format(this->textCharFormats->value(key));
        QFont font(format.font());
        font.setPointSize(font.pointSize()+delta);
        format.setFont(font);
        this->textCharFormats->insert(key,format);
    }*/
}
void ConfigManager::setReplaceDefaultFont(bool replace)
{
    QSettings settings;
    settings.beginGroup("theme");
    settings.setValue("replaceDefaultFont",replace);
}
void ConfigManager::replaceDefaultFont()
{
    QSettings settings;
    settings.beginGroup("theme");
    QString family = settings.value("fontFamily").toString();
    foreach(const QString &key, this->textCharFormats->keys())
    {
        QTextCharFormat format(this->textCharFormats->value(key));
        QFont font(format.font());
        font.setFamily(family);
        format.setFont(font);
        this->textCharFormats->insert(key,format);
    }
}

void ConfigManager::setFontFamily(QString family)
{
    QSettings settings;
    settings.beginGroup("theme");
    settings.setValue("fontFamily",family);
    this->replaceDefaultFont();
}

void ConfigManager::setPointSize(int size)
{
    QSettings settings;
    settings.beginGroup("theme");
    settings.setValue("pointSize",size);
    foreach(const QString &key, this->textCharFormats->keys())
    {
        QTextCharFormat format(this->textCharFormats->value(key));
        QFont font(format.font());
        font.setPointSize(size);
        format.setFont(font);
        this->textCharFormats->insert(key,format);
    }
}


void ConfigManager::save()
{
    QDir dir;
    QString dataLocation("");
#if QT_VERSION < 0x050000
    dataLocation = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#else
    dataLocation = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#endif
    if(dataLocation.isEmpty())
    {
            //QMessageBox::warning(this->mainWindow,QObject::tr("Attention"), QObject::tr("QStandardPaths::DataLocation est introuvable."));
            return;
    }
    if(!dir.exists(dataLocation))
    {
        dir.mkpath(dataLocation);
    }
    QSettings settings;
    settings.beginGroup("theme");
    QSettings file(themePath()+settings.value("theme").toString()+".texiteasy-theme",QSettings::IniFormat);

    QMapIterator<QString,QTextCharFormat> it(*this->textCharFormats);
    QString key;
    QTextCharFormat val;
    while(it.hasNext())
    {
        it.next();
        file.setValue(it.key(),ConfigManager::textCharFormatToString(it.value(),this->textCharFormats->value("normal")));
    }
    file.setValue("normal",this->textCharFormatToString(this->textCharFormats->value("normal"),QTextCharFormat()));

}


bool ConfigManager::load(QString theme)
{
    QDir dir;
    QString dataLocation("");
#if QT_VERSION < 0x050000
    dataLocation = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#else
    dataLocation = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#endif
    if(dataLocation.isEmpty())
    {
            //QMessageBox::warning(this->mainWindow,QObject::tr("Attention"), QObject::tr("QStandardPaths::DataLocation est introuvable."));
            return false;
    }
    if(theme.isEmpty())
    {
        QSettings settings;
        settings.beginGroup("theme");
        theme = settings.value("theme").toString();
    }
    else
    {
        QSettings settings;
        settings.beginGroup("theme");
        settings.setValue("theme",theme);
    }
    this->_theme = theme;
    QSettings file(themePath()+theme+".texiteasy-theme",QSettings::IniFormat);

    if(!file.contains("normal"))
    {
        return false;
    }

    QStringList keys = file.allKeys();

    QSettings settings;
    DEBUG_THEME_PARSER(qDebug()<<"Style normal :");
    QTextCharFormat normal = this->stringToTextCharFormat(file.value("normal").toString());
    QFont normalFont = normal.font();
    normalFont.setPointSize(settings.value("theme/pointSize").toInt());
    normal.setFont(normalFont);
    this->textCharFormats->insert("normal", normal);
    foreach(const QString& key, keys)
    {
        if(!key.compare("normal"))
        {
            continue;
        }

        DEBUG_THEME_PARSER(qDebug()<<"Style "<<key<<" :");
        QTextCharFormat val = ConfigManager::stringToTextCharFormat(file.value(key).toString(), normal);
        this->textCharFormats->insert(key, val);
    }

    {
        QSettings settings;
        settings.beginGroup("theme");
        if(settings.value("replaceDefaultFont").toBool())
        {
            this->replaceDefaultFont();
        }
    }

    return true;
}

void ConfigManager::openThemeFolder()
{
    QDesktopServices::openUrl(QUrl("file:///" + themePath()));
}

QStringList ConfigManager::themesList()
{
    QDir dir(themePath());
    QStringList list = dir.entryList(QDir::Files | QDir::Readable, QDir::Name).filter(QRegExp("\\.texiteasy-theme"));
    list.replaceInStrings(QRegExp("\.texiteasy-theme$"), "");
    return list;
}
QString ConfigManager::dictionaryPath()
{
       QString dataLocation("");
   #if QT_VERSION < 0x050000
       dataLocation = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
   #else
       dataLocation = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
   #endif
       return dataLocation+"/dictionaries/";
}
QString ConfigManager::themePath()
{
       QString dataLocation("");
   #if QT_VERSION < 0x050000
       dataLocation = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
   #else
       dataLocation = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
   #endif
       return dataLocation+"/themes/";
}


QStringList ConfigManager::dictionnaries()
{
    QDir dir(dictionaryPath());
    QStringList list = dir.entryList(QDir::Files | QDir::Readable, QDir::Name).filter(QRegExp("\\.dic"));
    list.replaceInStrings(QRegExp("\.dic$"), "");
    return list;
}

QStringList ConfigManager::languagesList()
{
    QDir dir(":/translations");
    return dir.entryList(QDir::Files | QDir::Readable, QDir::Name).replaceInStrings(QRegExp("^texiteasy_([a-zA-Z0-9]+)\\.qm$"), "\\1");
}
void ConfigManager::applyTranslation()
{
    QTranslator * translator = new QTranslator();
    if(!translator->load(":/translations/texiteasy_"+this->language()))
    {
        translator->load(":/translations/texiteasy_en");
    }
    QApplication::installTranslator(translator);
}

bool ConfigManager::isThisVersionHaveToBeReminded(QString version)
{
    QSettings settings;
    if(settings.value("lastDetectedUpdate", CURRENT_VERSION).toString().compare(version))
    {
        return true;
    }
    return false;
}
void ConfigManager::dontRemindMeThisVersion(QString version)
{
    QSettings settings;
    settings.setValue("lastDetectedUpdate",version);

}

void ConfigManager::checkRevision()
{
    QSettings settings;

    int fromVersion = settings.value("revision",0).toInt();

    QString dataLocation("");
    QString documentLocation("");
    QString programLocation("");
#if QT_VERSION < 0x050000
    dataLocation = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    documentLocation = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
    programLocation = QDesktopServices::storageLocation(QDesktopServices::ApplicationsLocation);
#else
    dataLocation = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    documentLocation = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    programLocation = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
#endif
    switch(fromVersion)
    {
        case 0:
        {
                qDebug()<<"First launch of TexitEasy";
            if(dataLocation.isEmpty())
            {
                return;
            }
            settings.setValue("lastFolder",documentLocation);

           {
                QDir dir;
                dir.mkpath(dataLocation);
            }
        }
        case 1:
        {
            qDebug()<<"texiteasy 1=>2";
            {
                QDir dir;

                QFile theme(":/themes/dark.sim-theme");
                QFile theme2(":/themes/light.sim-theme");
                theme.copy(dataLocation+dir.separator()+"dark.sim-theme");
                theme2.copy(dataLocation+dir.separator()+"light.sim-theme");
            }

            settings.setValue("bibtex","bibtex \"%1\"");
            #if OS_WINDOWS
                settings.setValue("pdflatex", "pdflatex.exe -synctex=1 -shell-escape -interaction=nonstopmode -enable-write18 \"%1\"");
            #else
                settings.setValue("pdflatex", "pdflatex -synctex=1 -shell-escape -interaction=nonstopmode -enable-write18 \"%1\"");
            #endif


            QString pdflatexCommand = "pdflatex";
    #ifdef OS_WINDOWS
            pdflatexCommand = "pdflatex.exe";
            {
                QDir dir(programLocation);
                if(!dir.exists())
                {
                    QStringList miktexDirs = dir.entryList(QDir::Dirs).filter(QRegExp("miktex",Qt::CaseInsensitive));
                    if(!miktexDirs.isEmpty())
                    {
                        if(dir.cd(miktexDirs.first()) && dir.cd("miktex") && dir.cd("bin"))
                        {
                            settings.setValue("builder/latexPath",dir.path()+dir.separator());
                        }

                    }


                }
            }
    #endif
            if(-2 == QProcess::execute(settings.value("latexPath").toString()+pdflatexCommand+" --version"))
            {
                qDebug()<<"latex not found ask for a the path";
            }
            else
            {
                qDebug()<<"latex found";
                //qDebug()<<QFileDialog::getExistingDirectory(0, QObject::trUtf8("Choisir l'emplacement contenant l'executable latex."),programLocation);
            }
        }
        case 2:
        {
            qDebug()<<"texiteasy 2=>3";
            QDir dir;
            QFile commandsSqllite(":/data/commands.sqlite");
            commandsSqllite.copy(dataLocation+dir.separator()+"commands.sqlite");
            QFile::setPermissions(dataLocation+dir.separator()+"commands.sqlite",
                                  QFile::ReadOwner |
                                  QFile::WriteOwner |
                                  QFile::ReadGroup |
                                  QFile::WriteGroup |
                                  QFile::ReadOther |
                                  QFile::WriteOther |
                                  QFile::ReadUser |
                                  QFile::WriteUser
                                  );
            settings.setValue("commandDatabaseFilename",dataLocation+dir.separator()+"commands.sqlite");
        }
    case 3:
        qDebug()<<"texiteasy 3=>4";
    case 4:
        qDebug()<<"texiteasy 4=>5";
        {
            {
                QDir().mkdir(themePath());
                QDir dir;
                QFile theme(":/themes/dark.texiteasy-theme");
                QFile theme2(":/themes/light.texiteasy-theme");
                QFile localtheme(dataLocation+dir.separator()+"dark.sim-theme");
                QFile localtheme2(dataLocation+dir.separator()+"light.sim-theme");
                localtheme.remove();
                localtheme2.remove();
                theme.copy(themePath()+"dark.texiteasy-theme");
                theme2.copy(themePath()+"light.texiteasy-theme");
            }
            QDir().mkdir(dictionaryPath());
            QDir dir(":/data/dictionaries");
            QStringList dictionaries = dir.entryList(QDir::NoDotAndDotDot | QDir::Files);
            foreach (QString dico, dictionaries)
            {
                QFile dicoFile(":/data/dictionaries/"+dico);
                dicoFile.copy(dictionaryPath()+dico);
            }
        }
     }
     settings.setValue("revision",CURRENT_CONFIG_REVISION);
}

