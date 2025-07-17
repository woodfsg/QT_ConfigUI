#ifndef ADDSTAGEDIALOG_H
#define ADDSTAGEDIALOG_H

#include <QDialog>

namespace Ui {
class StageDialog;
}

class StageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StageDialog(QWidget *parent = nullptr);
    ~StageDialog();

    QJsonObject getStageInfo() const;

private:
    Ui::StageDialog *ui;

};

#endif 