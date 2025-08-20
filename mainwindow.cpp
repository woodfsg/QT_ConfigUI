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
    
    initProgramMap(); // 初始化程序类别和子类别的映射关系
    initParamMap();   // 初始化参数映射表
    initFormFieldMaps(); // 初始化表单字段映射
    initStageComboBox();

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

void MainWindow::initStageComboBox()
{
    // 获取所有子组件
    QList<QWidget*> allWidgets = this->findChildren<QWidget*>();
    
    // 遍历所有组件，查找包含"stageComboBox"的组件
    for (QWidget* widget : allWidgets) {
        if (widget->objectName().contains("StageComboBox")) {
            // 尝试将组件转换为MultiSelectComboBox类型
            MultiSelectComboBox* comboBox = qobject_cast<MultiSelectComboBox*>(widget);
            if (comboBox) {
                // 执行初始化操作
                comboBox->addItems(QStringList() << "Stage 1" << "Stage 2" << "Stage 3");
                comboBox->SetSearchBarHidden(true);
            }
        }
        else if (widget->objectName().contains("SolenoidValveComboBox")) {
            // 尝试将组件转换为MultiSelectComboBox类型
            MultiSelectComboBox* comboBox = qobject_cast<MultiSelectComboBox*>(widget);
            if (comboBox) {
                // 执行初始化操作
                QStringList solenoidValves;
                for(int i = 1; i <= 24; ++i) {
                    solenoidValves << QString::number(i) + "号电磁阀";
                }
                for(int i = 1; i <= 9; ++i) {
                    solenoidValves << "F0" + QString::number(i) + "电磁阀";
                }
                for(int i = 10; i <= 15; ++i) {
                    solenoidValves << "F" + QString::number(i) + "电磁阀";
                }
                comboBox->addItems(solenoidValves);
                comboBox->SetSearchBarHidden(true);
            }
        }
    }
}

void MainWindow::initProgramMap()
{
    // 初始化下拉框映射
    m_programMap["请选择程序类别"] = QStringList() << "请选择子类别";
    m_programMap["信息输入"] = QStringList() << "操作者信息" << "管路信息";
    m_programMap["信息提示"] = QStringList() << "信息提示";
    m_programMap["磁铁动作"] = QStringList() << "磁铁动作";
    m_programMap["液体转移"] = QStringList() << "按体积" << "按气泡感受器" << "按压力";
    m_programMap["离心运转"] = QStringList() << "单次运转" << "循环运转" << "停止运转";
    m_programMap["循环步骤"] = QStringList() << "单次运转 " << "循环运转 " << "停止运转 ";
}

void MainWindow::initParamMap()
{
    m_paramMap.insert("请选择子类别", FormType::FORM_DEFAULT);

    // 信息输入类别
    m_paramMap.insert("操作者信息", FormType::FORM_OPERATOR_INFO);
    m_paramMap.insert("管路信息", FormType::FORM_PIPELINE_INFO);

    // 信息提示类别
    m_paramMap.insert("信息提示", FormType::FORM_INFO_PROMPT);

    // 磁铁动作类别
    m_paramMap.insert("磁铁动作", FormType::FORM_MAGNET_ACTION);

    // 液体转移类别
    m_paramMap.insert("按体积", FormType::FORM_BY_VOLUME);
    m_paramMap.insert("按气泡感受器", FormType::FORM_BY_BUBBLE_SENSOR);
    m_paramMap.insert("按压力", FormType::FORM_BY_PRESSURE);

    // 离心运转类别
    m_paramMap.insert("单次运转", FormType::FORM_CENTRIFUGAL_SINGLE);
    m_paramMap.insert("循环运转", FormType::FORM_CENTRIFUGAL_CYCLE);
    m_paramMap.insert("停止运转", FormType::FORM_CENTRIFUGAL_STOP);

    // 循环步骤类别
    m_paramMap.insert("单次运转 ", FormType::FORM_CYCLE_STEP_SINGLE);
    m_paramMap.insert("循环运转 ", FormType::FORM_CYCLE_STEP_CYCLE);
    m_paramMap.insert("停止运转 ", FormType::FORM_CYCLE_STEP_STOP);

}

void MainWindow::initFormFieldMaps()
{
    // 初始化信息输入表单的字段映射
    m_formFieldMaps[FormType::FORM_OPERATOR_INFO]["stepName"] = "operatorInfoStepNameLineEdit";
    m_formFieldMaps[FormType::FORM_OPERATOR_INFO]["operatorName"] = "operatorInfoOperatorNameLineEdit";
    m_formFieldMaps[FormType::FORM_OPERATOR_INFO]["description"] = "operatorInfoDescriptionTextEdit";
    m_formFieldMaps[FormType::FORM_OPERATOR_INFO]["stage"] = "operatorInfoStageComboBox";

    // 初始化管路信息表单的字段映射
    m_formFieldMaps[FormType::FORM_PIPELINE_INFO]["stepName"] = "pipelineInfoStepNameLineEdit";
    m_formFieldMaps[FormType::FORM_PIPELINE_INFO]["description"] = "pipelineInfoDescriptionTextEdit";
    m_formFieldMaps[FormType::FORM_PIPELINE_INFO]["stage"] = "pipelineInfoStageComboBox";

    // 初始化信息提示表单的字段映射
    m_formFieldMaps[FormType::FORM_INFO_PROMPT]["stepName"] = "infoPromptStepNameLineEdit";
    m_formFieldMaps[FormType::FORM_INFO_PROMPT]["message"] = "infoPromptMessageLineEdit";

    // 初始化磁铁动作表单的字段映射
    m_formFieldMaps[FormType::FORM_MAGNET_ACTION]["stepName"] = "magnetActionStepNameLineEdit";
    m_formFieldMaps[FormType::FORM_MAGNET_ACTION]["magnetAction"] = "magnetActionComboBox";
    m_formFieldMaps[FormType::FORM_MAGNET_ACTION]["description"] = "magnetActionDescriptionTextEdit";
    m_formFieldMaps[FormType::FORM_MAGNET_ACTION]["stage"] = "magnetActionStageComboBox";
    m_formFieldMaps[FormType::FORM_MAGNET_ACTION]["automation"] = "magnetActionAutomationComboBox";

    //初始化按体积表单的字段映射
    m_formFieldMaps[FormType::FORM_BY_VOLUME]["stepName"] = "byVolumeStepNameLineEdit";
    m_formFieldMaps[FormType::FORM_BY_VOLUME]["volume"] = "byVolumeSpinBox";
    m_formFieldMaps[FormType::FORM_BY_VOLUME]["description"] = "byVolumeDescriptionTextEdit";
    m_formFieldMaps[FormType::FORM_BY_VOLUME]["stage"] = "byVolumeStageComboBox";
    m_formFieldMaps[FormType::FORM_BY_VOLUME]["automation"] = "byVolumeAutomationComboBox";
    m_formFieldMaps[FormType::FORM_BY_VOLUME]["solenoidValve"] = "byVolumeSolenoidValveComboBox";
    m_formFieldMaps[FormType::FORM_BY_VOLUME]["directionSetting"] = "byVolumeDirectionSettingComboBox";
    m_formFieldMaps[FormType::FORM_BY_VOLUME]["rotationalSpeed"] = "byVolumeRotationalSpeedSpinBox";

    // 初始化按气泡感受器表单的字段映射
    m_formFieldMaps[FormType::FORM_BY_BUBBLE_SENSOR]["stepName"] = "byBubbleSensorStepNameLineEdit";
    m_formFieldMaps[FormType::FORM_BY_BUBBLE_SENSOR]["bubbleSensor"] = "byBubbleSensorComboBox";
    m_formFieldMaps[FormType::FORM_BY_BUBBLE_SENSOR]["description"] = "byBubbleSensorDescriptionTextEdit";
    m_formFieldMaps[FormType::FORM_BY_BUBBLE_SENSOR]["stage"] = "byBubbleSensorStageComboBox";
    m_formFieldMaps[FormType::FORM_BY_BUBBLE_SENSOR]["automation"] = "byBubbleSensorAutomationComboBox";
    m_formFieldMaps[FormType::FORM_BY_BUBBLE_SENSOR]["solenoidValve"] = "byBubbleSensorSolenoidValveComboBox";
    m_formFieldMaps[FormType::FORM_BY_BUBBLE_SENSOR]["directionSetting"] = "byBubbleSensorDirectionSettingComboBox";
    m_formFieldMaps[FormType::FORM_BY_BUBBLE_SENSOR]["rotationalSpeed"] = "byBubbleSensorRotationalSpeedSpinBox";

    // 初始化按压力表单的字段映射
    m_formFieldMaps[FormType::FORM_BY_PRESSURE]["stepName"] = "byPressureStepNameLineEdit";
    m_formFieldMaps[FormType::FORM_BY_PRESSURE]["pressure"] = "byPressureComboBox";
    m_formFieldMaps[FormType::FORM_BY_PRESSURE]["description"] = "byPressureDescriptionTextEdit";
    m_formFieldMaps[FormType::FORM_BY_PRESSURE]["stage"] = "byPressureStageComboBox";
    m_formFieldMaps[FormType::FORM_BY_PRESSURE]["automation"] = "byPressureAutomationComboBox";
    m_formFieldMaps[FormType::FORM_BY_PRESSURE]["solenoidValve"] = "byPressureSolenoidValveComboBox";
    m_formFieldMaps[FormType::FORM_BY_PRESSURE]["directionSetting"] = "byPressureDirectionSettingComboBox";
    m_formFieldMaps[FormType::FORM_BY_PRESSURE]["rotationalSpeed"] = "byPressureRotationalSpeedSpinBox";
    m_formFieldMaps[FormType::FORM_BY_PRESSURE]["pressure"] = "byPressureNumberLineEdit";

    // 初始化离心运转单次运转表单的字段映射
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_SINGLE]["stepName"] = "centrifugalSingleStepNameLineEdit";
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_SINGLE]["centrifugalSpeed"] = "centrifugalSingleCentrifugalSpeedSpinBox";
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_SINGLE]["description"] = "centrifugalSingleDescriptionTextEdit";
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_SINGLE]["stage"] = "centrifugalSingleStageComboBox";
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_SINGLE]["automation"] = "centrifugalSingleAutomationComboBox";
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_SINGLE]["unit"] = "centrifugalSingleUnitComboBox";
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_SINGLE]["directionSetting"] = "centrifugalSingleDirectionSettingComboBox";
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_SINGLE]["accelerationTime"] = "centrifugalSingleAccelerationTimeSpinBox";
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_SINGLE]["continuousRunning"] = "centrifugalSingleContinuousRunningComboBox";
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_SINGLE]["continuousRunning"] = "centrifugalSingleContinuousRunningComboBox";
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_SINGLE]["runningTime"] = "centrifugalSingleRunningTimeSpinBox";

    // 初始化离心运转循环运转表单的字段映射
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_CYCLE]["stepName"] = "centrifugalCycleStepNameLineEdit";
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_CYCLE]["centrifugalSpeed"] = "centrifugalCycleCentrifugalSpeedSpinBox";
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_CYCLE]["description"] = "centrifugalCycleDescriptionTextEdit";
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_CYCLE]["stage"] = "centrifugalCycleStageComboBox";
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_CYCLE]["automation"] = "centrifugalCycleAutomationComboBox";
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_CYCLE]["unit"] = "centrifugalCycleUnitComboBox";
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_CYCLE]["accelerationTime"] = "centrifugalCycleAccelerationTimeSpinBox";
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_CYCLE]["directionSetting"] = "centrifugalCycleDirectionSettingComboBox";
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_CYCLE]["singleTime"] = "centrifugalCycleSingleTimeSpinBox";
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_CYCLE]["internalTime"] = "centrifugalCycleInternalTimeSpinBox";
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_CYCLE]["totalTime"] = "centrifugalCycleTotalTimeSpinBox";

    // 初始化离心运转停止运转表单的字段映射
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_STOP]["stepName"] = "centrifugalStopStepNameLineEdit";
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_STOP]["description"] = "centrifugalStopDescriptionTextEdit";
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_STOP]["stage"] = "centrifugalStopStageComboBox";
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_STOP]["automation"] = "centrifugalStopAutomationComboBox";
    m_formFieldMaps[FormType::FORM_CENTRIFUGAL_STOP]["stopRunning"] = "centrifugalStopRunningComboBox";
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
            } else if(MultiSelectComboBox* multiComboBox = qobject_cast<MultiSelectComboBox*>(widget)) {
                QStringList selectedItems = multiComboBox->currentText(); // 多选下拉框的选中项
                formData[jsonKey] = selectedItems.join(","); 
            } else if (QComboBox* comboBox = qobject_cast<QComboBox*>(widget)) {
                 formData[jsonKey] = comboBox->currentText(); // 或者保存 currentIndex()
            } else if (QTextEdit* textEdit = qobject_cast<QTextEdit*>(widget)) {
                formData[jsonKey] = textEdit->toPlainText();
            } else if (QSpinBox* spinBox = qobject_cast<QSpinBox*>(widget)) {
                formData[jsonKey] = spinBox->value();
            } else if (QDoubleSpinBox* doubleSpinBox = qobject_cast<QDoubleSpinBox*>(widget)) {
                formData[jsonKey] = doubleSpinBox->value();
            }
            
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
                } else if(MultiSelectComboBox* multiComboBox = qobject_cast<MultiSelectComboBox*>(widget))
                {
                    // 设置多选下拉框的选中项
                    QStringList selectedItems = formData[jsonKey].toString().split(",");
                    multiComboBox->setCurrentText(selectedItems);
                } else if (QComboBox* comboBox = qobject_cast<QComboBox*>(widget)) {
                    // 根据文本设置当前项，或者根据索引设置
                    comboBox->setCurrentText(formData[jsonKey].toString());
                } else if (QTextEdit* textEdit = qobject_cast<QTextEdit*>(widget)) {
                    textEdit->setPlainText(formData[jsonKey].toString());
                } else if (QSpinBox* spinBox = qobject_cast<QSpinBox*>(widget)) {
                    spinBox->setValue(formData[jsonKey].toInt());
                } else if (QDoubleSpinBox* doubleSpinBox = qobject_cast<QDoubleSpinBox*>(widget)) {
                    doubleSpinBox->setValue(formData[jsonKey].toDouble());
                }
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
            } else if(MultiSelectComboBox* multiComboBox = qobject_cast<MultiSelectComboBox*>(widget)) {
                multiComboBox->TextClear(); // 清空多选下拉框的选中项
            } else if (QComboBox* comboBox = qobject_cast<QComboBox*>(widget)) {
                // 重置 ComboBox，例如设置为第一个项
                comboBox->setCurrentIndex(0);
            } else if (QTextEdit* textEdit = qobject_cast<QTextEdit*>(widget)) {
                textEdit->clear();
            } else if (QSpinBox* spinBox = qobject_cast<QSpinBox*>(widget)) {
                spinBox->setValue(0); // 重置为0
            } else if (QDoubleSpinBox* doubleSpinBox = qobject_cast<QDoubleSpinBox*>(widget)) {
                doubleSpinBox->setValue(0.0); // 重置为0.0
            }
            
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
        m_processStages.removeAt(index);
        ui->stageTree->takeTopLevelItem(index);
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
    }
    if(m_currentlyEditingItem == currentItem || (m_currentlyEditingItem && m_currentlyEditingItem->parent() == currentItem)) {
        m_currentlyEditingItem = nullptr; // 清空当前正在编辑的步骤项
        clearFormData(ui->paramConfig->currentIndex()); // 清空当前表单数据
    }
    delete currentItem; 
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
    // 清空当前表单数据
    clearFormData(formIndex);
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
    // 清空上一个页面的编辑框
    clearFormData(m_lastParamConfigIndex);
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

    QMessageBox::information(this, "成功", "数据已成功保存。");
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
        stageItem->setExpanded(false);
    }
}

// 处理离心单次运转持续运转选项改变的槽函数
void MainWindow::on_centrifugalSingleContinuousRunningComboBox_currentTextChanged(const QString &text)
{
    QSpinBox* runningTimeSpinBox = ui->centrifugalSingleRunningTimeSpinBox;
    if (runningTimeSpinBox) {
        // 如果选择"是"，禁用运行时间输入框
        if (text == "是") {
            runningTimeSpinBox->setEnabled(false);
            runningTimeSpinBox->setValue(0); // 清空数值
        }
        // 如果选择"否"，启用运行时间输入框
        else if (text == "否") {
            runningTimeSpinBox->setEnabled(true);
        }
    }
}
