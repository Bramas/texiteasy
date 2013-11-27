#ifndef WIDGETFILE_H
#define WIDGETFILE_H

#include <QWidget>
#include <QString>

class WidgetTextEdit;
class MiniSplitter;
class WidgetConsole;
class WidgetFindReplace;
class WidgetLineNumber;
class WidgetPdfViewer;
class WidgetPdfDocument;
class WidgetSimpleOutput;
class WidgetLineNumber;
class SyntaxHighlighter;
class File;
class Hunspell;

class WidgetFile : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetFile(QWidget *parent = 0);
    ~WidgetFile();
    void initTheme();

    void builTex(QString command);
    void bibtex(void);
    void clean(void);
    WidgetFile * masterFile() { return _masterFile; }
    void save();
    void saveAs(void);
    void setMasterFile(WidgetFile * master) { _masterFile = master; }
    bool isEmpty();
    WidgetTextEdit * widgetTextEdit() { return _widgetTextEdit; }
    WidgetPdfViewer * widgetPdfViewer() { return _widgetPdfViewer; }
    SyntaxHighlighter * syntaxHighlighter() { return _syntaxHighlighter; }
    MiniSplitter * verticalSplitter() { return _verticalSplitter; }
    MiniSplitter * editorSplitter() { return _editorSplitter; }

    void open(QString filename);
    void reload();

    bool isConsoleOpen(void);
    bool isErrorTableOpen(void);

    File * file();

    void setFileToBuild(File * file);


    Hunspell * spellChecker();
    QString spellCheckerEncoding();
    QString dictionary() { return _dictionary; }
    void setDictionary(QString dico);


    void addWidgetPdfViewerToSplitter();

signals:
    void verticalSplitterChanged();

public slots:

    void splitEditor(bool split);
    void openConsole(void);
    void openErrorTable(void);

    void closeErrorTable(void);
    void closeConsole(void);

    void toggleErrorTable(void);
    void toggleConsole(void);

    void openFindReplaceWidget(void);
    void closeFindReplaceWidget(void);

private:
    QString _dictionary;
    WidgetTextEdit * _widgetTextEdit;
    WidgetTextEdit * _widgetTextEdit2;
    MiniSplitter * _horizontalSplitter;
    MiniSplitter * _verticalSplitter;
    MiniSplitter * _editorSplitter;
    WidgetConsole * _widgetConsole;
    WidgetFindReplace * _widgetFindReplace;
    WidgetLineNumber * widgetLineNumber;
    WidgetPdfViewer * _widgetPdfViewer;
    WidgetSimpleOutput * _widgetSimpleOutput;
    WidgetLineNumber * _widgetLineNumber;
    Hunspell * _spellChecker;
    SyntaxHighlighter * _syntaxHighlighter;
    
    WidgetFile * _masterFile;
};

#endif // WIDGETFILE_H
