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

    QJsonObject getStageInfo() const;

private:
    Ui::AddStageDialog *ui;

};

#endif 