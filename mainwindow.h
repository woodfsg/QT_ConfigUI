#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QTreeWidget>
#include "processstage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // 添加枚举定义
    enum FormType {
        FORM_DEFAULT = 0,
        FORM_INPUT = 1,
        FORM_PROMPT = 2,
    };

    explicit MainWindow(const QString &filePath, QWidget *parent = nullptr);
    ~MainWindow() override;

signals:
    void windowClosed(); // 当窗口关闭时，将发出此信号

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui;
    QMap<QString, QStringList> m_programMap; // 用于存储程序类别和子类别的映射关系
    QMap<QString, int> m_paramMap;      //用于存储子类别和参数配置表单的映射关系
    QMap<int, QMap<QString, QString>> m_formFieldMaps; // 用于存储表单字段的映射关系
    QList<ProcessStage> m_processStages;    // 存储所有阶段和步骤的数据模型
    QTreeWidgetItem* m_currentlyEditingItem; // 当前正在编辑的步骤项
    int m_lastParamConfigIndex; // 上一个参数配置表单的索引
    QString m_filePath; // 用于存储文件路径

    QJsonObject saveFormData(int formIndex);
    void loadFormData(int formIndex, const QJsonObject& formData);
    void clearFormData(int formIndex);

    void initProgramMap(); // 初始化程序类别和子类别的映射关系
    void initParamMap(); 
    void initFormFieldMaps();

private slots:
    void on_programCategory_currentTextChanged(const QString &text);
    void on_subCategory_currentTextChanged(const QString &text);
    void on_addStageButton_clicked();
    void on_deleteItemButton_clicked();
    void on_stageTree_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_submitButton_clicked();
    void on_resetButton_clicked();
    void on_paramConfig_currentChanged(int index);
};
#endif // MAINWINDOW_H
