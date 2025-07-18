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

    // 获取和设置程序信息的方法
    QJsonObject getProgramInfo() const;
    void setProgramInfo(const QString& category, const QString& name, 
                       const QString& description);
    void setCategories(const QStringList& categories); // 设置类别下拉框选项

private:
    Ui::ProgramDialog *ui;
};

#endif // PROGRAMDIALOG_H
