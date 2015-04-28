#include "textdocument.h"

#include <QTextBlock>

TextDocument::TextDocument(QObject *parent) : QTextDocument(parent),
    _lastSaveRevision(0)
{
    connect(this, SIGNAL(modificationChanged(bool)), this, SLOT(onModificationChanged(bool)));
}

void TextDocument::onModificationChanged(bool modified) {
    if(!modified)
    {
        this->updateRevisions();
    }
}

void TextDocument::setUncommittedLines(QList<int> lines)
{
    foreach(int l, lines)
    {
        if(findBlockByNumber(l - 1).revision() == lastSaveRevision())
        {
            findBlockByNumber(l - 1).setRevision(-_lastSaveRevision - 1);
        }
    }
}

void TextDocument::resetRevisions()
{
    _lastSaveRevision = revision();

    for (QTextBlock block = begin(); block.isValid(); block = block.next())
        block.setRevision(_lastSaveRevision);
}

void TextDocument::updateRevisions()
{
    int oldLastSaveRevision = _lastSaveRevision;
    _lastSaveRevision = revision();

    if (oldLastSaveRevision != _lastSaveRevision) {
        for (QTextBlock block = begin(); block.isValid(); block = block.next()) {
            if (block.revision() < 0 || block.revision() != oldLastSaveRevision)
                block.setRevision(-_lastSaveRevision - 1);
            else
                block.setRevision(_lastSaveRevision);
        }
    }
}
