#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addstagedialog.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 初始化下拉框映射
    m_dataMap["请选择程序类别"] = QStringList() << "请选择子类别";
    m_dataMap["信息输入"] = QStringList() << "苹果" << "香蕉" << "橙子" << "葡萄";
    m_dataMap["信息提示"] = QStringList() << "胡萝卜" << "西兰花" << "菠菜" << "土豆";
}

MainWindow::~MainWindow()
{
    delete ui;
}

QJsonObject MainWindow::saveInputFormData()
{
    // 1. 创建一个空的 JSON 对象，用于存放表单数据
    QJsonObject formData;

    // 2. 通过 ui 指针和在 Designer 中设置的 objectName，获取每个输入框的内容
    //    .text() 方法可以获取 QLineEdit 当前的文本字符串
    QString stepName = ui->inputStepNameLineEdit->text();
    QString operatorName = ui->inputOperatorNameLineEdit->text();
    
    // （可选）进行数据校验
    if (stepName.isEmpty()) {
        // 你可以在这里弹出警告或进行其他处理
        // qWarning() << "警告：步骤名称为空！";
    }

    // 3. 将获取到的数据存入 JSON 对象
    //    使用英文作为 key 是一个非常好的习惯，便于跨平台和后续的程序处理
    formData["stepName"] = stepName;
    formData["operatorName"] = operatorName;

    // 4. 返回这个包含所有数据的 JSON 对象
    return formData;
}

QJsonObject MainWindow::savePromptFormData()
{
    return QJsonObject();
}

void MainWindow::on_category_currentTextChanged(const QString &text)
{
    ui->subCategory->clear();
    QStringList subCategory = m_dataMap.value(text);
    //change subCategory items
    ui->subCategory->addItems(subCategory);
    //change parameter configuration forms
    if (text == "请选择程序类别") {
        ui->paramConfig->setCurrentIndex(2);
    } else if (text == "信息输入") {
        ui->paramConfig->setCurrentIndex(0);
    } else if (text == "信息提示") {
        ui->paramConfig->setCurrentIndex(1);
    }
}

void MainWindow::on_addStageButton_clicked()
{
    // 1. 创建 AddStageDialog 的实例，变量名也相应修改
    AddStageDialog stageDialog(this);

    // 2. 以模态方式显示对话框
    int result = stageDialog.exec();

    // 3. 判断用户是否点击了 "OK"
    if (result == QDialog::Accepted)
    {
        // 如果是，就调用新的接口函数获取数据
        QString name = stageDialog.stageName();
        QString type = stageDialog.stageType();

        ProcessStage newStage;
        newStage.stageName = name+"_"+type;
        m_processStages.append(newStage);

        // 3. 在 QTreeWidget 中创建对应的顶级节点
        QTreeWidgetItem *stageItem = new QTreeWidgetItem(ui->treeWidget);
        stageItem->setText(0, newStage.stageName);
        // stageItem->setIcon(0, QIcon(":/icons/stage.png")); // 可以给阶段一个不同的图标

        // 4. 将阶段的唯一ID存入节点，以便将来查找
        stageItem->setData(0, Qt::UserRole, newStage.id.toString());
    }

}

void MainWindow::on_deleteItemButton_clicked()
{
    QTreeWidgetItem* currentItem = ui->treeWidget->currentItem();
    if (currentItem) { // 确保有节点被选中
        if (currentItem->parent() == nullptr) {
            // 如果当前选中的是顶层节点
            int index = ui->treeWidget->indexOfTopLevelItem(currentItem);
            if (index != -1) {
                QTreeWidgetItem* takenItem = ui->treeWidget->takeTopLevelItem(index);
                delete takenItem; // 同样会级联删除所有子节点
            }
        } else {
            currentItem->parent()->removeChild(currentItem);
            delete currentItem;
        }
    }
    return;
}

void MainWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (item) {
        if (item->parent() == nullptr) {
            // 如果当前选中的是顶层节点
            AddStageDialog stageDialog(this);
            int result = stageDialog.exec();
            if (result == QDialog::Accepted)
            {
                QString name = stageDialog.stageName();
                QString type = stageDialog.stageType();

                item->setText(0, name + "_" + type);
            }
        } else {
            // 如果当前选中的是子节点
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            ui->treeWidget->editItem(item, column);
        }

    }
}

void MainWindow::on_submitButton_clicked()
{
    // 1. 确定目标阶段
    QTreeWidgetItem *currentItem = ui->treeWidget->currentItem();
    if (currentItem->parent()!= nullptr) {
        return;
    }

    QTreeWidgetItem *stageItem = currentItem;
    
    // 2. 收集表单数据 (这部分不变)
    int formIndex = ui->paramConfig->currentIndex();
    QJsonObject formData;
    switch (formIndex)
    {
        case 0: // 假设这个表单是 QStackedWidget 的第 0 页
            formData = saveInputFormData(); // 在这里调用
            break;

        
    }

    QString stepName = formData["stepName"].toString();     

    // 3. 判断是新增还是更新 (这部分逻辑和之前类似，但操作对象变了)
    if (m_currentlyEditingItem && m_currentlyEditingItem->parent() == stageItem) {
        // --- 更新步骤 ---
        // QUuid stepId(m_currentlyEditingItem->data(0, Qt::UserRole).toString());
        // QUuid stageId(stageItem->data(0, Qt::UserRole).toString());

        // // 在数据模型中找到阶段，再找到步骤，然后更新它
        // for (auto& stage : m_processStages) {
        //     if (stage.id == stageId) {
        //         for (auto& step : stage.steps) {
        //             if (step.id == stepId) {
        //                 step.formData = formData;
        //                 step.stepName = stepName;
        //                 step.formIndex = formIndex;
        //                 break;
        //             }
        //         }
        //         break;
        //     }
        // }
        // // 更新UI
        // m_currentlyEditingItem->setText(0, stepName);
        // m_currentlyEditingItem = nullptr;
        // ui->submitButton->setText("提交");

    } else {
        // --- 新增步骤到选定阶段 ---
        // a. 创建新的 ProcessStep 实例
        ProcessStep newStep;
        newStep.formIndex = formIndex;
        newStep.formData = formData;
        newStep.stepName = stepName;

        // b. 在数据模型中，找到对应的阶段并将步骤添加进去
        QUuid stageId(stageItem->data(0, Qt::UserRole).toString());
        for (auto& stage : m_processStages) {
            if (stage.id == stageId) {
                stage.steps.append(newStep);
                break;
            }
        }
        
        // c. 在UI上，为阶段节点添加一个子节点
        QTreeWidgetItem *stepItem = new QTreeWidgetItem(stageItem);
        stepItem->setText(0, newStep.stepName);
        // stepItem->setIcon(0, QIcon(":/icons/step.png")); // 给步骤一个图标
        stepItem->setData(0, Qt::UserRole, newStep.id.toString()); // 存储步骤的ID
        
        stageItem->setExpanded(true); // 自动展开父节点
    }
}

void MainWindow::on_resetButton_clicked()
{
}
