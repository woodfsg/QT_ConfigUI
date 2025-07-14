#include "addstagedialog.h"
#include "ui_addstagedialog.h"
#include <QJsonObject>


AddStageDialog::AddStageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddStageDialog)
{
    ui->setupUi(this);
}

AddStageDialog::~AddStageDialog()
{
    delete ui;
}


QJsonObject AddStageDialog::getStageInfo() const
{
    QJsonObject stageInfo;
    stageInfo["stageName"] = ui->stageName->text();
    stageInfo["stageType"] = ui->stageType->currentText();
    return stageInfo;
}
