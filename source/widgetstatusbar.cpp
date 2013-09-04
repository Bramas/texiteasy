#include "widgetstatusbar.h"
#include "ui_widgetstatusbar.h"

#include <QPushButton>
#include <QDebug>
#include <QSplitter>

WidgetStatusBar::WidgetStatusBar(QWidget *parent, QSplitter * leftSplitter) :
    QStatusBar(parent),
    _leftSplitter(leftSplitter),
    ui(new Ui::WidgetStatusBar)
{
    ui->setupUi(this);

    _pushButtonConsole = new QPushButton(trUtf8("Console"));
    _pushButtonConsole->setCheckable(true);
    _pushButtonConsole->setChecked(false);
    this->addPermanentWidget(_pushButtonConsole);

    _pushButtonErreurs = new QPushButton(trUtf8("Erreurs"));
    _pushButtonErreurs->setCheckable(true);
    _pushButtonErreurs->setChecked(false);
    this->addPermanentWidget(_pushButtonErreurs);


    connect(_leftSplitter, SIGNAL(splitterMoved(int,int)), this, SLOT(onLeftSplitterMoved(int,int)));
    connect(_pushButtonConsole, SIGNAL(clicked()), this, SLOT(toggleConsole()));
    connect(_pushButtonErreurs, SIGNAL(clicked()), this, SLOT(toggleErrorTable()));
}

WidgetStatusBar::~WidgetStatusBar()
{
    delete ui;
}


void WidgetStatusBar::onLeftSplitterMoved(int pos, int index)
{
    QList<int> sizes = _leftSplitter->sizes();
    _pushButtonErreurs->setChecked(sizes[2] != 0);
    _pushButtonConsole->setChecked(sizes[3] != 0);
}

void WidgetStatusBar::toggleConsole()
{
    QList<int> sizes = _leftSplitter->sizes();
    if(sizes[3] == 0)
    {
        sizes.replace(0, sizes[0] - 60);
        sizes.replace(3, 60);
    }
    else
    {
        sizes.replace(0, sizes[0] + sizes[3]);
        sizes.replace(3, 00);
    }
    _leftSplitter->setSizes(sizes);
}

void WidgetStatusBar::toggleErrorTable()
{
    QList<int> sizes = _leftSplitter->sizes();
    if(sizes[2] == 0)
    {
        sizes.replace(0, sizes[0] - 60);
        sizes.replace(2, 60);
    }
    else
    {
        sizes.replace(0, sizes[0] + sizes[2]);
        sizes.replace(2, 00);
    }
    _leftSplitter->setSizes(sizes);
}
