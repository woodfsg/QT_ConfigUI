#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stagedialog.h"
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(const QString &filePath, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_filePath = filePath; // 存储文件路径
    ui->filePathLabel->setText(m_filePath);
    
    initProgramMap(); // 初始化程序类别和子类别的映射关系
    initParamMap();   // 初始化参数映射表
    initFormFieldMaps(); // 初始化表单字段映射

    m_currentlyEditingItem = nullptr;
    m_lastParamConfigIndex = ui->paramConfig->currentIndex();

    // 启动时加载数据
    loadStagesFromFile();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // 发出信号，通知关心此事的其他对象
    emit windowClosed();
    // 接受关闭事件，让窗口正常关闭
    event->accept();
}

void MainWindow::initProgramMap()
{
    // 初始化下拉框映射
    m_programMap["请选择程序类别"] = QStringList() << "请选择子类别";
    m_programMap["信息输入"] = QStringList() << "苹果";
    m_programMap["信息提示"] = QStringList() << "胡萝卜";
}

void MainWindow::initParamMap()
{
    m_paramMap.insert("苹果", FormType::FORM_INPUT);
    m_paramMap.insert("胡萝卜", FormType::FORM_PROMPT);
    m_paramMap.insert("请选择子类别", FormType::FORM_DEFAULT);
}

void MainWindow::initFormFieldMaps()
{
    // 初始化信息输入表单的字段映射
    m_formFieldMaps[FormType::FORM_INPUT]["stepName"] = "inputStepNameLineEdit";
    m_formFieldMaps[FormType::FORM_INPUT]["operatorName"] = "inputOperatorNameLineEdit";

    // 初始化信息提示表单的字段映射
    m_formFieldMaps[FormType::FORM_PROMPT]["stepName"] = "promptStepNameLineEdit";
    m_formFieldMaps[FormType::FORM_PROMPT]["message"] = "promptMessageLineEdit";
}

QJsonObject MainWindow::saveFormData(int formIndex)
{
    QJsonObject formData;
    // 获取当前索引对应的表单页面 widget
    QWidget* formWidget = ui->paramConfig->widget(formIndex);

    // 获取该表单的字段映射
    QMap<QString, QString> fieldMap = m_formFieldMaps.value(formIndex);

    // 遍历映射，保存数据
    for (auto it = fieldMap.constBegin(); it != fieldMap.constEnd(); ++it) {
        QString jsonKey = it.key();
        QString widgetObjectName = it.value();

        // 根据对象名查找对应的 UI 控件
        QWidget* widget = formWidget->findChild<QWidget*>(widgetObjectName);
        if (widget) {
            // 根据控件类型获取值并存入 JSON
            if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget)) {
                formData[jsonKey] = lineEdit->text();
            } else if (QComboBox* comboBox = qobject_cast<QComboBox*>(widget)) {
                 formData[jsonKey] = comboBox->currentText(); // 或者保存 currentIndex()
            }
            // TODO: 添加其他控件类型的处理，如 QCheckBox, QSpinBox 等
        }
    }
    formData["programCategory"] = ui->programCategory->currentText();
    formData["subCategory"] = ui->subCategory->currentText();
    return formData;
}

void MainWindow::loadFormData(int formIndex, const QJsonObject& formData)
{
    // 获取当前索引对应的表单页面 widget
    QWidget* formWidget = ui->paramConfig->widget(formIndex);

    // 获取该表单的字段映射
    QMap<QString, QString> fieldMap = m_formFieldMaps.value(formIndex);

    // 遍历映射，加载数据
    for (auto it = fieldMap.constBegin(); it != fieldMap.constEnd(); ++it) {
        QString jsonKey = it.key();
        QString widgetObjectName = it.value();

        if (formData.contains(jsonKey)) {
            // 根据对象名查找对应的 UI 控件
            QWidget* widget = formWidget->findChild<QWidget*>(widgetObjectName);
            if (widget) {
                // 根据控件类型设置值
                 if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget)) {
                    lineEdit->setText(formData[jsonKey].toString());
                } else if (QComboBox* comboBox = qobject_cast<QComboBox*>(widget)) {
                    // 根据文本设置当前项，或者根据索引设置
                    comboBox->setCurrentText(formData[jsonKey].toString());
                }
                // TODO: 添加其他控件类型的处理
            }
        }
    }
    ui->programCategory->setCurrentText(formData["programCategory"].toString());
    ui->subCategory->setCurrentText(formData["subCategory"].toString());
}

void MainWindow::clearFormData(int formIndex)
{
    // 获取当前索引对应的表单页面 widget
    QWidget* formWidget = ui->paramConfig->widget(formIndex);

    // 获取该表单的字段映射
    QMap<QString, QString> fieldMap = m_formFieldMaps.value(formIndex);

    // 遍历映射，清空数据
    for (auto it = fieldMap.constBegin(); it != fieldMap.constEnd(); ++it) {
        QString widgetObjectName = it.value();
        QWidget* widget = formWidget->findChild<QWidget*>(widgetObjectName);
        if (widget) {
            // 根据控件类型清空值
            if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget)) {
                lineEdit->clear();
            } else if (QComboBox* comboBox = qobject_cast<QComboBox*>(widget)) {
                // 重置 ComboBox，例如设置为第一个项
                comboBox->setCurrentIndex(0);
            }
            // TODO: 添加其他控件类型的处理
        }
    }
}

void MainWindow::on_programCategory_currentTextChanged(const QString &text)
{
    ui->subCategory->clear();
    QStringList subCategory = m_programMap.value(text);
    //change subCategory items
    ui->subCategory->addItems(subCategory);
}

void MainWindow::on_subCategory_currentTextChanged(const QString &text)
{
    int formIndex = m_paramMap.value(text);
    ui->paramConfig->setCurrentIndex(formIndex);
}

void MainWindow::on_addStageButton_clicked()
{
    // 1. 创建 StageDialog 的实例，变量名也相应修改
    StageDialog stageDialog(this);
    stageDialog.setWindowTitle("添加阶段");
    stageDialog.setWindowFlags(stageDialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // 2. 以模态方式显示对话框
    int result = stageDialog.exec();

    // 3. 判断用户是否点击了 "OK"
    if (result == QDialog::Accepted)
    {

        QJsonObject stageInfo = stageDialog.getStageInfo();

        // 如果是，就调用新的接口函数获取数据
        QString name = stageInfo["stageName"].toString();
        QString type = stageInfo["stageType"].toString();

        ProcessStage newStage;
        newStage.stageName = name+"_"+type;
        m_processStages.append(newStage);

        // 3. 在 QTreeWidget 中创建对应的顶级节点
        QTreeWidgetItem *stageItem = new QTreeWidgetItem(ui->stageTree);
        stageItem->setText(0, newStage.stageName);
        // stageItem->setIcon(0, QIcon(":/icons/stage.png")); // 可以给阶段一个不同的图标

        // 4. 将阶段的唯一ID存入节点，以便将来查找
        stageItem->setData(0, Qt::UserRole, newStage.id.toString());
    }

}

void MainWindow::on_deleteItemButton_clicked()
{
    QTreeWidgetItem* currentItem = ui->stageTree->currentItem();
    if (currentItem == nullptr) {
        QMessageBox::warning(this, "警告", "请选择要删除的阶段或步骤！");
        return;
    }

    int ret = QMessageBox::question(this, "确认删除", 
                "确定要删除选中的阶段或步骤吗？", 
                QMessageBox::Yes | QMessageBox::No);
    
    if(!(ret == QMessageBox::Yes)) {
        return; // 用户取消了删除
    }

    if (currentItem->parent() == nullptr) {
        // 如果当前选中的是顶层节点
        int index = ui->stageTree->indexOfTopLevelItem(currentItem);
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
            QTreeWidgetItem* takenItem = ui->stageTree->takeTopLevelItem(index);
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
    return;
}

void MainWindow::on_stageTree_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (item) {
        if (item->parent() == nullptr) {
            // 如果当前选中的是顶层节点
            StageDialog stageDialog(this);
            stageDialog.setWindowTitle("修改阶段");
            stageDialog.setWindowFlags(stageDialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
            int result = stageDialog.exec();
            if (result == QDialog::Accepted)
            {
                QJsonObject stageInfo = stageDialog.getStageInfo();

                QString name = stageInfo["stageName"].toString();
                QString type = stageInfo["stageType"].toString();
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
                            loadFormData(it->formIndex, it->formData);
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
    // 1. 确定目标阶段和插入位置
    QTreeWidgetItem *currentItem = ui->stageTree->currentItem();
    if (currentItem == nullptr) {
        QMessageBox::warning(this, "警告", "请先选择一个阶段或步骤！");
        return;
    }

    QTreeWidgetItem *stageItem = nullptr;
    int insertPos = -1; // -1 表示末尾插入

    if (currentItem->parent() == nullptr) {
        // 选中的是顶层节点（stage）
        stageItem = currentItem;
        insertPos = -1; // 末尾插入
    } else {
        // 选中的是子节点（step）
        stageItem = currentItem->parent();
        insertPos = stageItem->indexOfChild(currentItem) + 1;
    }

    // 2. 收集表单数据
    int formIndex = ui->paramConfig->currentIndex();
    QJsonObject formData;
    formData = saveFormData(formIndex);

    QString stepName = formData["stepName"].toString();

    // 3. 判断是新增还是更新
    if (m_currentlyEditingItem && m_currentlyEditingItem->parent() == stageItem) {
        // --- 更新步骤 ---
        QUuid stepId(m_currentlyEditingItem->data(0, Qt::UserRole).toString());
        QUuid stageId(stageItem->data(0, Qt::UserRole).toString());

        for (auto& stage : m_processStages) {
            if (stage.id == stageId) {
                for (auto& step : stage.steps) {
                    if (step.id == stepId) {
                        step.formData = formData;
                        step.stepName = stepName;
                        step.formIndex = formIndex;
                        break;
                    }
                }
                break;
            }
        }
        m_currentlyEditingItem->setText(0, stepName);
        m_currentlyEditingItem = nullptr;
        // 弹出提示
        QMessageBox::information(this, "提示", "步骤已更新！");
    } else {
        // --- 新增步骤 ---
        ProcessStep newStep;
        newStep.formIndex = formIndex;
        newStep.formData = formData;
        newStep.stepName = stepName;

        QUuid stageId(stageItem->data(0, Qt::UserRole).toString());
        for (auto& stage : m_processStages) {
            if (stage.id == stageId) {
                if (insertPos == -1) {
                    // 末尾插入
                    stage.steps.append(newStep);
                } else {
                    // 插入到指定位置
                    stage.steps.insert(insertPos, newStep);
                }
                break;
            }
        }

        // --- UI 插入 ---
        QTreeWidgetItem *stepItem = new QTreeWidgetItem();
        stepItem->setText(0, newStep.stepName);
        stepItem->setData(0, Qt::UserRole, newStep.id.toString());
        if (insertPos == -1 || insertPos >= stageItem->childCount()) {
            stageItem->addChild(stepItem);
        } else {
            stageItem->insertChild(insertPos, stepItem);
        }
        stageItem->setExpanded(true);
        // 弹出提示
        QMessageBox::information(this, "提示", "步骤已添加！");
    }
}

void MainWindow::on_resetButton_clicked()
{
    m_currentlyEditingItem = nullptr; // 清空当前正在编辑的步骤项
    int formIndex = ui->paramConfig->currentIndex();
    clearFormData(formIndex); // 清空当前表单数据
}

// 新增槽函数：切换页面时清空上一个页面的编辑框
void MainWindow::on_paramConfig_currentChanged(int index)
{
    switch (m_lastParamConfigIndex)
    {
        case FormType::FORM_INPUT:
            ui->inputStepNameLineEdit->clear();
            ui->inputOperatorNameLineEdit->clear();
            break;
        case FormType::FORM_PROMPT:
            ui->promptStepNameLineEdit->clear();
            ui->promptMessageLineEdit->clear();
            break;
    }
    // 更新记录的上一个页面索引
    m_lastParamConfigIndex = index;
    // 清空当前正在编辑的步骤项
    m_currentlyEditingItem = nullptr; 
}

// “保存”按钮的槽函数
void MainWindow::on_actionSave_triggered()
{
    saveStagesToFile();
}

// 将 m_processStages 中的数据保存到 JSON 文件
void MainWindow::saveStagesToFile()
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件进行写入: " + file.errorString());
        return;
    }

    QJsonArray stagesArray;
    for (const auto& stage : m_processStages) {
        stagesArray.append(stage.toJson());
    }

    QJsonDocument doc(stagesArray);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    QMessageBox::information(this, "成功", "数据已成功保存到 " + m_filePath);
}

// 从 JSON 文件加载数据到 m_processStages
void MainWindow::loadStagesFromFile()
{
    QFile file(m_filePath);
    if (!file.exists()) {
        return; // 文件不存在，不执行任何操作
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件进行读取: " + file.errorString());
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || doc.isEmpty()) {
        return;
    }

    m_processStages.clear();
    QJsonArray stagesArray = doc.array();
    for (const QJsonValue& value : stagesArray) {
        m_processStages.append(ProcessStage::fromJson(value.toObject()));
    }

    // 更新树形视图
    populateStageTree();
}

// “上移”按钮的槽函数
void MainWindow::on_upButton_clicked()
{
    QTreeWidgetItem *currentItem = ui->stageTree->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, "提示", "请选择一个要移动的项。");
        return;
    }

    if (currentItem->parent() == nullptr) {
        // --- 移动阶段 ---
        int index = ui->stageTree->indexOfTopLevelItem(currentItem);
        if (index > 0) {
            // 1. 更新UI
            QTreeWidgetItem *item = ui->stageTree->takeTopLevelItem(index);
            ui->stageTree->insertTopLevelItem(index - 1, item);
            ui->stageTree->setCurrentItem(item);

            // 2. 更新数据模型
            m_processStages.move(index, index - 1);
        }
    } else {
        // --- 移动步骤 ---
        QTreeWidgetItem *parentItem = currentItem->parent();
        int index = parentItem->indexOfChild(currentItem);
        if (index > 0) {
            // 1. 更新UI
            QTreeWidgetItem *item = parentItem->takeChild(index);
            parentItem->insertChild(index - 1, item);
            ui->stageTree->setCurrentItem(item);

            // 2. 更新数据模型
            QUuid stageId(parentItem->data(0, Qt::UserRole).toString());
            for (auto &stage : m_processStages) {
                if (stage.id == stageId) {
                    stage.steps.move(index, index - 1);
                    break;
                }
            }
        }
    }
}

// “下移”按钮的槽函数
void MainWindow::on_downButton_clicked()
{
    QTreeWidgetItem *currentItem = ui->stageTree->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, "提示", "请选择一个要移动的项。");
        return;
    }

    if (currentItem->parent() == nullptr) {
        // --- 移动阶段 ---
        int index = ui->stageTree->indexOfTopLevelItem(currentItem);
        if (index < ui->stageTree->topLevelItemCount() - 1) {
            // 1. 更新UI
            QTreeWidgetItem *item = ui->stageTree->takeTopLevelItem(index);
            ui->stageTree->insertTopLevelItem(index + 1, item);
            ui->stageTree->setCurrentItem(item);

            // 2. 更新数据模型
            m_processStages.move(index, index + 1);
        }
    } else {
        // --- 移动步骤 ---
        QTreeWidgetItem *parentItem = currentItem->parent();
        int index = parentItem->indexOfChild(currentItem);
        if (index < parentItem->childCount() - 1) {
            // 1. 更新UI
            QTreeWidgetItem *item = parentItem->takeChild(index);
            parentItem->insertChild(index + 1, item);
            ui->stageTree->setCurrentItem(item);

            // 2. 更新数据模型
            QUuid stageId(parentItem->data(0, Qt::UserRole).toString());
            for (auto &stage : m_processStages) {
                if (stage.id == stageId) {
                    stage.steps.move(index, index + 1);
                    break;
                }
            }
        }
    }
}

// 根据 m_processStages 的数据刷新 stageTree
void MainWindow::populateStageTree()
{
    ui->stageTree->clear(); // 清空现有的树
    for (const auto& stage : m_processStages) {
        QTreeWidgetItem *stageItem = new QTreeWidgetItem(ui->stageTree);
        stageItem->setText(0, stage.stageName);
        stageItem->setData(0, Qt::UserRole, stage.id.toString());

        for (const auto& step : stage.steps) {
            QTreeWidgetItem *stepItem = new QTreeWidgetItem(stageItem);
            stepItem->setText(0, step.stepName);
            stepItem->setData(0, Qt::UserRole, step.id.toString());
        }
        stageItem->setExpanded(true);
    }
}
