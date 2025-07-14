#include "startwindow.h"
#include "ui_startwindow.h"

StartWindow::StartWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StartWindow),
    m_mainWindow(nullptr) // 初始化 m_mainWindow 为 nullptr
{
    ui->setupUi(this);
}

StartWindow::~StartWindow()
{
    delete ui;
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