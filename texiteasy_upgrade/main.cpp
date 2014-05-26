#include "windows.h"
#include "miniz.c"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <limits.h>
#include <string.h>

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
    char logFilename[5000] = "";
    if(argc > 2)
    {
        strcat(dest, argv[2]);
    }
    strcat(logFilename, dest);
    strcat(logFilename, "\\texiteasy_upgrade.log");

    if(argc < 2)
    {
        remove(logFilename);
        return 1;
    }
    ofstream log(logFilename);
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

    for(int f_idx = 0; f_idx < filesCount; ++f_idx)
    {
        char filenameData[255];
        mz_zip_reader_get_filename(&zip_archive, f_idx, filenameData, 255);
        if(mz_zip_reader_is_file_a_directory(&zip_archive,f_idx))
        {
            char dirDest[5000] = "";
            strcat(dirDest, dest);
            strcat(dirDest, "\\");
            strcat(dirDest, filenameData);

            log <<"directory ["<<f_idx<<"] : "<<dirDest<<"\n";
            mkdir(dirDest);
            continue;
        }
    }
    bool error = false;
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
            log<<"file ["<<f_idx<<"] : "<<fileDest<<" : ERROR\n";
        }
        else
        {
            log<<"file ["<<f_idx<<"] : "<<fileDest<<" : OK\n";
            fwrite(data, sizeof(char), dataSize, file);
            fclose(file);
        }
    }
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
    log.close();
    /*
     * remove may not instantly work so we clean it before removing it
     */

    remove(logFilename);
    /*
     * remove zip file and launch texiteasy
     */
    remove(zipFilename);
    char texEx[5000] = "start ";
    strcat(texEx, dest);
    strcat(texEx, "\\TexitEasy.exe");
    system(texEx);
    system(texEx);
    free(zipFilename);
    return 0;
}
