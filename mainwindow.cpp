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

void MainWindow::on_addStage_clicked()
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

        QTreeWidgetItem *stage = new QTreeWidgetItem();
        stage->setText(0,name+"_"+type);
        ui->treeWidget->addTopLevelItem(stage);
    }

}

void MainWindow::on_deleteItem_clicked()
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
