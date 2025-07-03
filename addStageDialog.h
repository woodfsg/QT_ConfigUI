#ifndef ADDSTAGEDIALOG_H
#define ADDSTAGEDIALOG_H

#include <QDialog>

namespace Ui {
class AddStageDialog;
}

class AddStageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddStageDialog(QWidget *parent = nullptr);
    ~AddStageDialog();

    QString stageName() const;
    QString stageType() const;

private:
    Ui::AddStageDialog *ui;

};

#endif 