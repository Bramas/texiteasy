#include <QCoreApplication>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QFile>


void convertLineEnding(QString filename)
{
    QFile in(filename);
    if(!in.open(QFile::ReadWrite))
    {
        qDebug()<<"unable to open "<<filename;
        return;
    }
    qDebug()<<"convert "<<filename;
    QString content = QString::fromUtf8(in.readAll());
    content.replace("\r", "");//.replace("\n", "\n\r");
    in.close();
    in.remove();

    if(!in.open(QFile::ReadWrite))
    {
        qDebug()<<"unable to re-open "<<filename;
        return;
    }
    in.write(content.toUtf8());
}


void convertLineEnding(QDir dir)
{
    foreach(const QString &entry, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        convertLineEnding(QDir(dir.absoluteFilePath(entry)));
    }
    foreach(const QString &entry, dir.entryList(QDir::Files))
    {
        convertLineEnding(dir.absoluteFilePath(entry));
    }
}






int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);



    convertLineEnding(QDir("E:\\Projects\\texiteasy\\sourceRepository\\source\\data\\macros"));


    return a.exec();
}
