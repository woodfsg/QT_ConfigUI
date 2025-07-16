#include "startwindow.h"
#include "ui_startwindow.h"
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QLineEdit>

StartWindow::StartWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StartWindow),
    m_mainWindow(nullptr) // 初始化 m_mainWindow 为 nullptr
{
    ui->setupUi(this);

    init_programTable(); // 初始化程序表格
    loadCategories(); // 加载类别
    updateComboBox(); // 更新下拉框
}

StartWindow::~StartWindow()
{
    delete ui;
}

void StartWindow::init_programTable()
{
    //设置表格的列宽
    ui->programTable->setColumnWidth(0, 50);
    ui->programTable->setColumnWidth(3, 150);
    QHeaderView *hor_header = ui->programTable->horizontalHeader();
    hor_header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    hor_header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    hor_header->setSectionResizeMode(4, QHeaderView::Stretch);
    // 设置表格的行高
    QHeaderView *ver_header = ui->programTable->verticalHeader();
    ver_header->setSectionResizeMode(QHeaderView::Stretch);
    //设置表格的行标居中
    ver_header->setDefaultAlignment(Qt::AlignCenter);


    // 设置表格的 ToolTip
    // for (int row = 0; row < ui->programTable->rowCount(); ++row) {
    //     for (int col = 0; col < ui->programTable->columnCount(); ++col) {
    //         QTableWidgetItem *item = ui->programTable->item(row, col);
    //         if (item) {
    //             // 将单元格自身的文本设置为它的 ToolTip
    //             item->setToolTip(item->text());
    //         }
    //     }
    // }
}

void StartWindow::handleMainWindowClosed()
{
    this->show();
    if(m_mainWindow) {
        m_mainWindow->deleteLater();
        m_mainWindow = nullptr;
    }
}

void StartWindow::on_editProgramPushButton_clicked()
{
    QString filePath = "C:\\Users\\li\\Downloads\\github-recovery-codes.txt";

    // 如果 m_mainWindow 还存在 (虽然不太可能，但这是好的编程习惯)
    if (m_mainWindow) {
        m_mainWindow->deleteLater();
    }

    // 1. 创建 MainWindow 实例
    m_mainWindow = new MainWindow(filePath);

    // 2. 连接 MainWindow 的关闭信号到 StartWindow 的处理槽函数
    connect(m_mainWindow, &MainWindow::windowClosed, this, &StartWindow::handleMainWindowClosed);

    // 3. 隐藏 StartWindow
    this->hide();

    // 4. 显示 MainWindow
    m_mainWindow->show();
}

void StartWindow::loadCategories()
{
    QSettings settings("ConfigUI", "StartWindow");
    m_categories = settings.value("categories", QStringList()).toStringList();
    
    // 如果没有保存的类别，添加一些默认类别
    // if (m_categories.isEmpty()) {
    //     m_categories << "默认类别" << "测试类别" << "系统类别";
    // }
}

void StartWindow::saveCategories()
{
    QSettings settings("ConfigUI", "StartWindow");
    settings.setValue("categories", m_categories);
}

void StartWindow::updateComboBox()
{
    ui->showTypeComboBox->clear();
    ui->showTypeComboBox->addItems(m_categories);
}

void StartWindow::on_addTypePushButton_clicked()
{
    QInputDialog inputDialog(this);
    inputDialog.setWindowTitle("新增类别");
    inputDialog.setLabelText("请输入类别名称:");
    inputDialog.setTextValue("");
    inputDialog.setInputMode(QInputDialog::TextInput);

    // 去掉问号
    inputDialog.setWindowFlags(inputDialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

    bool ok = false;
    if (inputDialog.exec() == QDialog::Accepted) {
        ok = true;
    }
    QString categoryName = inputDialog.textValue();

    if (ok && !categoryName.isEmpty()) {
        // 检查类别是否已存在
        if (m_categories.contains(categoryName)) {
            return;
        }
        
        // 添加新类别
        m_categories.append(categoryName);
        updateComboBox();
        saveCategories();
        
        // 设置新添加的类别为当前选中项
        ui->showTypeComboBox->setCurrentText(categoryName);
    }
}

void StartWindow::on_deleteCurrentTypePushButton_clicked()
{
    if (m_categories.isEmpty()) {
        QMessageBox::warning(this, "警告", "没有可删除的类别！");
        return;
    }
    
    QString currentCategory = ui->showTypeComboBox->currentText();
    if (currentCategory.isEmpty()) {
        QMessageBox::warning(this, "警告", "请选择要删除的类别！");
        return;
    }
    
    // 确认删除
    int ret = QMessageBox::question(this, 
                                   "确认删除", 
                                   QString("确定要删除类别 \"%1\" 吗？").arg(currentCategory),
                                   QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        m_categories.removeAll(currentCategory);
        updateComboBox();
        saveCategories();
    }
}
