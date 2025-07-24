#include "startwindow.h"
#include "ui_startwindow.h"
#include "programdialog.h"
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QLineEdit>
#include <QDir>
#include <QDateTime>
#include <algorithm>
#include <QFileDialog>
#include <QStandardPaths>
#include <QSet>
#include <QDebug>

const int FILES_PER_PAGE = 10; // 每页显示的文件数量

StartWindow::StartWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StartWindow),
    m_mainWindow(nullptr)
{
    ui->setupUi(this);

    init_programTable();
    loadCategories();
    updateComboBox();
    
    // 加载程序文件并显示第一页
    loadProgramFiles();

}

StartWindow::~StartWindow()
{
    delete ui;
}

void StartWindow::init_programTable()
{
    //设置表格的列宽
    ui->programTable->setColumnWidth(2, 150);
    QHeaderView *hor_header = ui->programTable->horizontalHeader();
    hor_header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    hor_header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    hor_header->setSectionResizeMode(3, QHeaderView::Stretch);
    // 设置表格的行高
    QHeaderView *ver_header = ui->programTable->verticalHeader();
    ver_header->setSectionResizeMode(QHeaderView::Stretch);
    //设置表格的行标居中
    ver_header->setDefaultAlignment(Qt::AlignCenter);
}

void StartWindow::handleMainWindowClosed()
{
    this->show();
    if(m_mainWindow) {
        m_mainWindow->deleteLater();
        m_mainWindow = nullptr;
    }
}

void StartWindow::on_editProgramContentPushButton_clicked()
{
    // 获取选中的行
    QList<QTableWidgetItem*> items = ui->programTable->selectedItems();
    if (items.isEmpty()) {
        QMessageBox::warning(this, "警告", "请选择要编辑的程序！");
        return;
    }

    int row = items.first()->row();
    int fileIndex = (m_currentPage - 1) * FILES_PER_PAGE + row;
    QString filePath = m_programFilesPath + "/" + m_sortedFiles[fileIndex];

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

void StartWindow::loadProgramFiles()
{
    // 清空文件列表
    m_sortedFiles.clear();
    
    // 检查并创建目录（如果不存在）
    QDir dir(m_programFilesPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    // 获取所有.json文件
    QStringList filters;
    filters << "*.json";
    m_sortedFiles = dir.entryList(filters, QDir::Files | QDir::NoDotAndDotDot);

    // 根据创建时间排序
    std::sort(m_sortedFiles.begin(), m_sortedFiles.end(), 
        [](const QString& a, const QString& b) {
            QString timeA = a.split("_")[2];
            QString timeB = b.split("_")[2];
            return timeA < timeB;
        });

    // 更新页面树
    updatePageTree();
    // 显示第一页的文件
    displayFilesForPage(1);
}

void StartWindow::updatePageTree()
{
    // 清空页面树
    ui->pageTree->clear();

    // 计算总页数
    int totalPages = (m_sortedFiles.size() + FILES_PER_PAGE - 1) / FILES_PER_PAGE;

    // 直接添加页数为顶层节点
    for (int i = 1; i <= totalPages; ++i) {
        QTreeWidgetItem* pageItem = new QTreeWidgetItem(ui->pageTree);
        pageItem->setText(0, QString::number(i));
    }
}

void StartWindow::displayFilesForPage(int page)
{
    // 清空表格
    ui->programTable->clearContents();
    for (int i = 0; i < ui->programTable->rowCount(); ++i) {
        ui->programTable->setVerticalHeaderItem(i, new QTableWidgetItem(""));
    }

    // 计算起始和结束索引
    int startIndex = (page - 1) * FILES_PER_PAGE;
    int endIndex = qMin(startIndex + FILES_PER_PAGE, m_sortedFiles.size());

    // 显示文件属性
    for (int i = startIndex; i < endIndex; i++) {
        QString category, name, time, description;
        parseFileAttributes(m_sortedFiles[i], category, name, time, description);

        // 设置行号（从1开始）
        QTableWidgetItem* indexItem = new QTableWidgetItem(QString::number(i + 1));
        ui->programTable->setVerticalHeaderItem(i - startIndex, indexItem);

        // 设置各列的值
        ui->programTable->setItem(i - startIndex, 0, new QTableWidgetItem(category));
        ui->programTable->setItem(i - startIndex, 1, new QTableWidgetItem(name));
        ui->programTable->setItem(i - startIndex, 2, new QTableWidgetItem(time));
        ui->programTable->setItem(i - startIndex, 3, new QTableWidgetItem(description));
    }
}

void StartWindow::parseFileAttributes(const QString& fileName, QString& category, 
                                    QString& name, QString& time, QString& description)
{
    // 移除.json后缀
    QString baseName = fileName;
    baseName.chop(5); // 移除".json"

    // 分割文件名获取属性
    QStringList parts = baseName.split("_");
    if (parts.size() >= 4) {
        category = parts[0];
        name = parts[1];
        time = parts[2];
        description = parts[3];
    }
}

void StartWindow::on_pageTree_itemClicked(QTreeWidgetItem *item, int column)
{
    // 只处理顶层节点的点击
    if (item->parent() == nullptr) {
        int page = item->text(column).toInt();
        displayFilesForPage(page);
        m_currentPage = page; // 更新当前页码
    }
}

void StartWindow::on_addProgramPushButton_clicked()
{
    ProgramDialog dialog(this);
    dialog.setCategories(m_categories);
    
    if (dialog.exec() == QDialog::Accepted) {
        QJsonObject info = dialog.getProgramInfo();
        QString category = info["programCategory"].toString();
        QString name = info["programName"].toString();
        QString description = info["programDescription"].toString();

        if (category.isEmpty() || name.isEmpty()) {
            QMessageBox::warning(this, "警告", "类别和名称不能为空！");
            return;
        }
        
        // 创建新文件
        QString fileName = createFileName(category, name, description);
        QFile file(m_programFilesPath + "/" + fileName);
        if (file.open(QIODevice::WriteOnly)) {
            file.close();
            // 更新显示
            loadProgramFiles();
        }
    }
}

void StartWindow::on_deleteProgramPushButton_clicked()
{
    // 获取选中的行
    QList<QTableWidgetItem*> items = ui->programTable->selectedItems();
    if (items.isEmpty()) {
        QMessageBox::warning(this, "警告", "请选择要删除的程序！");
        return;
    }

    QSet<int> selectedRows;
    for (const QTableWidgetItem* item : items) {
        selectedRows.insert(item->row());
    }

    int ret = QMessageBox::question(this, "确认删除", 
                "确定要删除选中的程序吗？", 
                QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        int successCount = 0;
        int failureCount = 0;
        for (int row : selectedRows) {
            int fileIndex = (m_currentPage - 1) * FILES_PER_PAGE + row;
            if (fileIndex < m_sortedFiles.size()) {
                // 删除文件
                QFile file(m_programFilesPath + "/" + m_sortedFiles[fileIndex]);
                if (file.remove()) {
                    successCount++;
                } else {
                    failureCount++;
                    qDebug() << "无法删除文件：" << m_programFilesPath + "/" + m_sortedFiles[fileIndex];
                    
                }
            }
        }

        loadProgramFiles();

        QMessageBox::information(this, "删除完成", 
                                QString("成功删除 %1 个文件。\n失败 %2 个。")
                                .arg(successCount).arg(failureCount));
    }
}

void StartWindow::on_editProgramPushButton_clicked()
{
    // 获取选中的行
    QList<QTableWidgetItem*> items = ui->programTable->selectedItems();
    if (items.isEmpty()) {
        QMessageBox::warning(this, "警告", "请选择要修改的程序！");
        return;
    }

    int row = items.first()->row();
    int fileIndex = (m_currentPage - 1) * FILES_PER_PAGE + row;
    
    if (fileIndex < m_sortedFiles.size()) {
        QString category, name, time, description;
        parseFileAttributes(m_sortedFiles[fileIndex], category, name, time, description);
        
        ProgramDialog dialog(this);
        dialog.setCategories(m_categories);
        dialog.setProgramInfo(category, name, description);
        
        if (dialog.exec() == QDialog::Accepted) {
            QJsonObject info = dialog.getProgramInfo();
            QString newCategory = info["programCategory"].toString();
            QString newName = info["programName"].toString();
            QString newDescription = info["programDescription"].toString();

            if (newCategory.isEmpty() || newName.isEmpty()) {
                QMessageBox::warning(this, "警告", "类别和名称不能为空！");
                return;
            }
            
            // 创建新文件名
            QString newFileName = createFileName(newCategory, newName, newDescription);
            QString oldFilePath = m_programFilesPath + "/" + m_sortedFiles[fileIndex];
            QString newFilePath = m_programFilesPath + "/" + newFileName;
            
            // 重命名文件
            QFile file(oldFilePath);
            if (file.rename(newFilePath)) {
                // 更新显示
                m_sortedFiles[fileIndex] = newFileName; // 更新文件列表
                displayFilesForPage(m_currentPage); // 重新显示当前页
            }
        }
    }
}

QString StartWindow::createFileName(const QString& category, const QString& name, 
                                 const QString& description)
{
    // 获取当前时间
    QString timeStr = QDateTime::currentDateTime()
                     .toString("yyyy-MM-dd HH-mm-ss");
    
    // 创建文件名
    return QString("%1_%2_%3_%4.json")
           .arg(category)
           .arg(name)
           .arg(timeStr)
           .arg(description);
}

// 导出按钮点击事件处理
void StartWindow::on_exportPushButton_clicked()
{
    // 1. 获取选中的行
    QList<QTableWidgetItem*> selectedItems = ui->programTable->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "警告", "请选择要导出的程序！");
        return;
    }

    QString downloadPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation); // 2. 获取下载文件夹路径

    // 2. 弹出文件对话框选择导出目录
    QString destDir = QFileDialog::getExistingDirectory(this, "选择导出目录",
                                                       downloadPath, // 使用下载文件夹路径作为初始路径
                                                       QFileDialog::ShowDirsOnly
                                                       | QFileDialog::DontResolveSymlinks);

    if (destDir.isEmpty()) {
        return; // 用户取消了选择
    }

    // 3. 复制文件
    // 使用 QSet 存储选中的行号，避免因选择多个单元格而重复处理同一行
    QSet<int> selectedRows;
    for (const QTableWidgetItem* item : selectedItems) {
        selectedRows.insert(item->row());
    }

    int successCount = 0;
    int failureCount = 0;

    for (int row : selectedRows) {
        // 计算文件在 m_sortedFiles 列表中的实际索引
        int fileIndex = (m_currentPage - 1) * FILES_PER_PAGE + row;
        
        if (fileIndex < m_sortedFiles.size()) {
            QString sourceFileName = m_sortedFiles[fileIndex];
            QString sourceFilePath = m_programFilesPath + "/" + sourceFileName;
            QString destFilePath = destDir + "/" + sourceFileName;

            // 执行复制操作
            if (QFile::copy(sourceFilePath, destFilePath)) {
                successCount++;
            } else {
                failureCount++;
                qDebug() << "无法复制文件：" << sourceFilePath << " 到 " << destFilePath;
            }
        }
    }

    // 4. 显示结果
    QMessageBox::information(this, "导出完成", 
                             QString("成功导出 %1 个文件。\n失败 %2 个。")
                             .arg(successCount).arg(failureCount));
}
