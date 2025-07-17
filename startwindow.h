#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include "mainwindow.h"

namespace Ui {
class StartWindow;
}

class StartWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StartWindow(QWidget *parent = nullptr);
    ~StartWindow();

private:
    Ui::StartWindow *ui;
    MainWindow *m_mainWindow;
    QStringList m_categories; // 存储类别列表
    static const int FILES_PER_PAGE = 10; // 每页显示的文件数
    QString m_programFilesPath = "C:/Users/li/Documents/ConfigUI/programFiles";
    QStringList m_sortedFiles; // 存储排序后的文件列表

    void init_programTable();
    void loadCategories(); // 从配置文件加载类别
    void saveCategories(); // 保存类别到配置文件
    void updateComboBox(); // 更新下拉框显示
    void loadProgramFiles(); // 加载并排序文件
    void updatePageTree(); // 更新页面树
    void displayFilesForPage(int page); // 显示指定页的文件
    void parseFileAttributes(const QString& fileName, QString& category, 
                           QString& name, QString& time, QString& description); // 解析文件属性

private slots:
    void on_editProgramPushButton_clicked();
    void on_addTypePushButton_clicked();
    void on_deleteCurrentTypePushButton_clicked();
    void handleMainWindowClosed();
    void on_pageTree_itemClicked(QTreeWidgetItem *item, int column); // 处理页面点击
};

#endif // STARTWINDOW_H
