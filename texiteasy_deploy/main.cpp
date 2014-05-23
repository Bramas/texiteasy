#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QDebug>

#include "miniz.c"
#include <stdio.h>
#include <limits.h>
#include <QDir>

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint;

#define my_max(a,b) (((a) > (b)) ? (a) : (b))
#define my_min(a,b) (((a) < (b)) ? (a) : (b))

static int tinfl_put_buf_func(const void* pBuf, int len, void *pUser)
{
  return len == (int)fwrite(pBuf, 1, len, (FILE*)pUser);
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile log("log.txt");
    log.open(QFile::WriteOnly | QFile::Text);
    if(argc < 3)
    {
        qWarning()<<argc<<" argument(s) given";
        log.write(QString(QString::number(argc)+QString(" arguments given")).toLocal8Bit());
        return 1;
    }


    mz_zip_archive zip_archive;
    MZ_CLEAR_OBJ(zip_archive);
    if (!mz_zip_reader_init_file(&zip_archive, QCoreApplication::arguments().at(1).toLocal8Bit().data(), MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY))
    {
        qWarning()<<"file "<<QCoreApplication::arguments().at(1)<<" not found";
        log.write(QString("file "+QCoreApplication::arguments().at(1)+" not found").toLocal8Bit());
        return MZ_FALSE;
    }

    int filesCount = mz_zip_reader_get_num_files(&zip_archive);
    qDebug()<<filesCount<<" files found";

    for(int f_idx = 0; f_idx < filesCount; ++f_idx)
    {
        char filenameData[255];
        mz_zip_reader_get_filename(&zip_archive, f_idx, filenameData, 255);
        QString filename(filenameData);
        if(mz_zip_reader_is_file_a_directory(&zip_archive,f_idx))
        {
            qDebug()<<"directory ["<<f_idx<<"] : "<<filename;
            QDir().mkpath(filename);
            continue;
        }
    }
    for(int f_idx = 0; f_idx < filesCount; ++f_idx)
    {
        char filenameData[255];
        mz_zip_reader_get_filename(&zip_archive, f_idx, filenameData, 255);
        QString filename(filenameData);
        if(mz_zip_reader_is_file_a_directory(&zip_archive,f_idx))
        {
            continue;
        }
        qDebug()<<"file ["<<f_idx<<"] : "<<filename<<" data: "<<filenameData;
        size_t dataSize = 0;
        void * data = mz_zip_reader_extract_to_heap(&zip_archive, f_idx, &dataSize, MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY);
        QFile log(filename);
        log.open(QFile::WriteOnly);
        log.write((char*)data, dataSize);
        log.close();
    }

    /*
    QFile file(QCoreApplication::arguments().at(1));
    if(!file.exists())
    {
        qWarning()<<"file "<<QCoreApplication::arguments().at(1)<<" not found";
        log.write(QString("file "+QCoreApplication::arguments().at(1)+" not found").toLocal8Bit());
        return 1;
    }
    log.write(QString("copy "+QCoreApplication::arguments().at(1)+" to "+QCoreApplication::arguments().at(2)).toLocal8Bit());
    file.copy(QCoreApplication::arguments().at(2));
*/
    return 0;
    return a.exec();
}
