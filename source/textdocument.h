#ifndef TEXTDOCUMENT_H
#define TEXTDOCUMENT_H

#include <QTextDocument>

class TextDocument : public QTextDocument
{
    Q_OBJECT
public:

    TextDocument(QObject * parent = 0);

    void resetRevisions();
    int lastSaveRevision() { return _lastSaveRevision; }
private slots:
    void onModificationChanged(bool modified);
private:
    void updateRevisions();

    int _lastSaveRevision;
};

#endif // TEXTDOCUMENT_H
