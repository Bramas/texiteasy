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

#ifndef TASKWINDOW_H
#define TASKWINDOW_H
#include <QList>
#include <QObject>
#include <QWidget>

#include <QDir>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QMenu>
#include <QToolButton>
#include <QScrollBar>
#include <QListView>
#include "taskmodel.h"
typedef QObject IOutputPane;
typedef QString Id;
class TaskHub;
class Task;
class Builder;
class WidgetTextEdit;
class TaskWindowPrivate;

// Show issues (warnings or errors) and open the editor on click.
class TaskWindow : public IOutputPane
{
    Q_OBJECT

public:
    TaskWindow();
    virtual ~TaskWindow();

    void delayedInitialization();

    int taskCount(Id category = 0) const;
    int warningTaskCount(Id category = 0) const;
    int errorTaskCount(Id category = 0) const;

    void setBuilder(Builder *builder);
    void setWidgetTextEdit(WidgetTextEdit * widgetTextEdit) { _widgetTextEdit = widgetTextEdit; }

    // IOutputPane
    QWidget *outputWidget();
    QList<QWidget *> toolBarWidgets() const;

    QString displayName() const { return tr("Issues"); }
    int priorityInStatusBar() const;
    void visibilityChanged(bool visible);

    bool canFocus() const;
    bool hasFocus() const;
    void setFocus();

    bool canNavigate() const;
    bool canNext() const;
    bool canPrevious() const;
    void goToNext();
    void goToPrev();
public slots:
    void clearContents();
    void onError();
    void popup(int flags) { emit showPage(flags); }

    void hide() { emit hidePage(); }
    void toggle(int flags) { emit togglePage(flags); }
    void navigateStateChanged() { emit navigateStateUpdate(); }
    void flash() { emit flashButton(); }
    void setIconBadgeNumber(int number) { emit setBadgeNumber(number); }




signals:
    void tasksChanged();
    void tasksCleared();

    void showPage(int flags);
    void hidePage();
    void togglePage(int flags);
    void navigateStateUpdate();
    void flashButton();
    void setBadgeNumber(int number);

private slots:
    void addTask(const Task &task);
    void addCategory(Id categoryId, const QString &displayName, bool visible);
    void removeTask(const Task &task);
    void updatedTaskFileName(unsigned int id, const QString &fileName);
    void updatedTaskLineNumber(unsigned int id, int line);
    void showTask(unsigned int id);
    void openTask(unsigned int id);
    void clearTasks(Id categoryId);
    void setCategoryVisibility(Id categoryId, bool visible);
    void currentChanged(const QModelIndex &index);

    void triggerDefaultHandler(const QModelIndex &index);
    void actionTriggered();
    void setShowWarnings(bool);
    void updateCategoriesMenu();
    void filterCategoryTriggered(QAction *action);

private:
    int sizeHintForColumn(int column) const;

    WidgetTextEdit * _widgetTextEdit;
    Builder * _builder;
    TaskWindowPrivate *d;
};





class TaskDelegate : public QStyledItemDelegate
{
    Q_OBJECT

    friend class TaskView; // for using Positions::minimumSize()

public:
    TaskDelegate(QObject * parent = 0);
    ~TaskDelegate();
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    // TaskView uses this method if the size of the taskview changes
    void emitSizeHintChanged(const QModelIndex &index);

public slots:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    void generateGradientPixmap(int width, int height, QColor color, bool selected) const;

    mutable int m_cachedHeight;
    mutable QFont m_cachedFont;

    /*
      Collapsed:
      +----------------------------------------------------------------------------------------------------+
      | TASKICONAREA  TEXTAREA                                                           FILEAREA LINEAREA |
      +----------------------------------------------------------------------------------------------------+

      Expanded:
      +----------------------------------------------------------------------------------------------------+
      | TASKICONICON  TEXTAREA                                                           FILEAREA LINEAREA |
      |               more text -------------------------------------------------------------------------> |
      +----------------------------------------------------------------------------------------------------+
     */
    class Positions
    {
    public:
        Positions(const QStyleOptionViewItemV4 &options, TaskModel *model) :
            m_totalWidth(options.rect.width()),
            m_maxFileLength(model->sizeOfFile(options.font)),
            m_maxLineLength(model->sizeOfLineNumber(options.font)),
            m_realFileLength(m_maxFileLength),
            m_top(options.rect.top()),
            m_bottom(options.rect.bottom())
        {
            int flexibleArea = lineAreaLeft() - textAreaLeft() - ITEM_SPACING;
            if (m_maxFileLength > flexibleArea / 2)
                m_realFileLength = flexibleArea / 2;
            m_fontHeight = QFontMetrics(options.font).height();
        }

        int top() const { return m_top + ITEM_MARGIN; }
        int left() const { return ITEM_MARGIN; }
        int right() const { return m_totalWidth - ITEM_MARGIN; }
        int bottom() const { return m_bottom; }
        int firstLineHeight() const { return m_fontHeight + 1; }
        static int minimumHeight() { return taskIconHeight() + 2 * ITEM_MARGIN; }

        int taskIconLeft() const { return left(); }
        static int taskIconWidth() { return TASK_ICON_SIZE; }
        static int taskIconHeight() { return TASK_ICON_SIZE; }
        int taskIconRight() const { return taskIconLeft() + taskIconWidth(); }
        QRect taskIcon() const { return QRect(taskIconLeft(), top(), taskIconWidth(), taskIconHeight()); }

        int textAreaLeft() const { return taskIconRight() + ITEM_SPACING; }
        int textAreaWidth() const { return textAreaRight() - textAreaLeft(); }
        int textAreaRight() const { return fileAreaLeft() - ITEM_SPACING; }
        QRect textArea() const { return QRect(textAreaLeft(), top(), textAreaWidth(), firstLineHeight()); }

        int fileAreaLeft() const { return fileAreaRight() - fileAreaWidth(); }
        int fileAreaWidth() const { return m_realFileLength; }
        int fileAreaRight() const { return lineAreaLeft() - ITEM_SPACING; }
        QRect fileArea() const { return QRect(fileAreaLeft(), top(), fileAreaWidth(), firstLineHeight()); }

        int lineAreaLeft() const { return lineAreaRight() - lineAreaWidth(); }
        int lineAreaWidth() const { return m_maxLineLength; }
        int lineAreaRight() const { return right(); }
        QRect lineArea() const { return QRect(lineAreaLeft(), top(), lineAreaWidth(), firstLineHeight()); }

    private:
        int m_totalWidth;
        int m_maxFileLength;
        int m_maxLineLength;
        int m_realFileLength;
        int m_top;
        int m_bottom;
        int m_fontHeight;

        static const int TASK_ICON_SIZE = 16;
        static const int ITEM_MARGIN = 2;
        static const int ITEM_SPACING = 2 * ITEM_MARGIN;
    };
};

#endif // TASKWINDOW_H
