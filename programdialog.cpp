#include "programdialog.h"
#include "ui_programdialog.h"

ProgramDialog::ProgramDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgramDialog)
{
    ui->setupUi(this);
}

ProgramDialog::~ProgramDialog()
{
    delete ui;
}

QJsonObject ProgramDialog::getProgramInfo() const
{
    QJsonObject programInfo;
    programInfo["programName"] = ui->programNameLineEdit->text();
    programInfo["programDescription"] = ui->programDescriptionTextEdit->toPlainText();
    programInfo["programCategory"] = ui->programCategoryComboBox->currentText();
    return programInfo;
}

void ProgramDialog::setProgramInfo(const QString& category, const QString& name,
                                   const QString& description)
{
    ui->programCategoryComboBox->setCurrentText(category);
    ui->programNameLineEdit->setText(name);
    ui->programDescriptionTextEdit->setText(description);
}

void ProgramDialog::setCategories(const QStringList& categories)
{
    ui->programCategoryComboBox->clear();
    ui->programCategoryComboBox->addItems(categories);
}
