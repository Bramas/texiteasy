#ifndef QMLLOADER_H
#define QMLLOADER_H

#include <QWidget>

namespace Ui {
class qmlLoader;
}

class qmlLoader : public QWidget
{
    Q_OBJECT

public:
    explicit qmlLoader(QWidget *parent = 0);
    ~qmlLoader();

private:
    Ui::qmlLoader *ui;
};

#endif // QMLLOADER_H
