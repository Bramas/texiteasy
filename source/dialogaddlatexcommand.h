#ifndef DIALOGADDLATEXCOMMAND_H
#define DIALOGADDLATEXCOMMAND_H

#include <QDialog>

namespace Ui {
class DialogAddLatexCommand;
}

class DialogAddLatexCommand : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddLatexCommand(QWidget *parent = 0);
    ~DialogAddLatexCommand();
    QString command();
    QString name();

private slots:
    void onCurrentItemChanged(int index);
    void selectCustomItem();
private:
    Ui::DialogAddLatexCommand *ui;
};

#endif // DIALOGADDLATEXCOMMAND_H
