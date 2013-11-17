#ifndef DIALOGMACRO_H
#define DIALOGMACRO_H

#include <QDialog>
#include <QModelIndex>

namespace Ui {
class DialogMacros;
}
class QStandardItemModel;
class DialogMacros : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMacros(QWidget *parent = 0);
    ~DialogMacros();

public slots:
    void onClicked(QModelIndex index);
protected:
    void closeEvent(QCloseEvent *);
private slots:
    void setModified(bool b = true) { _modified = b; }
    void setModified(QString) { _modified = true; }
private:
    void saveLastClickedItem();
    void loadMacro(QString name);
private:
    Ui::DialogMacros *ui;
    QString _macrosPath;
    QStandardItemModel * _model;
    QModelIndex _lastClickedItem;
    bool _modified;
};

#endif // DIALOGMACRO_H
