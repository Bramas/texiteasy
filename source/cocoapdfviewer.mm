#include "cocoapdfviewer.h"
#include "mainwindow.h"
#include "widgetpdfdocument.h"
#include "widgetpdfviewer.h"

#include <QVBoxLayout>

#include <Cocoa/Cocoa.h>

WidgetPdfViewer::WidgetPdfViewer(QWidget *parent) :
    QMacCocoaViewContainer(0, parent)
    //ui(new Ui::WidgetPdfViewer),
{
    // Many Cocoa objects create temporary autorelease objects,
    // so create a pool to catch them.
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    // Create the NSSearchField, set it on the QCocoaViewContainer.
    NSVisualEffectView *search = [[NSVisualEffectView alloc] init];
    [search setBlendingMode:NSVisualEffectBlendingModeBehindWindow];
    [search setMaterial:NSVisualEffectMaterialDark];
    [search setAppearance:[NSAppearance appearanceNamed:NSAppearanceNameVibrantDark]];

    //[search.window setAppearance:[NSAppearance appearanceNamed:NSAppearanceNameVibrantLight]];
    //[search setMaterial:NSVisualEffectMaterialLight];


    _nativeWidget = new QMacNativeWidget(search);
    _verticalLayout = new QVBoxLayout(_nativeWidget);
    _nativeWidget->setLayout(_verticalLayout);
    _widgetPdfDocument = new WidgetPdfDocument(_nativeWidget);
    _widgetPdfDocument->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    _verticalLayout->addWidget(_widgetPdfDocument);
    _verticalLayout->setSpacing(0);
    _verticalLayout->setMargin(0);
    _verticalLayout->setContentsMargins(0,0,0,0);

    NSView *nativeWidgetView = reinterpret_cast<NSView *>(_nativeWidget->winId());

    //[search setAutoresizesSubviews:YES];
    [nativeWidgetView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
    //[nativeWidgetView setAutoresizesSubviews:YES];

    [search addSubview:nativeWidgetView positioned:NSWindowAbove relativeTo:nil];
    _nativeWidget->show();
    _widgetPdfDocument->show();
    setCocoaView(search);
    // Release our reference, since our super class takes ownership and we
    // don't need it anymore.
    [search release];
    // Clean up our pool as we no longer need it.
    [pool release];

}


CocoaPdfViewer::CocoaPdfViewer(QWidget * parent)
    : QMacCocoaViewContainer(0, parent)
{
    // Many Cocoa objects create temporary autorelease objects,
    // so create a pool to catch them.
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    // Create the NSSearchField, set it on the QCocoaViewContainer.
    NSVisualEffectView *search = [[NSVisualEffectView alloc] init];
    [search setBlendingMode:NSVisualEffectBlendingModeBehindWindow];
    [search setMaterial:NSVisualEffectMaterialDark];
    [search setAppearance:[NSAppearance appearanceNamed:NSAppearanceNameVibrantDark]];

    //[search.window setAppearance:[NSAppearance appearanceNamed:NSAppearanceNameVibrantLight]];
    //[search setMaterial:NSVisualEffectMaterialLight];

    setCocoaView(search);
    // Release our reference, since our super class takes ownership and we
    // don't need it anymore.
    [search release];
    // Clean up our pool as we no longer need it.
    [pool release];
}

CocoaPdfViewer::~CocoaPdfViewer()
{

}

