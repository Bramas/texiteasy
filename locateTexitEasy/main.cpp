#include <QFile>
#include <QFileInfo>
#include <QDir>

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        return 1;
    }
    QFile f(argv[1]);
    if(!f.open(QFile::WriteOnly | QFile::Text))
    {
        return 2;
    }

    f.write(QDir::toNativeSeparators(QFileInfo(argv[0]).absolutePath() + "/TexitEasy.exe").toLatin1());

    return 0;
}
