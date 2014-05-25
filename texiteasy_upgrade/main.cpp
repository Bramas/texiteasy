#include "windows.h"
#include "miniz.c"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <limits.h>

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

int WINAPI WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR szCmdLine,
                    int iCmdShow )
{


    LPWSTR *szArglist;
    int nArgs = 0;
    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);

    if(nArgs < 2)
    {
        remove("texiteasy_upgrade.log");
        return 1;
    }
    ofstream log("texiteasy_upgrade.log");
    char * zipFilename = szCmdLine;

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
            log <<"directory ["<<f_idx<<"] : "<<filenameData<<"\n";
            mkdir(filenameData);
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
        FILE * file = fopen(filenameData, "wb");
        if(!file)
        {
            error = true;
            log<<"file ["<<f_idx<<"] : "<<filenameData<<" : ERROR\n";
        }
        else
        {
            log<<"file ["<<f_idx<<"] : "<<filenameData<<" : OK\n";
            fwrite(data, sizeof(char), dataSize, file);
            fclose(file);
        }
    }
    mz_zip_reader_end(&zip_archive);

    if(error)
    {
        MessageBox(NULL, L"Texiteasy was unable to upgrade. Please Retry.", NULL, NULL);
        return 1;
    }
    log.close();
    /*
     * remove may not instantly work so we clean it before removing it
     */

    remove("texiteasy_upgrade.log");
    /*
     * remove zip file and launch texiteasy
     */
    remove(zipFilename);
    system("start TexitEasy.exe");
    free(zipFilename);
    return 0;
}
