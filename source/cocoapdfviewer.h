#ifndef COCOAPDFVIEWER_H
#define COCOAPDFVIEWER_H

#include <QMacCocoaViewContainer>
#include <QWidget>

class CocoaPdfViewer : public QMacCocoaViewContainer
{
public:
    CocoaPdfViewer(QWidget *parent);
    ~CocoaPdfViewer();
};

#endif // COCOAPDFVIEWER_H
