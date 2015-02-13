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

#include "configmanager.h"
#include "dialogtexdownloadassistant.h"
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
#include <QTextStream>
#include <QLocale>
#include <QDateTime>
#include <QUuid>
#include <QCryptographicHash>


#ifdef OS_LINUX
#   define POPPLER_VERSION "unknown"
#else
//#   include <poppler-qt5/poppler-config.h>
#define POPPLER_VERSION "unknown"
#endif

#define DEBUG_THEME_PARSER(a)

QString uniqueId()
{
    QString softId = APPLICATION_NAME;
    softId += "_";
    softId += CURRENT_VERSION;
    softId += "_";
#ifdef OS_WINDOWS
    softId += "WIN";
#else
#ifdef OS_MAC
    softId += "MAC";
#else
    softId += "LIN";
#endif
#endif
    softId += "_";
    softId += QDateTime::currentDateTimeUtc().toString("yyyyMMdd_hhmm_");
    softId += QUuid::createUuid().toString().left(19).right(18);
    softId += "_";
    softId += QString(QCryptographicHash::hash(softId.toLatin1(), QCryptographicHash::Sha1).toHex()).left(8);
    return softId;
}

ConfigManager ConfigManager::Instance;
QString ConfigManager::NoDictionnary = QObject::trUtf8("No Dictionnary");

QString ConfigManager::Extensions = QObject::trUtf8("Latex (*.tex *.latex);;BibTex(*.bib)");

QString ConfigManager::MacroSuffix = ".texiteasy-macro";
const QStringList ConfigManager::DefaultLatexCommands =
        QString("pdflatex -synctex=1 -shell-escape -interaction=nonstopmode -enable-write18 %1\n"
        "xelatex -synctex=1 -interaction=nonstopmode %1\n"
        "latexmk -e \"$pdflatex=q/pdflatex -synctex=1 -interaction=nonstopmode/\" -pdf %1\n"
                "latex -interaction=nonstopmode %1 ; dvipdfm %1.dvi\n"
                "latex -interaction=nonstopmode %1 ; dvips %1.dvi ; ps2pdf %1.ps\n"
                "makeindex %1.idx").split('\n');
const QStringList ConfigManager::DefaultLatexCommandNames =
        QString("PdfLatex\nXeLatex\nLatexmk\nLatex + dvipdfm\nLatex + dvips + ps2pdf\nMakeIndex").split('\n');

const QStringList ConfigManager::CodecsAvailable =
        QString("UTF-8\n\nApple Roman\nBig5\nBig5-HKSCS\nCP949\nEUC-JP\nEUC-KR\nGB18030-0\nIBM 850\nIBM 866\nIBM 874\nISO 2022-JP\nISO 8859-*/ISO 8859-1\n-/ISO 8859-2\n-/ISO 8859-3\n-/ISO 8859-4\n-/ISO 8859-5\n-/ISO 8859-6\n-/ISO 8859-7\n-/ISO 8859-8\n-/ISO 8859-9\n-/ISO 8859-10\n-/ISO 8859-13\n-/ISO 8859-14\n-/ISO 8859-15\n-/ISO 8859-16\nIscii-*/Iscii-Bng\n-/Iscii-Dev\n-/Iscii-Gjr\n-/Iscii-Knd\n-/Iscii-Mlm\n-/Iscii-Ori\n-/Iscii-Pnj\n-/Iscii-Tlg\n-/Iscii-Tml\nJIS X 0201\nJIS X 0208\nKOI8-R\nKOI8-U\nShift-JIS\nTIS-620\nTSCII\nUTF-8\nUTF-16\nUTF-16BE\nUTF-16LE\nUTF-32\nUTF-32BE\nUTF-32LE\nWindows-*/Windows-1250\n-/Windows-1251\n-/Windows-1252\n-/Windows-1253\n-/Windows-1254\n-/Windows-1255\n-/Windows-1256\n-/Windows-1257\n-/Windows-1258\n").split('\n');

ConfigManager::ConfigManager() :
    mainWindow(0),
    textCharFormats(new QMap<QString,QTextCharFormat>())
{

    QCoreApplication::setOrganizationName("TexitEasy");
    QCoreApplication::setOrganizationDomain("texiteasy.com");
    QCoreApplication::setApplicationName("TexitEasy");
    QSettings::setDefaultFormat(QSettings::IniFormat);
 }
void ConfigManager::init(QString in_applicationPath)
{

    //qDebug()<<"Init ConfigManager";
    _applicationPath = in_applicationPath;

#ifdef PORTABLE_EXECUTABLE
    _settingsPath = in_applicationPath+"/Settings";
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, _settingsPath);
#endif

    checkRevision();

    QSettings settings;
    if(!settings.contains("softId"))
    {
        settings.setValue("softId", uniqueId());
    }

#ifdef PORTABLE_EXECUTABLE
    if(!settings.contains("pdfSynchronized"))
    {
        settings.setValue("pdfSynchronized", false);
    }
#endif
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
        settings.setValue("latexCommandNames", DefaultLatexCommandNames);
        settings.setValue("latexCommands", DefaultLatexCommands);
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



    //Log::debug(QString("dictionnaryPath : %1").arg(this->dictionaryPath());
    return;

}
void ConfigManager::setMainWindow(QWidget * mainWindow)
{
    this->mainWindow = mainWindow;
}

QString ConfigManager::popplerVersion()
{
        return POPPLER_VERSION;
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
    QString dataPath = dataLocation();
    if(!dir.exists(dataPath))
    {
        dir.mkpath(dataPath);
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
    QString dataPath = dataLocation();
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
void ConfigManager::addToDictionnary(QString dico, QString word)
{
    // we do not care about multiple entries.
    QFile file(dictionaryPath()+dico+".user-word");
    file.open(QFile::WriteOnly | QFile::Append);
    QTextStream s(&file);
    s.setCodec("utf8");
    s << QString(" ");
    s << word;
}
QStringList ConfigManager::userDictionnary(QString dico)
{
    QFile file(dictionaryPath()+dico+".user-word");
    file.open(QFile::ReadOnly);
    QTextStream s(&file);
    s.setCodec("utf8");
    QString word;
    QStringList list;
    while(!s.atEnd())
    {
        s >> word;
        list.append(word);
    }
    return list;
}

QStringList ConfigManager::themesList()
{
    QDir dir(themePath());
    QStringList list = dir.entryList(QDir::Files | QDir::Readable, QDir::Name).filter(QRegExp("\\.texiteasy-theme$"));
    list.replaceInStrings(QRegExp("\\.texiteasy-theme$"), "");
    return list;
}
QString ConfigManager::dictionaryPath()
{
#ifdef OS_MAC
    #ifdef LIB_DEPLOY
        return QApplication::applicationDirPath()+"/../Resources/dictionaries/";
    #else
        return QApplication::applicationDirPath()+"/../../../../Resources/dictionaries/";
    #endif
#else
#ifdef OS_LINUX
    return "/usr/share/texiteasy/dictionaries/";
#else
    return applicationPath()+"/data/dictionaries/";
#endif
#endif

}

QString ConfigManager::dataLocation()
{

#ifdef PORTABLE_EXECUTABLE
    return _settingsPath;
#else
    QString location("");
    #if QT_VERSION < 0x050000
        location = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    #else
        location = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    #endif
    return location;
#endif
}

QString ConfigManager::updateFiles()
{
#ifdef OS_WINDOWS
    QFile fLog("texiteasy_upgrade.log");
    if(fLog.exists())
    {
        return "";
    }

    {
        QString filename(dataLocation()+"/updateFiles.zip");
        QFile f(filename);
        if(f.exists())
        {
            return filename;
        }
    }

    {
        QString filename(dataLocation()+"/updateFiles.exe");
        QFile f(filename);
        if(f.exists())
        {
            return filename;
        }
    }
#endif
    return "";
}

QString ConfigManager::macrosPath()
{
       return dataLocation()+"/macros/";
}
QString ConfigManager::themePath()
{
       return dataLocation()+"/themes/";
}


QStringList ConfigManager::dictionnaries()
{
    QDir dir(dictionaryPath());
    QStringList list = dir.entryList(QDir::Files | QDir::Readable, QDir::Name).filter(QRegExp("\\.dic"));
    list.replaceInStrings(QRegExp("\\.dic$"), "");
    list << NoDictionnary;
    return list;
}


QStringList ConfigManager::languagesList()
{
    QDir dir(":/translations");
    return dir.entryList(QDir::Files | QDir::Readable, QDir::Name).replaceInStrings(QRegExp("^texiteasy_([a-zA-Z0-9]+)\\.qm$"), "\\1");
}

QString ConfigManager::latexCommand(QString name)
{
    if(name.isEmpty())
    {
        name = defaultLatex();
    }
    int i = latexCommandNames().indexOf(name);
    QStringList list = latexCommands();
    if(i >= 0 && i < list.count())
    {
        return list.at(i);
    }
    return QString::null;
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

void ConfigManager::recursiveCopy(QString from, QString to, QFile::Permissions permission)
{
    QDir().mkdir(to);
    QDir fromDir(from);
    QStringList files = fromDir.entryList(QDir::NoDotAndDotDot | QDir::Files);
    foreach (QString fileName, files)
    {
        if(QFile::exists(to+"/"+fileName))
        {
            QFile(to+"/"+fileName).remove();
        }
        QFile file(from+"/"+fileName);
        if(QFile(to+"/"+fileName).exists())
        {
            QFile(to+"/"+fileName).remove();
        }
        file.copy(to+"/"+fileName);
        QFile::setPermissions(to+"/"+fileName, permission);
    }
    QStringList dirs = fromDir.entryList(QDir::NoDotAndDotDot | QDir::Dirs);
    foreach (QString dir, dirs)
    {
        recursiveCopy(from+"/"+dir, to+"/"+dir, permission);
    }
}
QString ConfigManager::systemInfo()
{
    QString sysType = "Linux";
    QString sysVersion = "Unknown";
#ifdef OS_MAC
    sysType = "Mac Os";
    switch(QSysInfo::macVersion())
    {
    case QSysInfo::MV_9: sysVersion = "9"; break;
    case QSysInfo::MV_10_0: sysVersion = "10.0"; break;
    case QSysInfo::MV_10_1: sysVersion = "10.1"; break;
    case QSysInfo::MV_10_2: sysVersion = "10.2"; break;
    case QSysInfo::MV_10_3: sysVersion = "10.3"; break;
    case QSysInfo::MV_10_4: sysVersion = "10.4"; break;
    case QSysInfo::MV_10_5: sysVersion = "10.5"; break;
    case QSysInfo::MV_10_6: sysVersion = "10.6"; break;
    case QSysInfo::MV_10_7: sysVersion = "10.7"; break;
    case QSysInfo::MV_10_8: sysVersion = "10.8"; break;
    case QSysInfo::MV_10_9: sysVersion = "10.9"; break;
    default: break;
    }
#endif
#ifdef OS_WINDOWS
    sysType = "Windows";
    switch(QSysInfo::windowsVersion())
    {
    case QSysInfo::WV_NT: sysVersion = "NT"; break;
    case QSysInfo::WV_2000: sysVersion = "2000"; break;
    case QSysInfo::WV_XP: sysVersion = "XP"; break;
    case QSysInfo::WV_2003: sysVersion = "2003"; break;
    case QSysInfo::WV_VISTA: sysVersion = "Vista"; break;
    case QSysInfo::WV_WINDOWS7: sysVersion = "7"; break;
    case QSysInfo::WV_WINDOWS8: sysVersion = "8"; break;
    }
#endif
    return sysType+" "+sysVersion;
}

void ConfigManager::resetThemes()
{
    QDir().mkpath(themePath());
    //remove some old files and update theme
    {
        QFile localtheme(themePath()+"dark.texiteasy-theme");
        QFile localtheme2(themePath()+"light.texiteasy-theme");
        QFile::setPermissions(themePath()+"dark.texiteasy-theme",
                          QFile::ReadOwner |
                          QFile::WriteOwner |
                          QFile::ReadGroup |
                          QFile::WriteGroup |
                          QFile::ReadOther |
                          QFile::WriteOther |
                          QFile::ReadUser |
                          QFile::WriteUser
                          );
        QFile::setPermissions(themePath()+"light.texiteasy-theme",
                          QFile::ReadOwner |
                          QFile::WriteOwner |
                          QFile::ReadGroup |
                          QFile::WriteGroup |
                          QFile::ReadOther |
                          QFile::WriteOther |
                          QFile::ReadUser |
                          QFile::WriteUser
                          );
        localtheme.remove();
        localtheme2.remove();
    }
    QFile theme(":/themes/dark.texiteasy-theme");
    QFile theme2(":/themes/light.texiteasy-theme");
    theme.copy(themePath()+"dark.texiteasy-theme");
    theme2.copy(themePath()+"light.texiteasy-theme");
    QFile::setPermissions(themePath()+"dark.texiteasy-theme",
                  QFile::ReadOwner |
                  QFile::WriteOwner |
                  QFile::ReadGroup |
                  QFile::WriteGroup |
                  QFile::ReadOther |
                  QFile::WriteOther |
                  QFile::ReadUser |
                  QFile::WriteUser
                  );
    QFile::setPermissions(themePath()+"light.texiteasy-theme",
                  QFile::ReadOwner |
                  QFile::WriteOwner |
                  QFile::ReadGroup |
                  QFile::WriteGroup |
                  QFile::ReadOther |
                  QFile::WriteOther |
                  QFile::ReadUser |
                  QFile::WriteUser
                  );
}

void ConfigManager::checkLatexExecutable()
{
    QSettings settings;
    QString documentLocation("");
    QString programLocation("");
#if QT_VERSION < 0x050000
    documentLocation = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
    programLocation = QDesktopServices::storageLocation(QDesktopServices::ApplicationsLocation);
#else
    documentLocation = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    programLocation = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
#endif

    QString pdflatexCommand = "pdflatex";
#ifdef OS_WINDOWS
#ifdef PORTABLE_EXECUTABLE
    QDir dir(_applicationPath);
    dir.cdUp();
    dir.mkdir("MikTex");
    settings.setValue("builder/latexPath", "../MikTex/miktex/bin");
#else
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
#endif
    if(-2 == QProcess::execute(settings.value("latexPath").toString()+pdflatexCommand+" --version"))
    {
        qDebug()<<"latex not found ask for a the path";

#ifdef OS_WINDOWS
#ifndef PORTABLE_EXECUTABLE
        DialogTexDownloadAssistant latexDialogAssistant;
        latexDialogAssistant.exec();
#endif
#endif
        //qDebug()<<QFileDialog::getExistingDirectory(0, QObject::trUtf8("Choisir l'emplacement contenant l'executable latex."),programLocation);
    }
    else
    {
        settings.setValue("latexFound", true);
        qDebug()<<"latex found";
    }
}

void ConfigManager::checkRevision()
{
    QSettings settings;

    int fromVersion = settings.value("version_hex",0x000000).toInt();

    QString documentLocation("");
    QString programLocation("");
#if QT_VERSION < 0x050000
    documentLocation = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
    programLocation = QDesktopServices::storageLocation(QDesktopServices::ApplicationsLocation);
#else
    documentLocation = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    programLocation = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
#endif
    if(!settings.value("latexFound").toBool())
    {
        checkLatexExecutable();
    }
    switch(fromVersion)
    {
        case 0x000000:
        {
            qDebug()<<"First launch of TexitEasy";
            if(settings.contains("lastFolder"))
            {
                settings.setValue("lastFolder",documentLocation);
            }
            QDir().mkpath(dataLocation());

            {
                QDir dir;
                QFile commandsSqllite(":/data/commands.sqlite");
                commandsSqllite.copy(dataLocation()+dir.separator()+"commands.sqlite");
                QFile::setPermissions(dataLocation()+dir.separator()+"commands.sqlite",
                                  QFile::ReadOwner |
                                  QFile::WriteOwner |
                                  QFile::ReadGroup |
                                  QFile::WriteGroup |
                                  QFile::ReadOther |
                                  QFile::WriteOther |
                                  QFile::ReadUser |
                                  QFile::WriteUser
                                  );
                settings.setValue("commandDatabaseFilename",dataLocation()+dir.separator()+"commands.sqlite");
            }

            qDebug()<<"texiteasy =>0.6.0";
    }

    case 0x000600:
    case 0x000601:
    case 0x000602:
    case 0x000603:
    case 0x000700:
    case 0x000800:
    case 0x000801:
    case 0x000802:
    case 0x000803:
    case 0x000804:

        qDebug()<<"texiteasy 0.8.x => 0.9.0";
        // append Latex + dvips + ps2pdf to default typesetting
        if(settings.contains("builder/latexCommandNames"))
        {
            QStringList latexCommandNames = settings.value("builder/latexCommandNames").toStringList();
            latexCommandNames << "Latex + dvips + ps2pdf";
            settings.setValue("builder/latexCommandNames", latexCommandNames);

            QStringList latexCommands = settings.value("builder/latexCommands").toStringList();
            latexCommands << "latex -interaction=nonstopmode %1 ; dvips %1.dvi ; ps2pdf %1.ps";
            settings.setValue("builder/latexCommands", latexCommands);
        }

    case 0x000900:
    case 0x000901:
    case 0x000902:


    case 0x001000:
    case 0x001001:

    case 0x001100:
    case 0x001101:
    case 0x001102:
    case 0x001103:
    {
        QSettings hotKeys;
        hotKeys.beginGroup("shortcuts");
        hotKeys.setValue("Cancel", "");
        hotKeys.setValue("Copy", "");
        hotKeys.setValue("Paste", "");
        hotKeys.setValue("Cut", "");
        hotKeys.setValue("Redo", "");
    }
    case 0x001200:
    case 0x001201:
    case 0x001202:
    case 0x001203:
    case 0x001300:
    case 0x001301:
    {
        qDebug()<<"texiteasy => 0.13.2";

        // install macros
        {
            recursiveCopy(":/data/macros", macrosPath(),
                                                          QFile::ReadOwner |
                                                          QFile::WriteOwner |
                                                          QFile::ReadGroup |
                                                          QFile::WriteGroup |
                                                          QFile::ReadOther |
                                                          QFile::WriteOther |
                                                          QFile::ReadUser |
                                                          QFile::WriteUser
                                                          );
        }
    }
    case 0x001302:
    case 0x001303:
    case 0x001304:
    case 0x001400:
    {
        QString roamingFolder =  QFileInfo(settings.fileName()).absolutePath();
        QStringList roamingfiles;
        roamingfiles << "TexitEasy.exe" << "LocateTexitEasy.exe" << "elevate.exe";
        roamingfiles << "texiteasy_upgrade.exe" << "TexitEasy.exe"
                     << "icudt51.dll"
                     << "icuin51.dll"
                     << "icuuc51.dll"
                     << "libbz2-1.dll"
                     << "libcurl-4.dll"
                     << "libeay32.dll"
                     << "libfreetype-6.dll"
                     << "libgcc_s_dw2-1.dll"
                     << "libiconv-2.dll"
                     << "libintl-8.dll"
                     << "libjbig-2.dll"
                     << "libjpeg-8.dll"
                     << "liblcms2-2.dll"
                     << "liblzma-5.dll"
                     << "libpcre-1.dll"
                     << "libpcre16-0.dll"
                     << "libpng16-16.dll"
                     << "libpoppler-43.dll"
                     << "libpoppler-qt5-1.dll"
                     << "libssh2-1.dll"
                     << "libstdc++-6.dll"
                     << "libtiff-5.dll"
                     << "libwinpthread-1.dll"
                     << "Qt5Core.dll"
                     << "Qt5Gui.dll"
                     << "Qt5Network.dll"
                     << "Qt5Quick.dll"
                     << "Qt5Script.dll"
                     << "Qt5Sql.dll"
                     << "Qt5Widgets.dll"
                     << "Qt5Xml.dll"
                     << "ssleay32.dll"
                     << "zlib1.dll";

        foreach(QString rf, roamingfiles)
        {
            if(QFile(roamingFolder+"/"+rf).exists())
            {
                QFile(roamingFolder+"/"+rf).remove();
            }
        }
    }
    case 0x001500:
    case 0x001600:
    case 0x001601:
    case 0x001602:
    case 0x001603:
        resetThemes();
    case 0x001700:

        break;
    }

    if(fromVersion != CURRENT_VERSION_HEX)
    {
        qDebug()<<"Thank you for the update, I feel better now.";
        QStringList versions_history = settings.value("versions_history",QStringList()).toStringList();
        versions_history.append(CURRENT_VERSION);
        settings.setValue("versions_history",versions_history);
        settings.setValue("version_hex",CURRENT_VERSION_HEX);
    }
}

