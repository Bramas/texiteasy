/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company.  For licensing terms and
** conditions see http://www.qt.io/terms-conditions.  For further information
** use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file.  Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, The Qt Company gives you certain additional
** rights.  These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "taskwindow.h"

#include "task.h"
#include "taskmodel.h"
#include "icontext.h"
#include "builder.h"
#include "widgettextedit.h"
#include "latexoutputfilter.h"
#include "widgetfile.h"
#include "mainwindow.h"
#include "filemanager.h"
#include "configmanager.h"

#include <QDebug>
#include <QApplication>
#include <QStyle>




namespace {
const int ELLIPSIS_GRADIENT_WIDTH = 16;
}

class TaskView : public QListView
{
public:
    TaskView(QWidget *parent = 0);
    ~TaskView();
    void resizeEvent(QResizeEvent *e);
};

class TaskWindowContext : public IContext
{
public:
    TaskWindowContext(QWidget *widget);
};

TaskView::TaskView(QWidget *parent)
    : QListView(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    QFontMetrics fm(font());
    int vStepSize = fm.height() + 3;
    if (vStepSize < TaskDelegate::Positions::minimumHeight())
        vStepSize = TaskDelegate::Positions::minimumHeight();

    verticalScrollBar()->setSingleStep(vStepSize);
    this->setStyleSheet("QListView::item { color:#ffffff; }");
}

TaskView::~TaskView()
{ }

void TaskView::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e)
    static_cast<TaskDelegate *>(itemDelegate())->emitSizeHintChanged(selectionModel()->currentIndex());
}

/////
// TaskWindow
/////

class TaskWindowPrivate
{
public:
    TaskModel *m_model;
    TaskFilterModel *m_filter;
    TaskView *m_listview;
    TaskWindowContext *m_taskWindowContext;
    QMenu *m_contextMenu;
    QToolButton *m_filterWarningsButton;
    QToolButton *m_categoriesButton;
    QMenu *m_categoriesMenu;
    QList<QAction *> m_actions;
};

static QToolButton *createFilterButton(QIcon icon, const QString &toolTip,
                                       QObject *receiver, const char *slot)
{
    QToolButton *button = new QToolButton;
    button->setIcon(icon);
    button->setToolTip(toolTip);
    button->setCheckable(true);
    button->setChecked(true);
    button->setAutoRaise(true);
    button->setEnabled(true);
    QObject::connect(button, SIGNAL(toggled(bool)), receiver, slot);
    return button;
}

TaskWindow::TaskWindow() : d(new TaskWindowPrivate)
{

    d->m_model = new TaskModel(this);
    d->m_filter = new TaskFilterModel(d->m_model);
    d->m_listview = new TaskView;

    d->m_listview->setModel(d->m_filter);
    d->m_listview->setFrameStyle(QFrame::NoFrame);
    d->m_listview->setWindowTitle(tr("Issues"));
    d->m_listview->setSelectionMode(QAbstractItemView::SingleSelection);
    TaskDelegate *tld = new TaskDelegate(this);
    d->m_listview->setItemDelegate(tld);
    d->m_listview->setWindowIcon(QIcon(ICON_WINDOW));
    d->m_listview->setContextMenuPolicy(Qt::ActionsContextMenu);
    d->m_listview->setAttribute(Qt::WA_MacShowFocusRect, false);

    d->m_taskWindowContext = new TaskWindowContext(d->m_listview);

    //Core::ICore::addContextObject(d->m_taskWindowContext);

    connect(d->m_listview->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            tld, SLOT(currentChanged(QModelIndex,QModelIndex)));

    connect(d->m_listview->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(currentChanged(QModelIndex)));
    if(ConfigManager::Instance.doubleClickToGoToError()) {
        connect(d->m_listview, SIGNAL(activated(QModelIndex)),
                this, SLOT(triggerDefaultHandler(QModelIndex)));
    } else {
        connect(d->m_listview->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                this, SLOT(triggerDefaultHandler(QModelIndex)));
    }

    d->m_contextMenu = new QMenu(d->m_listview);

    d->m_listview->setContextMenuPolicy(Qt::ActionsContextMenu);

    d->m_filterWarningsButton = createFilterButton(
                QIcon(ICON_WARNING),
                tr("Show Warnings"), this, SLOT(setShowWarnings(bool)));

    d->m_categoriesButton = new QToolButton;
    d->m_categoriesButton->setIcon(QIcon(ICON_FILTER));
    d->m_categoriesButton->setToolTip(tr("Filter by categories"));
    d->m_categoriesButton->setProperty("noArrow", true);
    d->m_categoriesButton->setAutoRaise(true);
    d->m_categoriesButton->setPopupMode(QToolButton::InstantPopup);

    d->m_categoriesMenu = new QMenu(d->m_categoriesButton);
    connect(d->m_categoriesMenu, SIGNAL(aboutToShow()), this, SLOT(updateCategoriesMenu()));
    connect(d->m_categoriesMenu, SIGNAL(triggered(QAction*)), this, SLOT(filterCategoryTriggered(QAction*)));

    d->m_categoriesButton->setMenu(d->m_categoriesMenu);

    /*
    QObject *hub = TaskHub::instance();
    connect(hub, SIGNAL(categoryAdded(Id,QString,bool)),
            this, SLOT(addCategory(Id,QString,bool)));
    connect(hub, SIGNAL(taskAdded(Task)),
            this, SLOT(addTask(Task)));
    connect(hub, SIGNAL(taskRemoved(Task)),
            this, SLOT(removeTask(Task)));
    connect(hub, SIGNAL(taskLineNumberUpdated(uint,int)),
            this, SLOT(updatedTaskLineNumber(uint,int)));
    connect(hub, SIGNAL(taskFileNameUpdated(uint,QString)),
            this, SLOT(updatedTaskFileName(uint,QString)));
    connect(hub, SIGNAL(tasksCleared(Id)),
            this, SLOT(clearTasks(Id)));
    connect(hub, SIGNAL(categoryVisibilityChanged(Id,bool)),
            this, SLOT(setCategoryVisibility(Id,bool)));
    connect(hub, SIGNAL(popupRequested(int)),
            this, SLOT(popup(int)));
    connect(hub, SIGNAL(showTask(uint)),
            this, SLOT(showTask(uint)));
    connect(hub, SIGNAL(openTask(uint)),
            this, SLOT(openTask(uint)));
            */

    connect(d->m_filter, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(emitBadgeNumber()));
    connect(d->m_filter, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(emitBadgeNumber()));
    connect(d->m_filter, SIGNAL(modelReset()),
            this, SLOT(emitBadgeNumber()));
}

TaskWindow::~TaskWindow()
{
    //Core::ICore::removeContextObject(d->m_taskWindowContext);
    delete d->m_filterWarningsButton;
    delete d->m_listview;
    delete d->m_filter;
    delete d->m_model;
    delete d;
}

void TaskWindow::emitBadgeNumber()
{
    emit setBadgeNumber(d->m_filter->rowCount());
}
/*
static ITaskHandler *handler(QAction *action)
{
    QVariant prop = action->property("ITaskHandler");
    ITaskHandler *handler = qobject_cast<ITaskHandler *>(prop.value<QObject *>());
    QTC_CHECK(handler);
    return handler;
}*/

void TaskWindow::delayedInitialization()
{
    static bool alreadyDone = false;
    if (alreadyDone)
        return;

    alreadyDone = true;
/*
        QAction *action = h->createAction(this);
        Q_ASSERT(action);
        action->setProperty("ITaskHandler", qVariantFromValue(qobject_cast<QObject*>(h)));
        connect(action, SIGNAL(triggered()), this, SLOT(actionTriggered()));
        d->m_actions << action;

        Id id = h->actionManagerId();
        if (id.isValid()) {
            Core::Command *cmd = Core::ActionManager::instance()
                    ->registerAction(action, id, d->m_taskWindowContext->context(), true);
            action = cmd->action();
        }
        d->m_listview->addAction(action);
    }*/

    // Disable everything for now:
    currentChanged(QModelIndex());
}

QList<QWidget*> TaskWindow::toolBarWidgets() const
{
    return QList<QWidget*>() << d->m_filterWarningsButton << d->m_categoriesButton;
}

QWidget *TaskWindow::outputWidget()
{
    return d->m_listview;
}

void TaskWindow::clearTasks(Id categoryId)
{
    d->m_model->clearTasks(categoryId);

    emit tasksChanged();
    emit tasksCleared();
    navigateStateChanged();
}

void TaskWindow::setCategoryVisibility(Id categoryId, bool visible)
{
    if (categoryId == 0)
        return;

    QList<Id> categories = d->m_filter->filteredCategories();

    if (visible)
        categories.removeOne(categoryId);
    else
        categories.append(categoryId);

    d->m_filter->setFilteredCategories(categories);
}

void TaskWindow::currentChanged(const QModelIndex &index)
{
    const Task task = index.isValid() ? d->m_filter->task(index) : Task();
    foreach (QAction *action, d->m_actions) {
        //ITaskHandler *h = handler(action);
        //action->setEnabled((task.isNull() || !h) ? false : h->canHandle(task));
    }
}

void TaskWindow::visibilityChanged(bool visible)
{
    if (visible)
        delayedInitialization();
}

void TaskWindow::addCategory(Id categoryId, const QString &displayName, bool visible)
{
    d->m_model->addCategory(categoryId, displayName);
    if (!visible) {
        QList<Id> filters = d->m_filter->filteredCategories();
        filters += categoryId;
        d->m_filter->setFilteredCategories(filters);
    }
}

void TaskWindow::addTask(const Task &task)
{
    d->m_model->addTask(task);

    emit tasksChanged();
    navigateStateChanged();

    if (task.type == Task::Error && d->m_filter->filterIncludesErrors()
            && !d->m_filter->filteredCategories().contains(task.category))
        flash();
}

void TaskWindow::removeTask(const Task &task)
{
    d->m_model->removeTask(task);

    emit tasksChanged();
    navigateStateChanged();
}

void TaskWindow::updatedTaskFileName(unsigned int id, const QString &fileName)
{
    d->m_model->updateTaskFileName(id, fileName);
    emit tasksChanged();
}

void TaskWindow::updatedTaskLineNumber(unsigned int id, int line)
{
    d->m_model->updateTaskLineNumber(id, line);
    emit tasksChanged();
}

void TaskWindow::showTask(unsigned int id)
{
    int sourceRow = d->m_model->rowForId(id);
    QModelIndex sourceIdx = d->m_model->index(sourceRow, 0);
    QModelIndex filterIdx = d->m_filter->mapFromSource(sourceIdx);
    d->m_listview->setCurrentIndex(filterIdx);
   // popup(Core::IOutputPane::ModeSwitch);
}

void TaskWindow::openTask(unsigned int id)
{
    int sourceRow = d->m_model->rowForId(id);
    QModelIndex sourceIdx = d->m_model->index(sourceRow, 0);
    QModelIndex filterIdx = d->m_filter->mapFromSource(sourceIdx);
    triggerDefaultHandler(filterIdx);
}

void TaskWindow::triggerDefaultHandler(const QModelIndex &index)
{
    qDebug()<<index.isValid();
    if (!index.isValid())
        return;
    Task task(d->m_model->task(index));
    qDebug()<<task.isNull();
    if (task.isNull())
        return;
    qDebug()<<task.file.trimmed().isEmpty();
    if (task.file.trimmed().isEmpty())
        return;

    bool found = false;
    foreach(const AssociatedFile &associatedFile, _widgetTextEdit->widgetFile()->file()->associatedFiles())
    {
        if(associatedFile.filename == task.file)
        {
            found = true;
        }
    }
    qDebug()<<_widgetTextEdit->getCurrentFile()->getFilename();
    qDebug()<<task.file;
    if (!found && _widgetTextEdit->getCurrentFile()->getFilename() != task.file)
        return;

    QString err = task.description;
    int line = task.movedLine;
    QRegExp undefinedCommand("Undefined control sequence.* (\\\\[a-zA-Z]+)");
    QString search("");
    if(err.indexOf(undefinedCommand) != -1)
    {
        search = undefinedCommand.capturedTexts().at(1);
    }
    this->_widgetTextEdit->widgetFile()->window()->open(task.file);
    WidgetFile * w = FileManager::Instance.widgetFile(task.file);
    if(w){
        w->widgetTextEdit()->goToLine(line,search);
    }
    else
    {
        qDebug()<<"widgetFile "<<task.file<<" not found";
    }
}

void TaskWindow::actionTriggered()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action || !action->isEnabled())
        return;
    /*
    ITaskHandler *h = handler(action);
    if (!h)
        return;

    QModelIndex index = d->m_listview->selectionModel()->currentIndex();
    Task task = d->m_filter->task(index);
    if (task.isNull())
        return;

    h->handle(task);*/
}

void TaskWindow::setShowWarnings(bool show)
{
    d->m_filter->setFilterIncludesWarnings(show);
    d->m_filter->setFilterIncludesUnknowns(show); // "Unknowns" are often associated with warnings
}

void TaskWindow::updateCategoriesMenu()
{
    typedef QMap<QString, Id>::ConstIterator NameToIdsConstIt;

    d->m_categoriesMenu->clear();

    const QList<Id> filteredCategories = d->m_filter->filteredCategories();

    QMap<QString, Id> nameToIds;
    foreach (Id categoryId, d->m_model->categoryIds())
        nameToIds.insert(d->m_model->categoryDisplayName(categoryId), categoryId);

    const NameToIdsConstIt cend = nameToIds.constEnd();
    for (NameToIdsConstIt it = nameToIds.constBegin(); it != cend; ++it) {
        const QString &displayName = it.key();
        const Id categoryId = it.value();
        QAction *action = new QAction(d->m_categoriesMenu);
        action->setCheckable(true);
        action->setText(displayName);
        action->setData(categoryId);
        action->setChecked(!filteredCategories.contains(categoryId));
        d->m_categoriesMenu->addAction(action);
    }
}

void TaskWindow::filterCategoryTriggered(QAction *action)
{
    Id categoryId = action->data().toString();

    setCategoryVisibility(categoryId, action->isChecked());
}

int TaskWindow::taskCount(Id category) const
{
    return d->m_model->taskCount(category);
}

int TaskWindow::errorTaskCount(Id category) const
{
    return d->m_model->errorTaskCount(category);
}

int TaskWindow::warningTaskCount(Id category) const
{
    return d->m_model->warningTaskCount(category);
}

int TaskWindow::priorityInStatusBar() const
{
    return 90;
}

void TaskWindow::clearContents()
{
    // clear all tasks in all displays
    // Yeah we are that special
    //TaskHub::clearTasks();
    d->m_model->clearTasks();
}

bool TaskWindow::hasFocus() const
{
    return d->m_listview->window()->focusWidget() == d->m_listview;
}

bool TaskWindow::canFocus() const
{
    return d->m_filter->rowCount();
}

void TaskWindow::setFocus()
{
    if (d->m_filter->rowCount()) {
        d->m_listview->setFocus();
        if (d->m_listview->currentIndex() == QModelIndex())
            d->m_listview->setCurrentIndex(d->m_filter->index(0,0, QModelIndex()));
    }
}

bool TaskWindow::canNext() const
{
    return d->m_filter->rowCount();
}

bool TaskWindow::canPrevious() const
{
    return d->m_filter->rowCount();
}

void TaskWindow::goToNext()
{
    if (!canNext())
        return;
    QModelIndex startIndex = d->m_listview->currentIndex();
    QModelIndex currentIndex = startIndex;

    if (startIndex.isValid()) {
        do {
            int row = currentIndex.row() + 1;
            if (row == d->m_filter->rowCount())
                row = 0;
            currentIndex = d->m_filter->index(row, 0);
            if (d->m_filter->hasFile(currentIndex))
                break;
        } while (startIndex != currentIndex);
    } else {
        currentIndex = d->m_filter->index(0, 0);
    }
    d->m_listview->setCurrentIndex(currentIndex);
    triggerDefaultHandler(currentIndex);
}

void TaskWindow::goToPrev()
{
    if (!canPrevious())
        return;
    QModelIndex startIndex = d->m_listview->currentIndex();
    QModelIndex currentIndex = startIndex;

    if (startIndex.isValid()) {
        do {
            int row = currentIndex.row() - 1;
            if (row < 0)
                row = d->m_filter->rowCount() - 1;
            currentIndex = d->m_filter->index(row, 0);
            if (d->m_filter->hasFile(currentIndex))
                break;
        } while (startIndex != currentIndex);
    } else {
        currentIndex = d->m_filter->index(0, 0);
    }
    d->m_listview->setCurrentIndex(currentIndex);
    triggerDefaultHandler(currentIndex);
}

bool TaskWindow::canNavigate() const
{
    return true;
}

/////
// Delegate
/////

TaskDelegate::TaskDelegate(QObject *parent) :
    QStyledItemDelegate(parent),
    m_cachedHeight(0)
{ }

TaskDelegate::~TaskDelegate()
{
}

QSize TaskDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 opt = option;
    initStyleOption(&opt, index);

    const QAbstractItemView * view = qobject_cast<const QAbstractItemView *>(opt.widget);
    const bool selected = (view->selectionModel()->currentIndex() == index);
    QSize s;
    s.setWidth(option.rect.width());

    if (!selected && option.font == m_cachedFont && m_cachedHeight > 0) {
        s.setHeight(m_cachedHeight);
        return s;
    }

    QFontMetrics fm(option.font);
    int fontHeight = fm.height();
    int fontLeading = fm.leading();

    TaskModel *model = static_cast<TaskFilterModel *>(view->model())->taskModel();
    Positions positions(option, model);

    if (selected) {
        QString description = index.data(TaskModel::Description).toString();
        // Layout the description
        int leading = fontLeading;
        int height = 0;
        description.replace(QLatin1Char('\n'), QChar::LineSeparator);
        QTextLayout tl(description);
        tl.setAdditionalFormats(index.data(TaskModel::Task_t).value<Task>().formats);
        tl.beginLayout();
        while (true) {
            QTextLine line = tl.createLine();
            if (!line.isValid())
                break;
            line.setLineWidth(positions.textAreaWidth());
            height += leading;
            line.setPosition(QPoint(0, height));
            height += static_cast<int>(line.height());
        }
        tl.endLayout();

        s.setHeight(height + leading + fontHeight + 3);
    } else {
        s.setHeight(fontHeight + 3);
    }
    if (s.height() < positions.minimumHeight())
        s.setHeight(positions.minimumHeight());

    if (!selected) {
        m_cachedHeight = s.height();
        m_cachedFont = option.font;
    }

    return s;
}

void TaskDelegate::emitSizeHintChanged(const QModelIndex &index)
{
    emit sizeHintChanged(index);
}

void TaskDelegate::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    emit sizeHintChanged(current);
    emit sizeHintChanged(previous);
}

void TaskDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 opt = option;
    initStyleOption(&opt, index);
    painter->save();

    QFontMetrics fm(opt.font);
    QColor backgroundColor;
    QColor textColor;

    const QAbstractItemView * view = qobject_cast<const QAbstractItemView *>(opt.widget);
    bool selected = view->selectionModel()->currentIndex() == index;

    if (selected) {
        painter->setBrush(opt.palette.highlight().color());
        backgroundColor = opt.palette.highlight().color();
    } else {
        painter->setBrush(opt.palette.background().color());
        backgroundColor = opt.palette.background().color();
    }
    painter->setPen(Qt::NoPen);
    painter->drawRect(opt.rect);

    // Set Text Color
    if (selected)
        textColor = opt.palette.highlightedText().color();
    else
        textColor = opt.palette.text().color();

    painter->setPen(textColor);

    TaskModel *model = static_cast<TaskFilterModel *>(view->model())->taskModel();
    Positions positions(opt, model);

    // Paint TaskIconArea:
    QIcon icon = index.data(TaskModel::Icon).value<QIcon>();
    painter->drawPixmap(positions.left(), positions.top(),
                        icon.pixmap(positions.taskIconWidth(), positions.taskIconHeight()));

    // Paint TextArea:
    if (!selected) {
        // in small mode we lay out differently
        QString bottom = index.data(TaskModel::Description).toString().split(QLatin1Char('\n')).first();
        painter->setClipRect(positions.textArea());
        painter->drawText(positions.textAreaLeft(), positions.top() + fm.ascent(), bottom);
        if (fm.width(bottom) > positions.textAreaWidth()) {
            // draw a gradient to mask the text
            int gradientStart = positions.textAreaRight() - ELLIPSIS_GRADIENT_WIDTH + 1;
            QLinearGradient lg(gradientStart, 0, gradientStart + ELLIPSIS_GRADIENT_WIDTH, 0);
            lg.setColorAt(0, Qt::transparent);
            lg.setColorAt(1, backgroundColor);
            painter->fillRect(gradientStart, positions.top(), ELLIPSIS_GRADIENT_WIDTH, positions.firstLineHeight(), lg);
        }
    } else {
        // Description
        QString description = index.data(TaskModel::Description).toString();
        // Layout the description
        int leading = fm.leading();
        int height = 0;
        description.replace(QLatin1Char('\n'), QChar::LineSeparator);
        QTextLayout tl(description);
        tl.setAdditionalFormats(index.data(TaskModel::Task_t).value<Task>().formats);
        tl.beginLayout();
        while (true) {
            QTextLine line = tl.createLine();
            if (!line.isValid())
                break;
            line.setLineWidth(positions.textAreaWidth());
            height += leading;
            line.setPosition(QPoint(0, height));
            height += static_cast<int>(line.height());
        }
        tl.endLayout();
        tl.draw(painter, QPoint(positions.textAreaLeft(), positions.top()));

        QColor mix;
        mix.setRgb( static_cast<int>(0.7 * textColor.red()   + 0.3 * backgroundColor.red()),
                static_cast<int>(0.7 * textColor.green() + 0.3 * backgroundColor.green()),
                static_cast<int>(0.7 * textColor.blue()  + 0.3 * backgroundColor.blue()));
        painter->setPen(mix);

        const QString directory = QDir::toNativeSeparators(index.data(TaskModel::File).toString());
        int secondBaseLine = positions.top() + fm.ascent() + height + leading;
        if (index.data(TaskModel::FileNotFound).toBool()
                && !directory.isEmpty()) {
            QString fileNotFound = tr("File not found: %1").arg(directory);
            painter->setPen(Qt::red);
            painter->drawText(positions.textAreaLeft(), secondBaseLine, fileNotFound);
        } else {
            painter->drawText(positions.textAreaLeft(), secondBaseLine, directory);
        }
    }
    painter->setPen(textColor);

    // Paint FileArea
    QString file = index.data(TaskModel::File).toString();
    const int pos = file.lastIndexOf(QLatin1Char('/'));
    if (pos != -1)
        file = file.mid(pos +1);
    const int realFileWidth = fm.width(file);
    painter->setClipRect(positions.fileArea());
    painter->drawText(qMin(positions.fileAreaLeft(), positions.fileAreaRight() - realFileWidth),
                      positions.top() + fm.ascent(), file);
    if (realFileWidth > positions.fileAreaWidth()) {
        // draw a gradient to mask the text
        int gradientStart = positions.fileAreaLeft() - 1;
        QLinearGradient lg(gradientStart + ELLIPSIS_GRADIENT_WIDTH, 0, gradientStart, 0);
        lg.setColorAt(0, Qt::transparent);
        lg.setColorAt(1, backgroundColor);
        painter->fillRect(gradientStart, positions.top(), ELLIPSIS_GRADIENT_WIDTH, positions.firstLineHeight(), lg);
    }

    // Paint LineArea
    int line = index.data(TaskModel::Line).toInt();
    int movedLine = index.data(TaskModel::MovedLine).toInt();
    QString lineText;

    if (line == -1) {
        // No line information at all
    } else if (movedLine == -1) {
        // removed the line, but we had line information, show the line in ()
        QFont f = painter->font();
        f.setItalic(true);
        painter->setFont(f);
        lineText = QLatin1Char('(') + QString::number(line) + QLatin1Char(')');
    }  else if (movedLine != line) {
        // The line was moved
        QFont f = painter->font();
        f.setItalic(true);
        painter->setFont(f);
        lineText = QString::number(movedLine);
    } else {
        lineText = QString::number(line);
    }

    painter->setClipRect(positions.lineArea());
    const int realLineWidth = fm.width(lineText);
    painter->drawText(positions.lineAreaRight() - realLineWidth, positions.top() + fm.ascent(), lineText);
    painter->setClipRect(opt.rect);

    // Separator lines
    painter->setPen(QColor::fromRgb(150,150,150));
    painter->drawLine(0, opt.rect.bottom(), opt.rect.right(), opt.rect.bottom());
    painter->restore();
}

TaskWindowContext::TaskWindowContext(QWidget *widget)
  : IContext(widget)
{
    setWidget(widget);
    setContext(Context("Core.ProblemPane"));
}



void TaskWindow::setBuilder(Builder *builder)
{
    this->_builder = builder;
    if(!this->_builder)
    {
        return;
    }
    connect(_builder, SIGNAL(error()),this, SLOT(onError()));
    connect(_builder, SIGNAL(success()),this, SLOT(onError()));
    //connect(_builder, SIGNAL(success()),this, SLOT(onSuccess()));
    connect(_builder, SIGNAL(started()), this, SLOT(clearContents()));
}


void TaskWindow::onError()
{
    LatexOutputFilter f;
    f.setSource(_builder->getFile()->getFilename());
    f.run(_builder->output());
    foreach(const LatexLogEntry &logEntry, f.m_infoList)
    {
        if(logEntry.message.trimmed().isEmpty() && logEntry.type != LT_ERROR)
        {
            continue;
        }
        Task task(Task::Unknown, logEntry.message, logEntry.file, logEntry.oldline, "latex");
        switch(logEntry.type)
        {
        case LT_ERROR:
            task.type = Task::Error;
            task.icon = QApplication::style()->standardIcon(QStyle::SP_MessageBoxCritical);
            task.category = "error";
            break;
        case LT_WARNING:
            task.type = Task::Warning;
            task.icon = QIcon(QPixmap(":/data/img/warning.png"));//QApplication::style()->standardIcon(QStyle::SP_MessageBoxWarning);
            task.category = "warning";
            break;
        case LT_INFO:
        case LT_BADBOX:
            task.type = Task::Unknown;
            task.category = "notice";
            //task.icon = QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation);
            break;
        }
        this->addTask(task);
    }
}
