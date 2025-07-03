#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    return;
}

void MainWindow::on_deleteItem_clicked()
{
    return;
}

