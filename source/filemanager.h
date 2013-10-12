#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QList>
#include "widgetfile.h"

class FileManager : public QObject
{
    Q_OBJECT
public:
    static FileManager Instance;
    WidgetFile * currentWidgetFile() { if(_currentWidgetFileId < 0) return 0; return _widgetFiles.at(_currentWidgetFileId); }

    void initTheme();

    bool open(QString filename);
    void close(WidgetFile *widget);
    int count() { return _widgetFiles.count(); }

signals:
    void cursorPositionChanged(int,int);

public slots:
    bool newFile();

    void save() { this->currentWidgetFile()->save(); }
    void saveAs() { this->currentWidgetFile()->saveAs(); }
    void pdflatex(void){ this->currentWidgetFile()->pdflatex(); }
    void bibtex(void) { this->currentWidgetFile()->bibtex(); }
    void openFindReplaceWidget() { this->currentWidgetFile()->openFindReplaceWidget(); }
    void undo();
    void redo();
    void copy();
    void cut();
    void paste();
    void wrapEnvironment();
    void setPdfSynchronized(bool pdfSynchronized);
    void jumpToPdfFromSource();
    void rehighlight();

    void setCurrent(WidgetFile * widget)
    {
        if(!widget)
        {
            setCurrent(-1);
            return;
        }
        setCurrent(_widgetFiles.indexOf(widget));
    }
    void setCurrent(int index) { _currentWidgetFileId = index; }

private slots:
    void sendCursorPositionChanged(int x, int y) { emit cursorPositionChanged(x, y); }

private:
    explicit FileManager(QObject *parent = 0);
    QList<WidgetFile *> _widgetFiles;
    int _currentWidgetFileId;
    bool _pdfSynchronized;
};

#endif // FILEMANAGER_H
