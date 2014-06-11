#include "windows.h"
#include "miniz.c"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <limits.h>
#include <string.h>

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QTextStream>

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint;

#define my_max(a,b) (((a) > (b)) ? (a) : (b))
#define my_min(a,b) (((a) < (b)) ? (a) : (b))

#define ZIP_FILENAME_MAX_SIZE 5000

static int tinfl_put_buf_func(const void* pBuf, int len, void *pUser)
{
  return len == (int)fwrite(pBuf, 1, len, (FILE*)pUser);
}


using namespace std;

int main(int argc, char ** argv)
{

    char dest[5000] = "";
    QString logFilename;

    strcat(dest, QFileInfo(argv[0]).absolutePath().toLatin1().data());
    logFilename = dest;
    logFilename += "/texiteasy_upgrade.log";

    QString oldFolder = dest;
    oldFolder += "/old/";
    if(argc > 2)
    {
        oldFolder = argv[2];
    }

    if(argc < 2)
    {
        QFile(logFilename).remove();
        return 1;
    }
    QFile logFile(logFilename);
    if(!logFile.open(QFile::WriteOnly | QFile::Text))
    {
        MessageBox(NULL, L"Error: Unable to open log file", NULL, NULL);
        return 1;
    }
    QTextStream log(&logFile);
    char * zipFilename = argv[1];

    if(strstr(zipFilename, ".exe"))
    {
        char command[5000] = "start \"";
        strcat(command, zipFilename);
        system(strcat(command, "\""));
        return 0;
    }


    FILE * zipFile;
    if(!(zipFile = fopen(zipFilename, "rb")))
    {
        log<<"Error: file "<<zipFilename<<" not found";
        return 1;
    }
    fclose(zipFile);

    mz_zip_archive zip_archive;
    MZ_CLEAR_OBJ(zip_archive);
    if (!mz_zip_reader_init_file(&zip_archive, zipFilename, MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY))
    {
        log<<"Error: file "<<zipFilename<<" cannot be inited";
        return MZ_FALSE;
    }

    int filesCount = mz_zip_reader_get_num_files(&zip_archive);
    log <<"Start Upgrade\n";
    log << filesCount<<" files found\n";


    /*
     * move old files
     */
    bool error = false;
    for(int f_idx = 0; f_idx < filesCount; ++f_idx)
    {
        char filenameData[255];
        mz_zip_reader_get_filename(&zip_archive, f_idx, filenameData, 255);
        if(mz_zip_reader_is_file_a_directory(&zip_archive,f_idx))
        {
            continue;
        }
        QString before = dest;
        before += "\\";
        before += filenameData;

        QString after = oldFolder;
        after += filenameData;

        QDir().mkpath(QFileInfo(before).absolutePath());
        QDir().mkpath(QFileInfo(after).absolutePath());

        QFile f(before);
        if(f.exists() && !f.rename(after))
        {
            error = true;
            log<<"file ["<<f_idx<<"] : "<<before.toLatin1().data()<<" : ERROR unable to move\n";
        }
        else
        {
            log<<"file ["<<f_idx<<"] : "<<before.toLatin1().data()<<" : OK\n";
        }
    }
    if(error)
    {

        MessageBox(NULL, L"Texiteasy was unable to upgrade. Please Retry. (some files cannot be moved)", NULL, NULL);
        char texEx[5000] = "start ";
        strcat(texEx, dest);
        strcat(texEx, "\\TexitEasy.exe");
        system(texEx);
        return 0;
    }
    /*
     * copy new files
     */
    error = false;
    for(int f_idx = 0; f_idx < filesCount; ++f_idx)
    {
        char filenameData[255];
        mz_zip_reader_get_filename(&zip_archive, f_idx, filenameData, 255);
        if(mz_zip_reader_is_file_a_directory(&zip_archive,f_idx))
        {
            continue;
        }
        size_t dataSize = 0;
        void * data = mz_zip_reader_extract_to_heap(&zip_archive, f_idx, &dataSize, MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY);

        char fileDest[5000] = "";
        strcat(fileDest, dest);
        strcat(fileDest, "\\");
        strcat(fileDest, filenameData);
        FILE * file = fopen(fileDest, "wb");
        if(!file)
        {
            error = true;
            log<<"file ["<<f_idx<<"] : "<<fileDest<<" : ERROR unable to copy\n";
        }
        else
        {
            log<<"file ["<<f_idx<<"] : "<<fileDest<<" : OK\n";
            fwrite(data, sizeof(char), dataSize, file);
            fclose(file);
        }
    }

    /* Close
     */
    mz_zip_reader_end(&zip_archive);

    if(error)
    {

        MessageBox(NULL, L"Texiteasy was unable to upgrade. Please Retry.", NULL, NULL);
        char texEx[5000] = "start ";
        strcat(texEx, dest);
        strcat(texEx, "\\TexitEasy.exe");
        system(texEx);
        return 0;
    }
    logFile.close();
    /*
     * remove may not instantly work so we clean it before removing it
     */

    logFile.remove();
    /*
     * remove zip file and launch texiteasy
     */
    QFile(zipFilename).remove();
    QString texEx = dest;
    texEx += "/TexitEasy.exe -n";
    QProcess * p = new QProcess();
    p->start(texEx);
    free(zipFilename);
    return 0;
}
