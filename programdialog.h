#ifndef PROGRAMDIALOG_H
#define PROGRAMDIALOG_H

#include <QDialog>
#include <QJsonObject>

namespace Ui {
class ProgramDialog;
}

class ProgramDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProgramDialog(QWidget *parent = nullptr);
    ~ProgramDialog();

    QJsonObject getProgramInfo() const;

private:
    Ui::ProgramDialog *ui;
};

#endif // PROGRAMDIALOG_H
