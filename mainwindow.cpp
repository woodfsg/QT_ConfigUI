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

    m_currentlyEditingItem = nullptr;

    // 初始化上一个页面索引
    m_lastParamConfigIndex = ui->paramConfig->currentIndex();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QJsonObject MainWindow::saveInputFormData()
{
    // 1. 创建一个空的 JSON 对象，用于存放表单数据
    QJsonObject formData;
    
    // （可选）进行数据校验
    // if (stepName.isEmpty()) {
    //     // 你可以在这里弹出警告或进行其他处理
    //     // qWarning() << "警告：步骤名称为空！";
    // }

    // 2. 将获取到的数据存入 JSON 对象
    //    使用英文作为 key 是一个非常好的习惯，便于跨平台和后续的程序处理
    formData["stepName"] = ui->inputStepNameLineEdit->text();
    formData["operatorName"] = ui->inputOperatorNameLineEdit->text();
    formData["programCategory"] = ui->programCategory->currentText();
    formData["subCategory"] = ui->subCategory->currentText();

    // 4. 返回这个包含所有数据的 JSON 对象
    return formData;
}

QJsonObject MainWindow::savePromptFormData()
{
    // 1. 创建一个空的 JSON 对象，用于存放表单数据
    QJsonObject formData;
    
    // （可选）进行数据校验
    // if (stepName.isEmpty()) {
    //     // 你可以在这里弹出警告或进行其他处理
    //     // qWarning() << "警告：步骤名称为空！";
    // }

    // 2. 将获取到的数据存入 JSON 对象
    //    使用英文作为 key 是一个非常好的习惯，便于跨平台和后续的程序处理

    formData["programCategory"] = ui->programCategory->currentText();
    formData["subCategory"] = ui->subCategory->currentText();

    formData["stepName"] = ui->promptStepNameLineEdit->text();
    formData["message"] = ui->promptMessageLineEdit->text();

    // 4. 返回这个包含所有数据的 JSON 对象
    return formData;
}

void MainWindow::loadInputFormData(QJsonObject &formData)
{
    // 1. 检查 formData 是否包含所需的键
    if (formData.contains("stepName")) {
        ui->inputStepNameLineEdit->setText(formData["stepName"].toString());
    }
    if (formData.contains("operatorName")) {
        ui->inputOperatorNameLineEdit->setText(formData["operatorName"].toString());
    }
    if (formData.contains("programCategory")) {
        ui->programCategory->setCurrentText(formData["programCategory"].toString());
    }
}

void MainWindow::loadPromptFormData(QJsonObject &formData)
{
    // 1. 检查 formData 是否包含所需的键
    if (formData.contains("stepName")) {
        ui->promptStepNameLineEdit->setText(formData["stepName"].toString());
    }
    if (formData.contains("message")) {
        ui->promptMessageLineEdit->setText(formData["message"].toString());
    }
    if (formData.contains("programCategory")) {
        ui->programCategory->setCurrentText(formData["programCategory"].toString());
    }
}

void MainWindow::on_programCategory_currentTextChanged(const QString &text)
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
                // 删除ProcessStage数据模型中的阶段
                QUuid stageId(currentItem->data(0, Qt::UserRole).toString());
                for (auto it = m_processStages.begin(); it != m_processStages.end(); ++it) {
                    if (it->id == stageId) {
                        m_processStages.erase(it); // 从数据模型中删除阶段
                        break;
                    }
                }
                // 从树形视图中删除当前顶层节点
                QTreeWidgetItem* takenItem = ui->treeWidget->takeTopLevelItem(index);
                delete takenItem; // 同样会级联删除所有子节点
            }
        } else {
            // 删除ProcessStep数据模型中的步骤
            QUuid stepId(currentItem->data(0, Qt::UserRole).toString());
            QUuid stageId(currentItem->parent()->data(0, Qt::UserRole).toString());
            for (auto& stage : m_processStages) {   
                if (stage.id == stageId) {
                    for (auto it = stage.steps.begin(); it != stage.steps.end(); ++it) {
                        if (it->id == stepId) {
                            stage.steps.erase(it); // 从数据模型中删除步骤
                            break;
                        }
                    }
                    break;
                }
            }
            // 从树形视图中删除当前节点
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
                item->setText(column, name + "_" + type);
                // 更新数据模型中的阶段名称
                QUuid stageId(item->data(column, Qt::UserRole).toString());
                for (auto& stage : m_processStages) {
                    if (stage.id == stageId) {
                        stage.stageName = name + "_" + type; // 更新阶段名称
                        break;
                    }
                }
            }
        } else {
            // 如果当前选中的是子节点
            QUuid stepId(item->data(column, Qt::UserRole).toString());
            QUuid stageId(item->parent()->data(column, Qt::UserRole).toString());
            for (auto& stage : m_processStages) {   
                if (stage.id == stageId) {
                    for (auto it = stage.steps.begin(); it != stage.steps.end(); ++it) {
                        if (it->id == stepId) {
                            switch (it->formIndex)
                            {
                                case 0: // 假设这个表单是 QStackedWidget 的第 0 页
                                    loadInputFormData(it->formData); // 在这里调用
                                    break;

                                case 1: // 假设这个表单是 QStackedWidget 的第 1 页
                                    loadPromptFormData(it->formData); // 在这里调用
                                    break;
                                
                            }
                            ui->paramConfig->setCurrentIndex(it->formIndex);
                            m_currentlyEditingItem = item; // 记录当前正在编辑的步骤项
                            break;
                        }
                    }
                    break;
                }
            }
        }

    }
}

void MainWindow::on_submitButton_clicked()
{
    // 1. 确定目标阶段
    QTreeWidgetItem *currentItem = ui->treeWidget->currentItem();
    if (currentItem==nullptr) {
        return;
    }

    QTreeWidgetItem *stageItem = nullptr;

    if (currentItem->parent() == nullptr) {
        // 如果当前选中的是顶层节点
        stageItem = currentItem;
    } else {
        // 如果当前选中的是子节点，获取其父节点
        stageItem = currentItem->parent();
    }

    
    // 2. 收集表单数据
    int formIndex = ui->paramConfig->currentIndex();
    QJsonObject formData;
    switch (formIndex)
    {
        case 0: // 假设这个表单是 QStackedWidget 的第 0 页
            formData = saveInputFormData(); // 在这里调用
            break;

        case 1: // 假设这个表单是 QStackedWidget 的第 1 页
            formData = savePromptFormData(); // 在这里调用
            break;

        case 2: // 假设这个表单是 QStackedWidget 的第 2 页
            return; // 如果是其他页面，直接返回
        
    }

    QString stepName = formData["stepName"].toString();     

    // 3. 判断是新增还是更新 (这部分逻辑和之前类似，但操作对象变了)
    if (m_currentlyEditingItem && m_currentlyEditingItem->parent() == stageItem) {
        // --- 更新步骤 ---
        QUuid stepId(m_currentlyEditingItem->data(0, Qt::UserRole).toString());
        QUuid stageId(stageItem->data(0, Qt::UserRole).toString());

        // 在数据模型中找到阶段，再找到步骤，然后更新它
        for (auto& stage : m_processStages) {
            if (stage.id == stageId) {
                for (auto& step : stage.steps) {
                    if (step.id ==  stepId) {
                        step.formData = formData;
                        step.stepName = stepName;
                        step.formIndex = formIndex;
                        break;
                    }
                }
                break;
            }
        }
        // 更新UI
        m_currentlyEditingItem->setText(0, stepName);
        m_currentlyEditingItem = nullptr;

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
    int formIndex = ui->paramConfig->currentIndex();
    switch (formIndex)
    {
        case 0: // 假设这个表单是 QStackedWidget 的第 0 页
            ui->inputStepNameLineEdit->clear();
            ui->inputOperatorNameLineEdit->clear();
            break;

        case 1: // 假设这个表单是 QStackedWidget 的第 1 页
            ui->promptStepNameLineEdit->clear();
            ui->promptMessageLineEdit->clear();
            break;
        
    }
}

// 新增槽函数：切换页面时清空上一个页面的编辑框
void MainWindow::on_paramConfig_currentChanged(int index)
{
    // 清空上一个页面的内容
    switch (m_lastParamConfigIndex)
    {
        case 0:
            ui->inputStepNameLineEdit->clear();
            ui->inputOperatorNameLineEdit->clear();
            break;
        case 1:
            ui->promptStepNameLineEdit->clear();
            ui->promptMessageLineEdit->clear();
            break;
        // 如有更多页面可继续添加
    }
    // 更新记录的上一个页面索引
    m_lastParamConfigIndex = index;
    // 清空当前正在编辑的步骤项
    m_currentlyEditingItem = nullptr; 
}
