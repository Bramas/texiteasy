
#include <stdio.h>
#include <QProcess>
#include <QFileInfo>

int main(int argc, char ** argv)
{
    QString appDir = QFileInfo(argv[0]).absolutePath();
    appDir += "/texiteasyPath.txt";
    FILE * file = fopen(appDir.toLatin1().data(), "r");
    //FILE * log = fopen("C:\\Users\\Wybot\\AppData\\Roaming\\TexitEasy\\log.txt", "w");
    //fwrite(appDir.toLatin1().data(), sizeof(char), appDir.toLatin1().size(), log);
    if(!file)
    {
        //fwrite("Unable to open", sizeof(char), 14, log);
        return 1;
    }
    char start[5000];
    size_t s = fread(start, sizeof(char), 5000, file);

    //fwrite(start, sizeof(char), s, log);
    //fputc('\n', log);
    /*
    start[s] = ' ';
    ++s;
    int c = 0;
    while(argv[0][c] !='\0')
    {
        start[s] = argv[0][c];
        ++c;
        ++s;
    }*/
    start[s] = '\0';

    //fwrite(start, sizeof(char), s, log);
    QProcess* proc = new QProcess();
    QString str(start);
    QStringList args;
    for(int i = 1; i < argc; ++i)
    {
        args << argv[i];
    }
    proc->start(str, args);
    //system(start);

/*
int main(int argc, char ** argv)
{
    char* from =  "";
    char* to =  "";
    if(argc > 2)
    {
        from = argv[1];
        to = argv[2];
    }
    else
    {
        return 1;
    }
    QFile(from).remove();
    QString c("mklink ");
    c += "\"";
    c += from;
    c += "\" ";
    c += "\"";
    c += to;
    c += "\"";
    system(c.toLatin1());

    return 0;*/
}

