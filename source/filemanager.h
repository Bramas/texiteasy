#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QList>
#include <QFileSystemWatcher>
#include "file.h"
#include "widgetfile.h"

class QMimeData;
class MainWindow;
typedef QWidget WidgetPdfViewerWrapper;

class FileManager : public QObject
{
    Q_OBJECT
public:
    static FileManager Instance;
    void init();
    void initWidgetPdfViewerWrapper();
    WidgetFile * currentWidgetFile() { if(_currentWidgetFileId < 0) return 0; return _widgetFiles.at(_currentWidgetFileId); }

    void initTheme();

    bool open(QString filename);
    void close(WidgetFile *widget);
    int count() { return _widgetFiles.count(); }
    AssociatedFile reverseAssociation(QString filename, int* index = 0);

    File * file(int index);
    WidgetFile * widgetFile(QString filename);
    WidgetFile * widgetFile(int index) { return _widgetFiles.at(index); }
    void setMainWindow(MainWindow * m) { _mainWindow = m; }
    void removeWatch(QString filename) { _fileSystemWatcher->removePath(filename); }
    void addWatch(QString filename) { _fileSystemWatcher->addPath(filename); }
    void handleMimeData(const QMimeData * mimeData);
    WidgetPdfViewerWrapper * widgetPdfViewerWrapper() { return _widgetPdfViewerWrapper; }

signals:
    void cursorPositionChanged(int,int);
    void requestOpenFile(QString);
    void filenameChanged(QString);
    void filenameChanged(WidgetFile*, QString);
    /**
     * @brief verticalSplitterChanged signal is send if the console or the errortable (simpleoutput)
     *        is open or close but not every time the splitter is moved
     */
    void verticalSplitterChanged();
    void messageFromCurrentFile(QString);

public slots:
    bool newFile();
    void openAssociatedFile();

    void sendFilenameChanged(WidgetFile* w, QString name) { emit filenameChanged(w,name); emit filenameChanged(name); }

    /**
     * The main parts of the slots are just used to call the same slots on the current file
     */
    void save() { this->currentWidgetFile()->save(); }
    void saveAs() { this->currentWidgetFile()->saveAs(); }
    void builTex(void);
    void bibtex(void) { this->currentWidgetFile()->bibtex(); }
    void clean(void) { this->currentWidgetFile()->clean(); }
    void openFindReplaceWidget() { this->currentWidgetFile()->openFindReplaceWidget(); }
    void undo();
    void redo();
    void copy();
    void cut();
    void paste();
    void wrapEnvironment();
    void setPdfSynchronized(bool pdfSynchronized);
    void setPdfViewerInItsOwnWidget(bool ownWidget);
    void jumpToPdfFromSource();
    void rehighlight();
    void toggleConsole();
    void toggleErrorTable();

    void setCurrent(WidgetFile * widget);
    void setCurrent(int index);

    void setDictionaryFromAction();



private slots:
    void sendCursorPositionChanged(int x, int y) { emit cursorPositionChanged(x, y); }
    void sendVerticalSplitterChanged() { emit verticalSplitterChanged(); }
    void sendMessageFromCurrentFile(QString message) { emit messageFromCurrentFile(message); }
    void onFileSystemChanged(QString filename);

private:

    void createMasterConnexions(WidgetFile * widget, WidgetFile * master, AssociatedFile::Type type = AssociatedFile::INPUT);
    void deleteMasterConnexions(WidgetFile * widget, AssociatedFile::Type type = AssociatedFile::INPUT);
    void changeConnexions(WidgetFile *oldFile);

    explicit FileManager(QObject *parent = 0);
    QList<WidgetFile *> _widgetFiles;
    int _currentWidgetFileId;
    bool _pdfSynchronized;
    QFileSystemWatcher * _fileSystemWatcher;
    MainWindow * _mainWindow;
    WidgetPdfViewerWrapper * _widgetPdfViewerWrapper;
};

#endif // FILEMANAGER_H
