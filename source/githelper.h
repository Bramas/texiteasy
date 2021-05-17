#ifndef GITHELPER_H
#define GITHELPER_H

#include <QProcess>
class GitHelper : public QObject
{
    Q_OBJECT
public:
    GitHelper(QString filename);
    ~GitHelper();
    //const QList<int> & uncommitLines() { return _uncommitLines; }

signals:
    void uncommittedLines(QList<int>);

private slots:
    void parseUnified(int exitCode, QProcess::ExitStatus exitStatus);
private:
    QProcess _process;
    QList<int> _uncommitLines;
};

#endif // GITHELPER_H
