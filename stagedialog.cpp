#include "stagedialog.h"
#include "ui_stagedialog.h"
#include <QJsonObject>


StageDialog::StageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StageDialog)
{
    ui->setupUi(this);
}

StageDialog::~StageDialog()
{
    delete ui;
}


QJsonObject StageDialog::getStageInfo() const
{
    QJsonObject stageInfo;
    stageInfo["stageName"] = ui->stageName->text();
    stageInfo["stageType"] = ui->stageType->currentText();
    return stageInfo;
}
