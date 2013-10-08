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

class WidgetFile : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetFile(QWidget *parent = 0);
    ~WidgetFile();
    void initTheme();

    void pdflatex(void);
    void bibtex(void);
    void save(void);
    void saveAs(void);
    bool isEmpty();
    WidgetTextEdit * widgetTextEdit() { return _widgetTextEdit; }
    WidgetPdfViewer * widgetPdfViewer() { return _widgetPdfViewer; }
    SyntaxHighlighter * syntaxHighlighter() { return _syntaxHighlighter; }
    MiniSplitter * verticalSplitter() { return _verticalSplitter; }

    void open(QString filename);

signals:
    
public slots:
    void openFindReplaceWidget(void);
    void closeFindReplaceWidget(void);

private:
    WidgetTextEdit * _widgetTextEdit;
    MiniSplitter * _horizontalSplitter;
    MiniSplitter * _verticalSplitter;
    WidgetConsole * _widgetConsole;
    WidgetFindReplace * _widgetFindReplace;
    WidgetLineNumber * widgetLineNumber;
    WidgetPdfViewer * _widgetPdfViewer;
    WidgetSimpleOutput * _widgetSimpleOutput;
    WidgetLineNumber * _widgetLineNumber;
    SyntaxHighlighter * _syntaxHighlighter;
    
};

#endif // WIDGETFILE_H
