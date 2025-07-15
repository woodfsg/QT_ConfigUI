#include "startwindow.h"
#include "ui_startwindow.h"
#include <QHeaderView>

StartWindow::StartWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StartWindow),
    m_mainWindow(nullptr) // 初始化 m_mainWindow 为 nullptr
{
    ui->setupUi(this);

    init_programTable(); // 初始化程序表格
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