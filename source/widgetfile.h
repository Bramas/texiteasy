#ifndef WIDGETFILE_H
#define WIDGETFILE_H

#include <QWidget>
#include <QString>
#include <QScriptValue>
#include <QScriptEngine>
#include <QDebug>

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
class MainWindow;
class File;
class Hunspell;

class WidgetFile : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetFile(MainWindow *parent = 0);
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

    void setWindow(MainWindow * window) { _window = window; }
    MainWindow * window() { return _window; }
    void addWidgetPdfViewerToSplitter();


    static QScriptValue toScriptValue(QScriptEngine *engine, WidgetFile * const &s)
    {
      QScriptValue obj = engine->newQObject(const_cast<WidgetFile *>(s));
      return obj;
    }

    static void fromScriptValue(const QScriptValue &obj, WidgetFile *&s)
    {
        Q_UNUSED(obj);
        Q_UNUSED(s);
    }

signals:
    void verticalSplitterChanged();

public slots:
    WidgetTextEdit * widgetTextEdit() { return _widgetTextEdit; }
    WidgetTextEdit * widgetTextEdit2() { return _widgetTextEdit2; }

    bool isEditorSplited();
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
    MainWindow * _window;
    WidgetFile * _masterFile;
};

Q_DECLARE_METATYPE(WidgetFile*)


#endif // WIDGETFILE_H
