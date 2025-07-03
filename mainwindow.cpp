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

        // 4. 将获取到的数据显示在主窗口的 Label 上
        ui->resultLabel->setText(QString("新阶段已添加：名称=%1, 类型=%2").arg(name).arg(type));
    }
    else
    {
        // 如果用户点击了 "Cancel"
        ui->resultLabel->setText("用户取消了添加操作。");
    }    

}

void MainWindow::on_deleteItem_clicked()
{
    return;
}

