#include "addstagedialog.h"
#include "ui_addstagedialog.h"


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

QString AddStageDialog::stageName() const
{
    return ui->stageName->text();
}

QString AddStageDialog::stageType() const
{
    return ui->stageType->currentText();
}
