#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QList>
#include <QMutex>
#include "file.h"
#include "macroengine.h"
#include "widgetfile.h"

class MainWindow;
class QMimeData;
class MainWindow;
class QMessageBox;
typedef QWidget WidgetPdfViewerWrapper;

/**
 * @brief Provides informations about opened files.
 * @author Quentin BRAMAS
 *
 * This class provide usefull information about which file is currently opened.
 * Other class can ask for the current file information, and can be informed when the state of the current
 * file change.
 * A lots of slots are just used to call the same slots on the current file.
 */

class FileManager : public QObject
{
    Q_OBJECT
public:
    static FileManager Instance;
    void init();
    void initWidgetPdfViewerWrapper();
    WidgetFile * currentWidgetFile() { if(_currentWidgetFileId < 0) return 0; return _widgetFiles.at(_currentWidgetFileId); }

    void initTheme();

    bool open(QString filename, MainWindow *window);
    void close(WidgetFile *widget);
    int count() { return _widgetFiles.count(); }
    AssociatedFile reverseAssociation(QString filename, int* index = 0);

    File * file(int index);
    WidgetFile * widgetFile(QString filename);
    WidgetFile * widgetFile(int index) { return _widgetFiles.at(index); }
    void setMainWindow(MainWindow * m) { _mainWindow = m; }
    MainWindow * mainWindow() { return _mainWindow; }
    bool handleMimeData(const QMimeData * mimeData);
    WidgetPdfViewerWrapper * widgetPdfViewerWrapper() { return _widgetPdfViewerWrapper; }

    bool onMacroTriggered(Macro macro, bool force = false);

    void reopenWithEncoding(QString codec);
    void setEncoding(QString codec);

    void createMasterConnexions(WidgetFile * widget, WidgetFile * master, AssociatedFile::Type type = AssociatedFile::INPUT);
    void deleteMasterConnexions(WidgetFile * widget, AssociatedFile::Type type = AssociatedFile::INPUT);
signals:
    void cursorPositionChanged(int,int);
    void requestOpenFile(QString);
    void currentFileModified(bool);
    void currentFileModified();
    void filenameChanged(QString);
    void filenameChanged(WidgetFile*, QString);
    /**
     * @brief verticalSplitterChanged signal is send if the console or the errortable (simpleoutput)
     *        is open or close but not every time the splitter is moved
     */
    void verticalSplitterChanged();
    void messageFromCurrentFile(QString);


public slots:
    bool newFile(MainWindow *mainWindow);
    void openAssociatedFile();
    void goToSection();

    void sendFilenameChanged(WidgetFile* w, QString name) { emit filenameChanged(w,name); emit filenameChanged(name); }

    void updateLineWrapMode();
    void splitEditor(bool split) { if(this->currentWidgetFile()) this->currentWidgetFile()->splitEditor(split); }
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
    void comment();
    void uncomment();
    void toggleComment();
    void checkGrammar();
    void setPdfSynchronized(bool pdfSynchronized);
    void setPdfViewerInItsOwnWidget(bool ownWidget);
    void jumpToPdfFromSource();
    void rehighlight();

    void setCurrent(WidgetFile * widget);
    void setCurrent(int index);

    void setDictionaryFromAction();
    void checkCurrentFileSystemChanges();
    void onFileSystemChanged(QString filename);
    void onFileSystemChanged(WidgetFile * widget);
    void ensurePdfViewerIsVisible();

    void setCurrentPdfToPdfViewer();
    void openCurrentPdf();


private slots:
    void sendCursorPositionChanged(int x, int y) { emit cursorPositionChanged(x, y); }
    void sendVerticalSplitterChanged() { emit verticalSplitterChanged(); }
    void sendMessageFromCurrentFile(QString message) { emit messageFromCurrentFile(message); }
    void sendCurrentFileModified(bool b) { emit currentFileModified(b); emit currentFileModified(); }

private:

    void changeConnexions(WidgetFile *oldFile);

    explicit FileManager(QObject *parent = 0);

    QMessageBox * _askReloadMessageBox;
    QMutex _askReloadMutex;
    QList<WidgetFile *> _widgetFiles;
    int _currentWidgetFileId;
    bool _pdfSynchronized;
    MainWindow * _mainWindow;
    WidgetPdfViewerWrapper * _widgetPdfViewerWrapper;
};

#endif // FILEMANAGER_H
